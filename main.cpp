#include <cstdlib>
#include <iostream>
#include "args/action.h"
#include "args/arg_processor.h"
#include "data/maze.h"
#include "generators/recursivegen.h"

/**
 * @file main.cpp contains the main function and any basic helper functions
 * specific to main.
 *
 * Created by Paul Miller as part of the assignment 1 solution for
 * COSC1252/1254 - Programming Using C++, semester 2, 2018.
 **/

/**
 * constant that represents that no arguments where passed into main.
 **/
const int NOARGS = 1;

/**
 * function that displays instructions on using this program.
 **/
void display_usage(void);

int main(int argc, char** argv) {
  // test if no arguments were passed in - call usage if this
  // is the case.
  if (argc == NOARGS) {
    std::cerr << std::endl
              << "Error: insufficient command line arguments" << std::endl
              << std::endl;
    display_usage();
    return EXIT_FAILURE;
  }
  // create the command line argument processor
  mazer2018::args::arg_processor processor(argc, argv);
  // vector of action objects - these are wrapped around by unique_ptr
  // as we need polymorphic behaviour - there are many kinds of actions
  //(generate, load, save) but all of them implement do_action()
  std::vector<std::unique_ptr<mazer2018::args::action> > actions;
  // process the arguments and give us a vector of them
  try {
    actions = processor.process();
    // at all stages we are manipulating a single maze - if this has not
    // been initialized, no processing can be done.
    mazer2018::data::maze m;
    // perform all actions requested by the command line arguments
    for (auto& action : actions) {
      m = action->do_action(m);
    }

  } catch (mazer2018::args::action_failed& af) {
    // oops - an action failed so display the appropriate error
    // message
    std::cerr << af.what() << std::endl;
    display_usage();
    return EXIT_FAILURE;
  }
  // everything has succeed so we return a success value
  return EXIT_SUCCESS;
}

/**
 * simply displays the instructions prior to exiting the program.
 **/
void display_usage(void) {
  // provide help for the generate command
  std::cout << "How to use this program:" << std::endl;
  std::cout << "------------------------" << std::endl << std::endl;
  std::cout << "You may enter several types of options to mazer. "
            << "You may enter --g for a " << std::endl;
  std::cout << "generate command, the command should be something like: "
            << std::endl
            << std::endl;
  std::cout << "\t--g seed width height, where seed, width and height "
            << "should be integers. " << std::endl
            << std::endl;
  std::cout << "This program was tested with values for width and height "
            << "up to 1000, and " << std::endl;
  std::cout << "seed values may be any positive integer. You may leave "
            << "out the seed or you may " << std::endl;
  std::cout << "leave out width and height but if you specify width you "
            << "must specify height." << std::endl
            << std::endl
            << std::endl;
  std::cout << "We have also provided an iterative solution where you "
            << "may specify this with the " << std::endl;
  std::cout << "flag --gs. This flag behaves the "
            << "same as --g except that it runs the iterative" << std::endl;
  std::cout << "implementation." << std::endl << std::endl;

  // provide help for the load command
  std::cout << "You may also specify a load command which is specified "
            << "as follows:" << std::endl
            << std::endl;
  std::cout << "\t--lb somemaze.maze, where \"somemaze.maze\" may be any "
            << "valid maze " << std::endl;
  std::cout << "\t\tbinary file. It does not need to have the \".maze\" "
            << "extension. " << std::endl
            << std::endl;

  // provide help for the save binary command.
  std::cout << "You may also save a maze in binary format. A command to "
            << "do this should be of" << std::endl;
  std::cout << "the following form: " << std::endl << std::endl;
  std::cout << "\t--sb somemazefile, where \"somemazefile\" is the "
            << "name of the binary " << std::endl;
  std::cout << "\t\tfile to save." << std::endl << std::endl;

  // provide help for the save svg command.
  std::cout << "Finally, you can save a maze as a svg file which you "
            << "could display in a web " << std::endl;
  std::cout << "browser. You may specify this as follows:" << std::endl
            << std::endl;
  std::cout << "\t--sv somevector.svg, where the program will validate "
            << std::endl;
  std::cout << "\t\tthe extension as the .svg extension is part of the "
            << "open" << std::endl;
  std::cout << "\t\tstandard created by the www "
            << "corporation. " << std::endl
            << std::endl;
}
