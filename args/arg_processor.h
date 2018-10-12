#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include "../constants/constants.h"
#include "action.h"
#include "../data/maze.h"

/**
 * @file arg_processor.h defines the command line argument processor and
 * related types and functions.
 **/
namespace mazer2018 {
/**
 * namespace used for command line argument processing
 **/
namespace args {
/**
 * defines what type of action has been requested in the
 * command line arguments parsed by the @ref arg_processor.
 **/
enum class option_type {
  /// an action to generate a maze with recursive
  GENERATE_RECURSIVE,
  /// an action to generate a maze with prime
  GENERATE_PRIME,
  /// maze path finding
  PATH_FINDING,
  /// an action to save a maze as an svg file
  SAVE_VECTOR,
  /// an action to save a maze as a binary file
  SAVE_BINARY,
  /// an action to load a maze from a binary file
  LOAD_BINARY
};

/**
 * what type of generate do we want to do - determined by the
 * number of arguments provided to a generate action
 **/
enum class generate_type {
  /// no arguments provided
  DEFAULT,
  /// only a seed provided
  NODIMS,
  /// dimensions provided but no seed
  NOSEED,
  /// all parameters have been specified
  ALLSPEC,
  /// an invalid generate request has been provided
  INVALID
};

/**
 * a convenience function that takes an @ref option_type enumeration
 * and returns a string representing that @ref option_type.
 **/
std::string option_string(option_type);
class arg_processor {
  /**
   * vector of unique_ptr objects that holds the actions that
   * have been requested from the command line. I chose a
   * vector of unique_ptr here for both performance and
   * convenience - when this vector goes out of scope its
   * data will be deleted.
   **/
  std::vector<std::unique_ptr<mazer2018::args::action> > actions;
  /**
   * vector of arguments passed in from the command line
   **/
  std::vector<std::string> arguments;

  /**
   * used to find the next option value for use with options
   * where we need to handle a variable number of arguments
   **/
  int find_next_option(const std::vector<std::string>, const int);
  /// the minimum size of a dimension
  static const int MINDIM = 4;
  /// the maximum size of a dimension
  static const int MAXDIM = 5000;
  /// the length of a file extension of ".svg"
  static const int EXTLEN = 4;
  /// if we are passed a request for something other than
  /// generation there should be exactly one argument
  static const int ONE_ARGUMENT = 1;

 public:
  /**
   * the number of different command line options available
   **/
  static const int NUM_OPTIONS = 6;
  /**
   * the command line options that are available to be used
   **/
  static const std::string arg_strings[NUM_OPTIONS];
  /**
   * constructor for the arg_processor. simply stores the
   * arguments in a string vector
   **/
  arg_processor(int argc, char* argv[]);

  /**
   * processes the command line arguments and returns a
   * vector of @ref action that can be executed in a polymorphic
   * way.
   **/
  std::vector<std::unique_ptr<mazer2018::args::action> > process(void);
  /// validate that a maze dimension is valid
  static bool valid_dim(int);
  /**
   * processes a generate request from the command line
   **/
  std::unique_ptr<action> process_generate_argument(int&, bool, int gen_type);

  /*
  maze path finding
  */
  std::unique_ptr<action> process_path_finding();
};
}  // namespace args
}  // namespace mazer2018
