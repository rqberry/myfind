# myfind

#### Ryan Quisenberry and Ariel Flaster


## myfind

this is our alternative to linux's find. generally, the first argument is a directory or multiple directories or nothing at all. these are the locations myfind must search. There are also other arguments you can pass to myfind, which are detailed below. It prints everything that meets the criteria of the options used, with the exception of -exec.

### -L

This option tells myfind to follow symbolic links and to print what they point to. This command has to come before other commands.

### -name

When the name option is used, you have to follow it up with a name. It'll search all subdirectories for files/directories with matching names.

### -type

If you use the -type option, you can specify a type (or a comma separated list of types) to search for. The following are all the different arguments you can pass to -type.

    b   block (buffered) special
    c   character (unbuffered) special
    d   directory
    p   named pipe (FIFO)
    f   regular file
    l   symbolic link (this is never true if -L is in effect)
    s   socket
    D   door (Solaris)



### -mtime

The mtime option lets you tell the computer exactly what time you want it to look for files from. Specifying a number lets you know how many days from yesterday you want to look into the past to find files. If you pass the argument 0, it will only return files from the day prior.


### -exec

-exec allows you to tell myfind to run a specific program on everything that shows up. Using open and closed curly braces automatically passes the things passed to myfind to the program you pass to exec. Exec relies on the system command to execute the program passed as an argument.

#### -print

This is more of an argument to exec. If -print is used, things will print in the normal find style in addition to the output of whatever command you give to exec.

### running our test and making

make is cool. also Ryan wrote a super cool ./test_find.sh which you need to run twice in order for it to work correctly.

### Known bugs

our test .sh doesn't work on pathnames with spaces in them. so don't use this on windows!!
