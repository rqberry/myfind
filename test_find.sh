#!/usr/bin/env bash

make > /dev/null

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
		)
find=$(realpath myfind)


fail () {
    echo -en "\e[31mfailed\e[39m"
    echo " Test: $test Argument: $arg ... "
    exit
}
pass () {
    echo -en "\e[32mpassed\e[39m"
    echo " Test: $test Argument: $arg ... "
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

rm crct.out
rm crct.err
rm test.out
rm test.err

make clean > /dev/null

# Assumes a file $test_dir that contains folders. The name of these folders is
# the name of the test. Each folder contains a file "args" which is a string fed
# to both find commands. Then output is saved and compared.

# Compile and unit tests
#cargo test || exit
#cargo build --release || exit

# find executable and test directory
#test_dir=test_dir
#find=$(realpath myfind) #./target/release/find)
#original_dir=$PWD

#all_passed=true
#fail () {
#    echo -e "\e[31mfailed\e[39m"
#    all_passed=false
#}
#pass () {
#    echo -e "\e[32mok\e[39m"
#}

# run tests
#echo -e "\n\n\t\e[1m\e[32m Start Integration Tests\e[0m\n"
#for tst in $test_dir do
#    cd $tst || exit
#    args=a#$(cat args)
#    $find $args > test.out 2> test.err
#    find $args  > crct.out 2> crct.err
#    echo -n "Testing $tst with arg: $args ... "
#    diff test.out crct.out > /dev/null && \
#        diff test.err crct.err > /dev/null && \
#        pass || fail
#    cd $original_dir
#done

#if [ "$all_passed" = true ]; then
#    color="\e[32m"
#else
#    color="\e[31m"
#fi
#echo -e "\n\t$color\e[1mFinished Integration Tests\e[39m\n"
