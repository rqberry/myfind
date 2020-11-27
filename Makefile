CXX       = g++-8
CXX_STD   = -std=c++2a
CXX_W     = -Wall -Wextra -Wpedantic
CXX_GDB   = -g -ggdb3 -O0
CXX_FS	  = -lstdc++fs
CXX_SAN   = -fsanitize=address,leak,undefined
CXX_NODB  = $(CXX_STD) $(CXX_W)
CXX_DB    = $(CXX_NODB) -DDEBUG
CXX_SAN   = $(CXX_DB)  $(CXX_DB)

.cc.o:
	$(CXX) $(CXX_NODB) -c -O0 $<

all: myfind

myfind: myfind.o
	$(CXX) $(CXX_STD) $(CXX_GDB) -o $@ $< $(CXX_FS) $(CXX_W)

valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all #./$(SRC)

helgrind: all
	valgrind --tool=helgrind #./$(SRC)

%.o: %.cc %.hh
	$(CXX) $(CXX_STD) $(CXX_NODB) -c -o $@ $<

clean:
	rm -fv *.o myfind
