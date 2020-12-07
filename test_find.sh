#!/usr/bin/env bash

echo -e "\n\n\t\e[1m\e[32m Start Integration Tests\e[0m\n"

GREEN="\e[32m"
RED="\e[31m"
NC="\e[39m"
args=test_dir
find=$(realpath myfind)


all_passed=true
fail () {
    echo -e "\e[31mfailed\e[39m"
    all_passed=false
}
pass () {
    echo -e "\e[32mok\e[39m"
}

# Test 1: one small file single threaded
echo -n "Test 1: "

$find $args > test.out 2> test.err
find $args  > crct.out 2> crct.err
echo -n "Testing $tst with arg: $args ... "
diff test.out crct.out > /dev/null && \
    diff test.err crct.err > /dev/null && \
    pass || fail

rm test.err
rm test.out
rm crct.out
rm crct.err


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
