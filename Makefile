CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lpng

SRC = main.cpp png_utils.cpp
OBJ = $(SRC:.cpp=.o)

TARGET = dressup

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

