#include <chrono>
#include <functional>
#include <random>
#include <ctime>
#include "../constants/constants.h"
#include "../data/maze.h"

#pragma once
namespace mazer2018 {
/**
 * the namespace under which all generators should live
 **/
namespace generators {
/**
 * class that implements a recursive ...
 **/
class recursivegen {
 protected:
  /**
   * a reference to the maze we will manipulate. We use a
   * reference as we want the changes made in this class to
   * be be held elsewhere
   **/
  data::maze& mymaze;

  /**
   * the random number generator.
   **/
  std::mt19937 rndgen;

  /**
   * the range of numbers over which we will generate random
   * numbers.
   **/
  std::uniform_int_distribution<int> dist;

  /**
   * the smallest distance between cells that we will allow for
   * recursion.
   **/
  const int GRANULARITY = 1;

  /**
   *  recursive function which involves splitting the maze
   **/
  virtual void splitter(int, int, int, int, data::orientation);

 public:
  /**
   * function simply sets the parameters to start maze generation
   * then calls the recursive function that does the work.
   **/
  void generate(void);

  /**
   * constructor that creates a recursive division method
   * @param m the maze reference to store
   * @param width the width of the maze to generate
   * @param height the height of the maze to generate
   * @param seed the seed for the random number generator
   **/
  recursivegen(data::maze& m, int width, int height,
               int seed = constants::ERROR)
      : mymaze(m), dist(0, std::max(width, height)) {
    // resize the maze to width and height specified
    m.height(height);
    m.width(width);
    /*
     * tests if the seed has been passed in. If it has
     * not, we use the current time in milliseconds
     */
    if (seed == constants::ERROR) {
      auto time = std::chrono::system_clock::now();
      std::time_t now = std::chrono::system_clock::to_time_t(time);
      seed = now;
      // print out the seed we are going to use
      std::cout << "The seed we are using is: " << seed << std::endl;
    }
    /* initialize the random number generator with the
     * seed specified
     */
    rndgen = std::mt19937(seed);
  }
};
}  // namespace generators
}  // namespace mazer2018
