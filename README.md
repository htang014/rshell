# rshell Command Shell
This is a basic command shell for CS100, written in C++.  It accepts and is able to run executables stored in the locations specified by $PATH, typically /usr/bin and /bin, using execvp().

## Installation
In bash, perform the following:
```
$ git clone http://github.com/htang014/rshell.git
$ cd rshell
$ git checkout hw1
$ make

```
## Launching
After proper installation:
```
$ bin/rshell
```

## Features/Usage
Rshell is a bash-like command shell.  A display showing the current working directory, followed by a command prompt ($ ) means the program is awaiting input.  You may execute any command stored in the $PATH variable along with supported flags.

* **ls** - Recently implemented!  Type `bin\ls` to use. See info below.



###To execute a command:

When the program displays "$ ", type in commands and press ENTER.  Any excess whitespace before or after arguments is ignored.



###Using logic connectors:

In order to use a connector, simply type one between two command executions. For instance:
```
$ cmd1 ; cmd2 || cmd3 && cmd4
```

NOTE: Logic operations are interpreted and performed left to right. Empty commands are treated as successful.  Connectors MUST be separated from each command by spaces.

* **&&** - Execute the command to the right of this if and only if the command to the left executed correctly.

* **||** - Execute the command to the right of this if and only if the command to the left DID NOT execute correctly.

* **;** - Execute the command the the right of this after executing the command to the left.

###I/O redirection and piping:

* **<** - Pass file to the right as input to command on the left. (``cat < inputfile``)
* **<<<** - Pass string to the right as input to command on the left. (``cat <<< "This is a string."``)
* **>** - Output all commands to the left to file on the right. Create if outfile does not exist, overwrite if it does.  (``cat > outfile1 > outfile2``)
* **>>** - Output all commands to the left to file on the right. Create if outfile does not exist, append if it does.
* **|** - Pipe output of command on the left to input of command on the right. (``cat infile | grep word``)

NOTE: Redirects and pipes can be combined.  Commands located after output redirection are ignored.

###Adding comments:

```
$ cmd -l # Everything after the '#' is a comment.
```

###Quitting:
```
$ exit
```

## Included Programs
### ls
Displays filenames in a specified directory, or the current directory by default.  Implementation is based off of GNU ls. The following options are available:
* `[file/dir]` - passing in filenames causes ls to ignore all other files and directories.  If filename is a directory, will display its contents.  In order to check hidden files, please use -a as well.
* `-l` - prints files in long-listed format
* `-R` - recursively performs ls through all folders and subfolders
* `-a` - displays hidden files
NOTE: Options can be combined under one '-' flag.  Options without a '-' flag are treated as file/directory names.

Output is color coded:
* Filenames are white by default
* Green filename indicates executable
* Blue filename indicates directory
* Grey text background indicates hidden file

###cp
``cp [source] [destination]``.  Takes the file in source and copies it to destination.


## Possible Errors

### rshell
* There was an error with execvp(): File or directory not found ---- Caused by an incorrect or invalid command.
* There was an error with open(): File or directory not found ---- Caused by trying to access an invalid file.
* Too many input redirects ---- Occurs when attempting to pass in more than one input to a command.
* Invalid redirection at front()/back() ---- Performed redirection at front or back to empty command.

### ls
* One or more flags are unknown ---- Caused by passing in a flag that does not exist.  For instance `ls -cupcake`.
* There was an error with stat(): File or directory not found ---- Occurs when ls fails to find/open a file.
* There was an error with opendir(): Permission denied ---- Occurs when user does not have access to a directory.

## Code Summary

###rshell.cpp

Contains the shell program and all related feature sets.  This file may be broken up in the future to allow for more efficient debugging.

###ls.cpp

Contains the ls program and related options.  Functions similarly to bash ls.

##cp.cpp

Copy files.  Under Construction.

###Timer.h

Contains the clock functions of cp.cpp.

## Known Bugs

###rshell
* Space after '$' prompt is sometimes deletable.  Causes unexpected behavior.
* Exit command does not take flags
* Cursor can move up and down inside shell.  Doing so will mess up commands.
* The bash command "cd" is not implemented and therefore does not work currently.
* Piping the linux command ``tee outfile`` and the piping the output of tee to another command will yield an empty outfile.
* Avoid using the '>' output redirection on an existing file.  The file will be overwritten, and the contents may be unpredictable.
* Bash commands run in this shell will not be contained within the shell and can potentially affect the bash process.  For instance, "clear" will clear the entire screen, including text originating in bash.

###ls
* The getpwuid() and getgrgid() syscalls used in the -l flag return NULL occasionally and unpredictably.  Owner name and group name will occasionally display as "ERROR" in this case.
* Errno errors occur in an unpredictable fashion when using -l.
* A rare error occurs where stat() fails to open files in a directory.  Slightly more noticable when executing on very large directories.  Appears to occur in predictable locations.
* Running ls with -R while passing in filenames displays current directory even if no files inside are mentioned.
* Color codes for hidden files (grey background) skews column formatting slightly.
