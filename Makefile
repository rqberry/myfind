CXX       = g++-8
CXX_STD   = -std=c++2a -lstdc++fs
CXX_W     = -Wall -Wextra -Wpedantic -g
CXX_GDB   = -ggdb3 -O0
CXX_SAN   = -fsanitize=address,leak,undefined
CXX_NODB  = $(CXX_STD) $(CXX_W) $(CXX_GDB)
CXX_DB    = $(CXX_NODB) -DDEBUG
CXX_SAN   = $(CXX_DB)  $(CXX_DB)

.cc.o:
	$(CXX) $(CXX_NODB) -c -O3 $<

all: myfind

myfind: myfind.o
	$(CXX) $(CXX_NODB) -o $@ $<

valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all #./$(SRC) lotr.txt

helgrind: all
	valgrind --tool=helgrind #./$(SRC) lotr.txt

%.o: %.cc %.hh
	$(CXX) $(CXX_STD) $(CXX_NODB) -c -o $@ $<

clean:
	rm -fv *.o myfind
