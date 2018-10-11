#include <stack>
#include <tuple>
#include "recursivegen.h"
#pragma once
namespace mazer2018 {
namespace generators {
/**
 * we will use the region name as an alias for a tuple that
 * holds all the data we need at each stage to implement
 * the recursive division method for creating a maze.
 **/
using region = std::tuple<int, int, int, int, data::orientation>;

enum region_part { LOW_X, HIGH_X, LOW_Y, HIGH_Y, ORIENTATION };
/**
 * implements the recursive division method for generating a
 * maze using a stack
 **/
class recursivegen_stack : public recursivegen {
  /**
   * inherit the constructor from the parent
   **/
  using recursivegen::recursivegen;

  /**
   * stack of the regions of the maze we still need to process
   **/
  std::stack<region> regions;

 public:
  /**
   * in the parent class this method was a recursive method
   * it isn't here but we override this method as this is the
   * implementation of partitioning the maze which is the part
   * of the algorithm that differs from a recursive and iterative
   * solution.
   **/
  virtual void splitter(int, int, int, int, data::orientation) override;

  /**
   * static method that allows me to assign a tuple's elements
   * to separate variables in one line
   **/
  static void assign_region(const region &, int &, int &, int &, int &,
                            data::orientation &);
};
}  // namespace generators
}  // namespace mazer2018
