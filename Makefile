CXX = g++
CXXFLAGS = -std=c++17 -O2 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lpng

SRC_COMMON = png_utils.cpp

all: dressup dressup_hardcore

dressup: main.cpp $(SRC_COMMON)
	$(CXX) $(CXXFLAGS) -o $@ main.cpp $(SRC_COMMON) $(LDFLAGS)

dressup_hardcore: main2.cpp $(SRC_COMMON)
	$(CXX) $(CXXFLAGS) -o $@ main2.cpp $(SRC_COMMON) $(LDFLAGS)

clean:
	rm -f dressup dressup_hardcore *.o

.PHONY: all clean
