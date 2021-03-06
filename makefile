CC = gcc
WARNING_FLAGS = -Wall -Wextra -g -O0
EXE = simpleMake
#SCAN_BUILD_DIR = #<desired directory for CSA analysis results>

all: simpleMake.o fileParser.o linkedList.o buildSpec.o dependencyGraph.o commandExec.o
	$(CC) -o $(EXE) simpleMake.o fileParser.o linkedList.o buildSpec.o dependencyGraph.o commandExec.o

simpleMake.o: simpleMake.c dependencyGraph.h fileParser.h linkedList.h commandExec.h
	$(CC) $(WARNING_FLAGS) -c simpleMake.c

fileParser.o: fileParser.c fileParser.h linkedList.h buildSpec.h 
	$(CC) $(WARNING_FLAGS) -c fileParser.c
 
linkedList.o: linkedList.c linkedList.h 
	$(CC) $(WARNING_FLAGS) -c linkedList.c
 
buildSpec.o: buildSpec.c buildSpec.h fileParser.h linkedList.h dependencyGraph.h
	$(CC) $(WARNING_FLAGS) -c buildSpec.c
 
dependencyGraph.o: dependencyGraph.c dependencyGraph.h linkedList.h commandExec.h 
	$(CC) $(WARNING_FLAGS) -c dependencyGraph.c
 
commandExec.o: commandExec.c commandExec.h dependencyGraph.h linkedList.h
	$(CC) $(WARNING_FLAGS) -c commandExec.c
 
clean:
	rm -f $(EXE) *.o
	rm -rf $(SCAN_BUILD_DIR)

#
# Run the Clang Static Analyzer
#
#scan-build: clean
	#scan-build -o $(SCAN_BUILD_DIR) make
