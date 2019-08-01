// #include <bits/stdc++.h>
#include <iostream>
#include <vector>
#include <tuple>
#include <forward_list>
#include <algorithm>
#include "akari.h"
using namespace std;

// #define DEBUG_AKARI_CPP

namespace aka{
//请在命名空间内编写代码，否则后果自负

typedef vector<vector<int> > GameMap;
typedef tuple<int, int> Coord;

enum CellType
{
    mark_noplaceable    = -4,
    mark_lit            = -3,
    white               = -2,
    black_nonumber      = -1,
    light_bulb          = 5
};

inline
const Coord getMapShape(const GameMap &map)
{
    return std::make_tuple(map.size(), map.at(0).size());
}

void printMap(const GameMap &map)
{
    cout << "+";
    for (auto &_: map.at(0)) {
        cout << "--+";
    }
    cout << endl;
    for (auto &row : map) {
        cout << "|";
        for (auto &cell : row) {
            switch (cell) {
                case CellType::mark_noplaceable: cout << "Ｘ"; break;
                case CellType::mark_lit: cout << "Ｌ"; break;
                case CellType::white: cout << "  "; break;
                case CellType::black_nonumber: cout << "＃"; break;
                case 0: cout << "０"; break;
                case 1: cout << "１"; break;
                case 2: cout << "２"; break;
                case 3: cout << "３"; break;
                case 4: cout << "４"; break;
                case CellType::light_bulb: cout << "？"; break;
                default: cout << " " << cell; break;
            }
            cout << "|";
        }
        cout << "\n+";
        for (auto &_: row) {
            cout << "--+";
        }
        cout << endl;
    }
}

static inline
const vector<Coord> _getCrossAround(const int x, const int y)
{
    return vector<Coord>{
        std::make_tuple(x - 1, y), std::make_tuple(x + 1, y),
        std::make_tuple(x, y - 1), std::make_tuple(x, y + 1)
    };
}

static
void _prune_black_zero(GameMap &map)
{
    int h, w; std::tie(h, w) = getMapShape(map);
    for (int x = 0; x != h - 1; ++x) {
        for (int y = 0; y != w - 1; ++y) {
            if (map.at(x).at(y + 1) == 0 || map.at(x + 1).at(y) == 0) {
                if (map.at(x).at(y) == CellType::white) {
                    map.at(x).at(y) = CellType::mark_noplaceable;
                }
            }
            if (map.at(x).at(y) == 0) {
                if (map.at(x).at(y + 1) == CellType::white) {
                    map.at(x).at(y + 1) = CellType::mark_noplaceable;
                }
                if (map.at(x + 1).at(y) == CellType::white) {
                    map.at(x + 1).at(y) = CellType::mark_noplaceable;
                }
            }
        }
    }
}

static
bool _checkPlaceable(GameMap &map, const int x, const int y, bool leave_markup=false)
{
    // Check if cell is placeable
    if (map.at(x).at(y) == CellType::light_bulb) { return true; }
    if (map.at(x).at(y) != CellType::white) { return false; }
    int h, w; std::tie(h, w) = getMapShape(map);
    // Check row for existing light bulb
    for (int iy = y - 1; iy != -1; --iy) {
        auto &cell = map.at(x).at(iy);
        if (cell == CellType::light_bulb) { return false; }
        if (cell >= -1 && cell <= 4) { break; }
        if (leave_markup) { cell = CellType::mark_lit; }
    }
    for (int iy = y + 1; iy != w; ++iy) {
        auto &cell = map.at(x).at(iy);
        if (cell == CellType::light_bulb) { return false; }
        if (cell >= -1 && cell <= 4) { break; }
        if (leave_markup) { cell = CellType::mark_lit; }
    }
    for (int ix = x - 1; ix != -1; --ix) {
        auto &cell = map.at(ix).at(y);
        if (cell == CellType::light_bulb) { return false; }
        if (cell >= -1 && cell <= 4) { break; }
        if (leave_markup) { cell = CellType::mark_lit; }
    }
    for (int ix = x + 1; ix != h; ++ix) {
        auto &cell = map.at(ix).at(y);
        if (cell == CellType::light_bulb) { return false; }
        if (cell >= -1 && cell <= 4) { break; }
        if (leave_markup) { cell = CellType::mark_lit; }
    }
    // Check numbered black cell constraint
    for (const auto &ixy : _getCrossAround(x, y)) {
        int ix, iy; std::tie(ix, iy) = ixy;
        // Continue if out of map
        if (ix < 0 || ix >= h || iy < 0 || iy >= w) { continue; }
        int cell = map.at(ix).at(iy);
        // Continue if not a numbered black cell
        if (cell < 0 || cell > 4) { continue; }
        if (cell == 0) { return false; }
        int remain_slot_cnt = cell;
        if (ix - 1 >= 0 && map.at(ix - 1).at(iy) == CellType::light_bulb) { remain_slot_cnt--; }
        if (ix + 1 <  h && map.at(ix + 1).at(iy) == CellType::light_bulb) { remain_slot_cnt--; }
        if (iy - 1 >= 0 && map.at(ix).at(iy - 1) == CellType::light_bulb) { remain_slot_cnt--; }
        if (iy + 1 <  w && map.at(ix).at(iy + 1) == CellType::light_bulb) { remain_slot_cnt--; }
        if (remain_slot_cnt < 1) {
            return false;
        }
    }
    return true;
}

static
GameMap * _placeLightBulb(const GameMap &map, const int x, const int y,
    bool place_markup=false)
{
    int h, w; std::tie(h, w) = getMapShape(map);
    if (x < 0 || x >= h || y < 0 || y >= w) { return nullptr; }
    GameMap *retmap = new GameMap(map);     // NOTE: Deep copy.
    // Reject if is a illegal move
    if (!_checkPlaceable(*retmap, x, y, place_markup)) { delete retmap; return nullptr; }
    retmap->at(x).at(y) = CellType::light_bulb;
    return retmap;
}

static
GameMap * _placeMultipleLightBulb(const GameMap &map, forward_list<Coord> coords)
{
    GameMap *oldmap = new GameMap(map);
    GameMap *newmap = nullptr;
    int h, w; std::tie(h, w) = getMapShape(map);
    for (const auto &coord : coords) {
        int x, y; std::tie(x, y) = coord;
        if (x < 0 || x >= h || y < 0 || y >= w) { delete oldmap; return nullptr; }
        newmap = _placeLightBulb(*oldmap, x, y, true);
        delete oldmap;
        if (newmap == nullptr) { return nullptr; }
        oldmap = newmap;
    }
    return newmap;
}

static
forward_list<GameMap *> _solveNumberedCell(const GameMap &map,
    const forward_list<Coord> &numbered_cells)
{
    int h, w; std::tie(h, w) = getMapShape(map);
    // Return case
    if (numbered_cells.empty()) { return forward_list<GameMap *>{new GameMap(map)}; }
    const auto current_cell = numbered_cells.front();
    int x, y; std::tie(x, y) = current_cell;
    forward_list<Coord> next_numbered_cells(numbered_cells);
    next_numbered_cells.pop_front();
    // Generate branches, place light bulbs
    const forward_list<forward_list<Coord> > *coords_list = nullptr;
    const forward_list<forward_list<Coord> > _case_1_coords_list{
        {std::make_tuple(x - 1, y)},
        {std::make_tuple(x + 1, y)},
        {std::make_tuple(x, y - 1)},
        {std::make_tuple(x, y + 1)}
    };
    const forward_list<forward_list<Coord> > _case_2_coords_list{
        // Corners x 4
        {std::make_tuple(x - 1, y), std::make_tuple(x, y - 1)},
        {std::make_tuple(x - 1, y), std::make_tuple(x, y + 1)},
        {std::make_tuple(x + 1, y), std::make_tuple(x, y - 1)},
        {std::make_tuple(x + 1, y), std::make_tuple(x, y + 1)},
        // Across x 2
        {std::make_tuple(x - 1, y), std::make_tuple(x + 1, y)},
        {std::make_tuple(x, y - 1), std::make_tuple(x, y + 1)},
    };
    const forward_list<forward_list<Coord> > _case_3_coords_list{
        {std::make_tuple(x - 1, y), std::make_tuple(x, y - 1), std::make_tuple(x, y + 1)},
        {std::make_tuple(x, y + 1), std::make_tuple(x - 1, y), std::make_tuple(x + 1, y)},
        {std::make_tuple(x + 1, y), std::make_tuple(x, y - 1), std::make_tuple(x, y + 1)},
        {std::make_tuple(x, y - 1), std::make_tuple(x - 1, y), std::make_tuple(x + 1, y)},
    };
    const forward_list<forward_list<Coord> > _case_4_coords_list{{
        std::make_tuple(x - 1, y), std::make_tuple(x + 1, y),
        std::make_tuple(x, y - 1), std::make_tuple(x, y + 1)
    }};
    switch (map.at(x).at(y)) {
        case 1: { coords_list = &_case_1_coords_list; break; }
        case 2: { coords_list = &_case_2_coords_list; break; }
        case 3: { coords_list = &_case_3_coords_list; break; }
        case 4: { coords_list = &_case_4_coords_list; break; }
        default: throw;
    }
    forward_list<GameMap *> sub_branches;
    GameMap *retmap = nullptr;
    for (auto &coords : *coords_list) {
        if ((retmap = _placeMultipleLightBulb(map, coords)) != nullptr) {
            sub_branches.push_front(retmap);
        }
    }
    // Recurse into branches
    forward_list<GameMap *> ret_branches;
    for (auto &branch : sub_branches) {
        ret_branches.splice_after(ret_branches.cbefore_begin(),
                _solveNumberedCell(*branch, next_numbered_cells));
        delete branch;
    }
    return ret_branches;
}

static
bool _isSolution_WithMarkup(const GameMap &map)
{
    for (auto &row : map) {
        for (auto &cell : row) {
            if (cell == CellType::white || cell == CellType::mark_noplaceable) {
                return false;
            }
        }
    }
    return true;
}

static
forward_list<GameMap *> _solveWhiteCell(const GameMap &map, const forward_list<Coord> &white_cells)
{
    // NOTE: Using brute force enumeration algorithm.
    // TODO: Come back with a better algorithm.
    // Success case
    if (_isSolution_WithMarkup(map)) { return forward_list<GameMap *>{new GameMap(map)}; }
    // Fail case
    if (white_cells.empty()) { return forward_list<GameMap *>(); }
    // Recrusion
    const auto current_cell = white_cells.front();
    int x, y; std::tie(x, y) = current_cell;
    forward_list<Coord> next_white_cells(white_cells);
    next_white_cells.pop_front();
    // Branch - not place
    auto retlist = _solveWhiteCell(map, next_white_cells);
    if (!retlist.empty()) { return retlist; }
    // Branch - place
    GameMap *placed_map = _placeLightBulb(map, x, y, true);
    if (placed_map != nullptr) {
        retlist.splice_after(retlist.cbefore_begin(),
                _solveWhiteCell(*placed_map, next_white_cells));
        delete placed_map;
    }
    return retlist;
}

static
GameMap _solveAkari(const GameMap &g)
{
#if defined(DEBUG_AKARI_CPP)
    std::cout << "Input map..." << std::endl;
    printMap(g);
#endif
    GameMap *map = new GameMap(g);
    int h, w; std::tie(h, w) = getMapShape(*map);
    // Prune
    _prune_black_zero(*map);
#if defined(DEBUG_AKARI_CPP)
    std::cout << "After pruning..." << std::endl;
    printMap(*map);
#endif
    // Get numbered cells
    forward_list<Coord> numbered_cells;
    for (int x = 0; x != h; ++x) {
        for (int y = 0; y != w; ++y) {
            auto cell = map->at(x).at(y);
            if (cell >= 1 && cell <= 4) {
                numbered_cells.push_front(std::make_tuple(x, y));
            }
        }
    }
    // Recursions for numbered black cells
    auto retmaps = _solveNumberedCell(*map, numbered_cells);
    delete map;
#if defined(DEBUG_AKARI_CPP)
    std::cout << "After placing for all numbered black cells, have following "
            << "branches..." << std::endl;
    for (const auto &retmap : retmaps) {
       printMap(*retmap);
    }
#endif
    GameMap *solution = nullptr;
    // Recurse into each branch
    for (const auto &retmap : retmaps) {
        if (solution != nullptr) { break; }
        // Get white cells
        forward_list<Coord> white_cells;
        for (int x = 0; x != h; ++x) {
            for (int y = 0; y != w; ++y) {
                if (retmap->at(x).at(y) == CellType::white) {
                    white_cells.push_front(std::make_tuple(x, y));
                }
            }
        }
        // Recursion for white cells
        auto solutions = _solveWhiteCell(*retmap, white_cells);
        if (!solutions.empty()) { solution = new GameMap(*solutions.front()); }
        for (const auto &sol : solutions) { delete sol; }
    }
    for (const auto &retmap : retmaps) { delete retmap; }
    if (solution == nullptr) {
        std::cerr << "_solveAkari(): No valid solution!" << std::endl;
        throw "_solveAkari(): No valid solution!";
    }
#if defined(DEBUG_AKARI_CPP)
    std::cout << "Final solution..." << std::endl;
    printMap(*solution);
#endif
    return GameMap(*solution);
}

GameMap solveAkari(GameMap & g)
{
    // 请在此函数内返回最后求得的结果
    auto solution = _solveAkari(g);

    // Remove all markups
    for (auto &row : solution) {
        for (auto &cell : row) {
            if (cell < CellType::white) { cell = CellType::white; }
        }
    }

#if defined(DEBUG_AKARI_CPP)
    std::cout << "Return solution..." << std::endl;
    printMap(solution);
#endif
    return solution;
}

}