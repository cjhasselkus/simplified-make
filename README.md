# Simplified make

A simpler version of the *make* tool used in LINUX systems for compilation. Written in C, this project accepts a makefile within the working directory given that the makefile follows specific formatting requirements (described below). When run on a correctly formatted makefile, the project will be built, and any valid commands in the makefile will be executed.

This version includes as its primary features makefile parsing, dependency checking, cyclical dependency detection, and command execution.

## Execution
Simply download files onto your system and build using the native *make* program. 

Once built, from any directory with a makefile, build a project by running simpleMake.


## Formatting Requirements

In order for this version of *make* to accept the given makefile, the file must adhere to the following formatting constraints:

* Target line must always begin in the first char of the line
* The target is followed by a ":" character and a list of dependence names separated by one or more spaces
* A command line must start with a tab character (not spaces)
* Any line that doesn't begin with a target name or a tab are invalid (barring comments)
* Comments, indicated by a "#" character are supported
* Named variables aren't supported.
* Following the original *make*'s convention, the file must be named "makefile" or "Makefile"

For example, if this project's makefile was written in this simplified format:

```
all: simpleMake.o fileParser.o linkedList.o buildSpec.o dependencyGraph.o commandExec.o
	gcc -o simpleMake simpleMake.o fileParser.o linkedList.o buildSpec.o dependencyGraph.o commandExec.o


simpleMake.o: simpleMake.c dependencyGraph.h fileParser.h linkedList.h commandExec.h
	gcc -Wextra -Wall -c simpleMake.c

fileParser.o: fileParser.c fileParser.h linkedList.h buildSpec.h 
	gcc -Wextra -Wall -c fileParser.c

linkedList.o: linkedList.c linkedList.h 
	gcc -Wextra -Wall -c linkedList.c

buildSpec.o: buildSpec.c buildSpec.h fileParser.h linkedList.h dependencyGraph.h
	gcc -Wextra -Wall -c buildSpec.c

dependencyGraph.o: dependencyGraph.c dependencyGraph.h linkedList.h commandExec.h 
	gcc -Wextra -Wall -c dependencyGraph.c

commandExec.o: commandExec.c commandExec.h dependencyGraph.h linkedList.h
	gcc -Wextra -Wall -c commandExec.c
```

## Testing
Test files to be added soon.

Test files will validate that simpleMake behaves as expected given both valid and invalid formatting, as well as when given various  makefile dependency structures.


## Authors

* **Carolyn Hasselkus** - Work done for CS537 : Introduction to Operation Systems at University of Wisconsin-Madison, taught by Professor Barton Miller.
