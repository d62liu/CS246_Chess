CXX      = g++-15
CXXFLAGS = -std=c++23 -fmodules-ts -Wall -Wextra
STD_MOD  = /opt/homebrew/Cellar/gcc/15.2.0_1/include/c++/15/bits/std.cc

TARGET = chess

# Build order: std -> types -> pieces -> board -> main

std.o:
	$(CXX) $(CXXFLAGS) $(STD_MOD) -c -o std.o

types_ifc.o: std.o src/game/types.h
	$(CXX) $(CXXFLAGS) -x c++ -c src/game/types.h -o types_ifc.o

types.o: types_ifc.o src/game/types.cc
	$(CXX) $(CXXFLAGS) -c src/game/types.cc -o types.o

pieces_ifc.o: types.o src/game/pieces/pieces.h
	$(CXX) $(CXXFLAGS) -x c++ -c src/game/pieces/pieces.h -o pieces_ifc.o

pieces.o: pieces_ifc.o src/game/pieces/pieces.cc
	$(CXX) $(CXXFLAGS) -c src/game/pieces/pieces.cc -o pieces.o

board_ifc.o: pieces.o src/game/board/board.h
	$(CXX) $(CXXFLAGS) -x c++ -c src/game/board/board.h -o board_ifc.o

board.o: board_ifc.o src/game/board/board.cc
	$(CXX) $(CXXFLAGS) -c src/game/board/board.cc -o board.o

main.o: board.o src/main.cc
	$(CXX) $(CXXFLAGS) -c src/main.cc -o main.o

$(TARGET): std.o types_ifc.o types.o pieces_ifc.o pieces.o board_ifc.o board.o main.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) $^

clean:
	rm -f *.o $(TARGET)
	rm -rf gcm.cache
