.NOTPARALLEL:

COMPILER = /usr/local/bin/g++-15.2.0 -std=c++20 -fmodules-ts

FLAGS = -Wall -g

CPPFLAGS =

LDFLAGS = -lX11 -lpng

TARGET = chess

SRC_FILES = window.cc window-impl.cc \
            board.cc subject-impl.cc piece-impl.cc \
            bishop-impl.cc king-impl.cc knight-impl.cc pawn-impl.cc queen-impl.cc rook-impl.cc \
            piecefactory-impl.cc board-impl.cc \
            player.cc computerplayer-impl.cc humanplayer-impl.cc \
            level1-impl.cc level2-impl.cc level3-impl.cc level4-impl.cc \
            playerfactory-impl.cc \
            game.cc game-impl.cc \
            display.cc textdisplay-impl.cc graphicaldisplay-impl.cc \
            controller.cc controller-impl.cc \
            main.cc

OBJ_FILES = $(SRC_FILES:.cc=.o)

SYS_HEADERS = chrono cstdlib iostream map memory random stdexcept string vector
SYS_STAMP = .sys_headers.stamp

all: $(TARGET)

$(TARGET): $(SYS_STAMP) $(OBJ_FILES)
	$(COMPILER) $(OBJ_FILES) -o $(TARGET) $(LDFLAGS)

$(OBJ_FILES): $(SYS_STAMP)

player.o: board.o
game.o: board.o player.o
display.o: window.o board.o
controller.o: game.o board.o
main.o: board.o controller.o display.o game.o player.o

%.o: %.cc
	$(COMPILER) $(FLAGS) $(CPPFLAGS) -c $<

$(SYS_STAMP):
	$(COMPILER) -c -x c++-system-header $(SYS_HEADERS)
	touch $(SYS_STAMP)

sys_headers:
	$(COMPILER) -c -x c++-system-header $(SYS_HEADERS)

clean:
	rm -f *.o $(TARGET) $(SYS_STAMP)
	rm -rf gcm.cache 2>/dev/null || true

.PHONY: clean all sys_headers
