.PHONY: main single multi clean

CXX = g++
CXXFLAGS = --std=c++11 -pthread #-DDEBUG_AKARI_CPP

main: multi

main.o: main.cpp

akari.o: akari.cpp makefile
akari: main.o akari.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@
single: akari
	@echo "============= RUN ============="
	@/usr/bin/time -v ./$<

akari-multithreaded.o: akari-multithreaded.cpp makefile
akari-multithreaded: main.o akari-multithreaded.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@
multi: akari-multithreaded
	@echo "============= RUN ============="
	@/usr/bin/time -v ./$<

clean:
	$(RM) ./*.o ./akari ./akari-multithreaded
