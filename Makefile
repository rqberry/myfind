CXX       = g++-8
CXX_STD   = -std=c++2a
CXX_W     = -Wall -Wextra -Wpedantic -g
CXX_GDB   = -ggdb3 -O0
CXX_SAN   = -fsanitize=address,leak,undefined
CXX_NODB  = $(CXX_STD) $(CXX_W) $(CXX_GDB)
CXX_DB    = $(CXX_NODB) -DDEBUG
CXX_SAN   = $(CXX_DB)  $(CXX_DB)
END 			= -lstdc++fs

.cc.o:
	$(CXX) $(CXX_NODB) -c -O3 $<

all: myfind

myfind: myfind.o
	$(CXX) $(CXX_NODB) -o $@ $< $(END)

valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all #./$(SRC)

helgrind: all
	valgrind --tool=helgrind #./$(SRC)

%.o: %.cc %.hh
	$(CXX) $(CXX_STD) $(CXX_NODB) -c -o $@ $<

clean:
	rm -fv *.o myfind
