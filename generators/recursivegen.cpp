#include "recursivegen.h"

/**
 * fills the maze to be fully connected then decide on a random orientation
 * to start the maze. we then call splitter which does the actual work
 **/
void mazer2018::generators::recursivegen::generate() {
  // make the maze a fully connected maze
  // mymaze.fill();
  mymaze.init();
  // set the width and height of the maze
  int width = mymaze.width();
  int height = mymaze.height();
  // set the orientation for the start of the maze
  std::uniform_int_distribution<int> dir_dist(0, 1);
  int start_dir = dir_dist(rndgen);
  // set the recursive function going that does the work
  splitter(0, width - 1, 0, height - 1, data::orientation(start_dir));
}

/**
 * @param low_x the low end of this submaze's x coordinates
 * @param high_x the high end of this submaze's x coordinates
 * @param low_y the low end of this submaze's y coordinates
 * @param high_y the high end of this submaze's y coordinates
 **/
void mazer2018::generators::recursivegen::splitter(int low_x, int high_x,
                                                   int low_y, int high_y,
                                                   data::orientation orient) {
  // import the data namespace to shortcut the length of some of the code
  using namespace data;

  // check the distance between high and low coordinates and return when
  // this is equal to one
  int xdiff = high_x - low_x;
  int ydiff = high_y - low_y;
  if (xdiff <= GRANULARITY || ydiff <= GRANULARITY) return;
  int wall_x = low_x + rndgen() % xdiff;
  int wall_y = low_y + rndgen() % ydiff;

  // check the orientation - the orientation dictates whether we are
  // processing the maze vertically or horizontally on this iteration
  if (orient == data::orientation::VERTICAL) {
    // decide at which point to insert a hole
    int reinsert = rndgen() % (ydiff + low_y);
    // insert the wall at each point other than where we will insert a
    // hole
    for (int x_count = low_x; x_count < high_x; ++x_count) {
      // if this is where we will insert a hole, do it and then start
      // the next iteration of this loop
      if (x_count == reinsert) {
        mymaze.delete_wall(x_count, wall_y, orientation::VERTICAL);
        continue;
      }
      mymaze.insert_wall(x_count, wall_y, orient);
    }
  } else {
    // orientation is horizontal

    // decide at which point to insert a hole
    int reinsert = rndgen() % (xdiff + low_x);
    // insert a wall at each point other than where we insert the hole
    for (int y_count = low_y; y_count <= high_y; ++y_count) {
      // insert the hole
      if (y_count == reinsert) {
        mymaze.delete_wall(wall_x, y_count, data::orientation::HORIZONTAL);
        continue;
      }
      // remove connections from this cell to its neighbours in a
      // horizontal direction
      mymaze.insert_wall(wall_x, y_count, orient);
    }
  }
  // change the orientation
  orient = !orient;

  // divide the rest of the maze into four from this point and process
  // the submazes
  splitter(low_x, wall_x, wall_y, high_y, orient);
  splitter(wall_x, high_x, wall_y, high_y, orient);
  splitter(low_x, wall_x, low_y, wall_y, orient);
  splitter(wall_x, high_x, low_y, wall_y, orient);
}
