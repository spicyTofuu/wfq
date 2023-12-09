# Makefile

CXX = g++
CXXFLAGS = -Wall -std=c++17
INCLUDES = -I/opt/homebrew/Cellar/nlohmann-json/3.11.3/include

SRC = wfq.cc
EXE = wfq
FILENAME = default_filename

all: $(EXE)

$(EXE): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $(EXE)

clean:
	rm -f $(EXE)

run: all
	./$(EXE) $(FILENAME)

.PHONY: all clean run
