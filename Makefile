CXX = g++
CXXFLAGS = -Iinclude -std=c++17 -O2 -msse4.1 -mavx2

#-DBUILD_DEBUG 
LDFLAGS = `sdl2-config --cflags --libs`

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = sw_renderer

all: $(TARGET)


run: $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)
