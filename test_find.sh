#!/usr/bin/env bash

make > /dev/null
touch test.out
touch test.err
touch crct.out
touch crct.err

echo -e "\n\n\t\e[1m\e[32m Start Integration Tests\e[0m\n"

GREEN="\e[32m"
RED="\e[31m"
NC="\e[39m"
declare -a args=("" "." ".." "../.." "test_dir" "test_dir/a" "test_dir ." "\". ..\""
		 "test_dir -name" "-name george" "-name README.md" "-name . ." "-name \". .\" ."
		 "-type b" "-type c" "-type d" "-type p" "-type f" "-type l" "-type s"  
		 "-type b,f" "-type c,f" "-type d,f" "-type p,f" "-type f,l" "-type l,d" "-type s,l" 
		 "-type b -type c" "-type d -type l" "-type p -type f"  "-type s,l -type d,l"
		 "-L" "-L -mtime 0" "-L -mtime 9999999999999" "-mtime 0 -L"
		 "-exec" "-exec echo hi \;" "-exec echo {} \;" "test_dir -exec cat README.md \; -print"
		 "-exec \; -print -print" "-exec \; -print -print hi" 
		 "../.. -type d -exec ls {} \; -print" 
		 "../.. -name README.md -name test_dir" "-name README.md,test_dir" "-mtime 0 -mtime 2"
		 "-mtime 0,1,2" "-mtime 0 a" "-name README.md a" "-type l a" "-exec \; a" "-print a" 
		 "-exec \"cat cat\" c{}t "
		)
find=$(realpath myfind)

fail () {
    echo -en "\e[31mfailed\e[39m"
    echo " Test: $test Argument: $arg"
    #exit
}
pass () {
    echo -en "\e[32mpassed\e[39m"
    echo " Test: $test Argument: $arg"
}

# Test 1: one small file single threaded

test=1
for arg in "${args[@]}"; do
    $find $arg > test.out 2> test.err
    find $arg  > crct.out 2> crct.err
    diff test.out crct.out > /dev/null && \
        diff test.err crct.err > /dev/null && \
        pass || fail
    test=$((test+1))
done

make clean > /dev/null
rm test.out
rm test.err
rm crct.out
rm crct.err