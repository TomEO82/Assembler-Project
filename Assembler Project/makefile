assembler: assembler.o parser.o helper.o firstPass.o secondPass.o outputFiles.o
	gcc -Wall -ansi -pedantic assembler.o parser.o helper.o firstPass.o secondPass.o outputFiles.o -o assembler
assembler.o: assembler.c globals.h
	gcc -Wall -ansi -pedantic -c assembler.c -o assembler.o
parser.o: parser.c globals.h
	gcc -Wall -ansi -pedantic -c parser.c -o parser.o
helper.o: helper.c globals.h
	gcc -Wall -ansi -pedantic -c helper.c -o helper.o
firstPass.o: firstPass.c globals.h
	gcc -Wall -ansi -pedantic -c firstPass.c -o firstPass.o
secondPass.o: secondPass.c globals.h
	gcc -Wall -ansi -pedantic -c secondPass.c -o secondPass.o
outputFiles.o: outputFiles.c globals.h
	gcc -Wall -ansi -pedantic -c outputFiles.c -o outputFiles.o
