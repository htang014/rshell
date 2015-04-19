# rshell Command Shell
This is a basic command shell for CS100, written in C++.  It accepts and is able to execute user-defined commands using execvp().

## Installation
In bash, perform the following:
```
$ git clone http://github.com/htang014/rshell.git
$ cd rshell
$ git checkout hw0
$ make

```
## Launching
After proper installation:
```
$ bin/rshell
```

## Features/Usage
Rshell is a bash-like command shell.  A display showing the current working directory, followed by a command prompt ($ ) means the program is awaiting input.  You may execute any command stored in the $PATH variable along with supported flags.



###To execute a command:

When the program displays "$ ", type in commands and press ENTER.  Any excess whitespace before or after arguments is ignored.



###Using logic connectors:

In order to use a connector, simply type one between two command executions. For instance:
```
$ cmd1 ; cmd2 || cmd3 && cmd4
```

NOTE: Logic operations are interpreted and performed left to right. Empty commands are treated as successful.

* **&&** - Execute the command to the right of this if and only if the command to the left executed correctly.

* **||** - Execute the command to the right of this if and only if the command to the left DID NOT execute correctly.

* **;** - Execute the command the the right of this after executing the command to the left.

###Adding comments:

```
$ cmd -l # Everything after the '#' is a comment.
```

## Possible Errors
* There was a problem with execvp(): File or directory not found ---- Caused by an incorrect or invalid command.

## Code Summary

###rshell.cpp

Contains the shell program and all related feature sets.  This file may be broken up in the future to allow for more efficient debugging.

## Known Bugs
* Space after '$' prompt is sometimes deletable.  Causes unexpected behavior.
* Cursor can move up and down inside shell.  Doing so will mess up commands.
* The bash command "cd" is not implemented and therefore does not work currently.
* Bash commands run in this shell will not be contained within the shell and can potentially affect the bash process.  For instance, "clear" will clear the entire screen, including text originating in bash.  This might be worked around by explicitly defining the command in bin/.
