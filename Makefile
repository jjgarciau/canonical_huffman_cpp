CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wshadow -Wconversion
BIN := canonical_huffman

all: $(BIN)

$(BIN): src/main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(BIN)
