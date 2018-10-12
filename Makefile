############################################################################
# mazer2018 - an implementation of the assignment 1 specification for
# COSC1252/COSC1254 - Programming Using C++
#
# Developed by Paul Miller. This solution can be used as a starting point for
# Assignment 2.
############################################################################

#object files created as part of compilation
OBJECTS=data/maze.o generators/recursivegen.o generators/grow_tree_generator.o main.o args/action.o args/arg_processor.o \
generators/recursivegen_stack.o
#header files included in various files.
HEADERS=data/maze.h generators/recursivegen.h generators/grow_tree_generator.h args/action.h args/arg_processor.h constants/constants.h \
generators/recursivegen_stack.h

#how do we create the binary for execution
all: $(OBJECTS)
	g++ $(OBJECTS) -o mazer

#how do we make each individual object file? 
%.o: %.cpp $(HEADERS)
	g++ -Wall -pedantic -std=c++14 -g -c $< -o $@

#how do we do cleanup so that we only have the source files?
.PHONY:clean
clean:
	rm -f $(OBJECTS) mazer

.PHONY:svgclean
svgclean : clean
	rm -f *.svg
