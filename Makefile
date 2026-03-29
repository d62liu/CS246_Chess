CXX      = g++-15
CXXFLAGS = -std=c++23 -fmodules-ts -Wall -Wextra -Isrc
STD_MOD  = /opt/homebrew/Cellar/gcc/15.2.0_1/include/c++/15/bits/std.cc
TARGET   = chess

ifneq ($(wildcard /opt/X11/include/X11/Xlib.h),)
    CXXFLAGS += -DHAVE_X11 -I/opt/X11/include
    X11LIBS   = -L/opt/X11/lib -lX11
else ifneq ($(wildcard /usr/include/X11/Xlib.h),)
    CXXFLAGS += -DHAVE_X11
    X11LIBS   = -lX11
else
    X11LIBS   =
endif

std.o:
	$(CXX) $(CXXFLAGS) $(STD_MOD) -c -o std.o

types_ifc.o: std.o src/game/types.h
	$(CXX) $(CXXFLAGS) -x c++ -c src/game/types.h -o types_ifc.o

types.o: types_ifc.o src/game/types.cc
	$(CXX) $(CXXFLAGS) -c src/game/types.cc -o types.o

observer_ifc.o: std.o src/game/display/observer/observer.h
	$(CXX) $(CXXFLAGS) -x c++ -c src/game/display/observer/observer.h -o observer_ifc.o

observer.o: observer_ifc.o src/game/display/observer/observer.cc
	$(CXX) $(CXXFLAGS) -c src/game/display/observer/observer.cc -o observer.o

pieces_ifc.o: types.o src/game/pieces/pieces.h
	$(CXX) $(CXXFLAGS) -x c++ -c src/game/pieces/pieces.h -o pieces_ifc.o

pieces.o: pieces_ifc.o src/game/pieces/pieces.cc
	$(CXX) $(CXXFLAGS) -c src/game/pieces/pieces.cc -o pieces.o

board_ifc.o: observer.o pieces.o types.o src/game/board/board.h
	$(CXX) $(CXXFLAGS) -x c++ -c src/game/board/board.h -o board_ifc.o

board.o: board_ifc.o src/game/board/board.cc
	$(CXX) $(CXXFLAGS) -c src/game/board/board.cc -o board.o

text_display_ifc.o: board.o src/game/display/text_display.h
	$(CXX) $(CXXFLAGS) -x c++ -c src/game/display/text_display.h -o text_display_ifc.o

text_display.o: text_display_ifc.o src/game/display/text_display.cc
	$(CXX) $(CXXFLAGS) -c src/game/display/text_display.cc -o text_display.o

strategy_ifc.o: board.o src/chess_strategy/strategy.h
	$(CXX) $(CXXFLAGS) -x c++ -c src/chess_strategy/strategy.h -o strategy_ifc.o

strategy.o: strategy_ifc.o src/chess_strategy/strategy.cc
	$(CXX) $(CXXFLAGS) -c src/chess_strategy/strategy.cc -o strategy.o

window.o: src/window/window.cc src/window/window.h
	$(CXX) $(CXXFLAGS) -c src/window/window.cc -o window.o

graphic_display_ifc.o: board.o observer.o src/game/display/graphic_display.h
	$(CXX) $(CXXFLAGS) -x c++ -c src/game/display/graphic_display.h -o graphic_display_ifc.o

graphic_display.o: graphic_display_ifc.o window.o src/game/display/graphic_display.cc
	$(CXX) $(CXXFLAGS) -c src/game/display/graphic_display.cc -o graphic_display.o

game_ifc.o: board.o text_display.o src/game/game.h
	$(CXX) $(CXXFLAGS) -x c++ -c src/game/game.h -o game_ifc.o

game.o: game_ifc.o pieces.o src/game/game.cc
	$(CXX) $(CXXFLAGS) -c src/game/game.cc -o game.o

chess_program_ifc.o: game.o src/game/chessProgram.h
	$(CXX) $(CXXFLAGS) -x c++ -c src/game/chessProgram.h -o chess_program_ifc.o

chess_program.o: chess_program_ifc.o strategy.o graphic_display.o src/game/chessProgram.cc
	$(CXX) $(CXXFLAGS) -c src/game/chessProgram.cc -o chess_program.o

main.o: chess_program.o src/main.cc
	$(CXX) $(CXXFLAGS) -c src/main.cc -o main.o

$(TARGET): std.o types_ifc.o types.o observer_ifc.o observer.o \
           pieces_ifc.o pieces.o board_ifc.o board.o \
           text_display_ifc.o text_display.o \
           strategy_ifc.o strategy.o \
           window.o graphic_display_ifc.o graphic_display.o \
           game_ifc.o game.o \
           chess_program_ifc.o chess_program.o \
           main.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) $^ $(X11LIBS)

clean:
	rm -f *.o $(TARGET)
	rm -rf gcm.cache
