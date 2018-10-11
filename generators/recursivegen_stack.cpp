#include "recursivegen_stack.h"

/**
 * @param r the region tuple to assign the components of
 * @param low_x the lower bound on the x coordinates in the region
 * @param high_x the higher bound on the x coordinates in the region
 * @param low_y the lower bound on the y coordinates in the region
 * @param high_y the higher bound on the y coordinates in the region
 **/
void mazer2018::generators::recursivegen_stack::assign_region(
    const mazer2018::generators::region& r, int& low_x, int& high_x, int& low_y,
    int& high_y, mazer2018::data::orientation& orient) {
  low_x = std::get<int(region_part::LOW_X)>(r);
  high_x = std::get<int(region_part::HIGH_X)>(r);
  low_y = std::get<int(region_part::LOW_Y)>(r);
  high_y = std::get<int(region_part::HIGH_Y)>(r);
  orient = std::get<int(region_part::ORIENTATION)>(r);
}

void mazer2018::generators::recursivegen_stack::splitter(
    int low_x, int high_x, int low_y, int high_y, data::orientation orient) {
  // push the entire maze onto the stack as a single region to begin the
  // recursive division
  regions.push(region(low_x, high_x, low_y, high_y, orient));
  do {
    // grab the top of the stack as we will process one region in
    // each iteration
    region tos = regions.top();
    regions.pop();

    // assign the components of the tuple
    assign_region(tos, low_x, high_x, low_y, high_y, orient);

    // check if this region is at minimum granularity and if it is we
    // simply discard it
    int xdiff = high_x - low_x;
    int ydiff = high_y - low_y;
    if (xdiff <= GRANULARITY || ydiff <= GRANULARITY) {
      continue;
    }
    // generate random numbers for the insertion of the wall.
    std::uniform_int_distribution<int> xdist(low_x, high_x);
    std::uniform_int_distribution<int> ydist(low_y, high_y);
    int wall_x = xdist(rndgen);
    int wall_y = ydist(rndgen);
    // handle orientations separately as they represent dealing with
    // x or y coordinates separately.
    if (orient == data::orientation::HORIZONTAL) {
      // draw a wall in all cells except a random one we will skip
      int yskip = ydist(rndgen);
      for (int count = low_y; count <= high_y; ++count) {
        if (count == yskip) {
          continue;
        }
        mymaze.insert_wall(wall_x, count, orient);
      }
    } else  // vertical
    {
      // same as for horizontal
      int xskip = xdist(rndgen);
      for (int count = low_x; count <= high_x; ++count) {
        if (count == xskip) {
          continue;
        }
        mymaze.insert_wall(count, wall_y, orient);
      }
    }
    // swap orientation
    orient = !orient;

    // push all subregions of this maze onto the stack
    regions.push(region(low_x, wall_x, low_y, wall_y, orient));
    regions.push(region(wall_x, high_x, low_y, wall_y, orient));
    regions.push(region(low_x, wall_x, wall_y, high_y, orient));
    regions.push(region(wall_x, high_x, wall_y, high_y, orient));

  } while (regions.size() > 0);
}
