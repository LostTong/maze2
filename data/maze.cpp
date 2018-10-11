#include "maze.h"

void mazer2018::data::maze::init(void) { _initialized = true; }

/**
 * @param dir the direction we wish to reverse
 * @return the reverse direction to the one passed in
 **/
const mazer2018::data::direction mazer2018::data::operator!(
    const mazer2018::data::direction dir) {
  // shortens our code quite a bit here
  using namespace mazer2018::data;
  // simply return a direction that is the reverse of the one
  // passed in
  switch (dir) {
    case direction::NORTH:
      return direction::SOUTH;
      break;
    case direction::SOUTH:
      return direction::NORTH;
      break;
    case direction::EAST:
      return direction::WEST;
      break;
    case direction::WEST:
      return direction::EAST;
      break;
    default:
      return direction::INVALID;
      break;
  }
}

/**
 * @param name the file name to save the binary data to
 * return true when we successfully save the maze and false if we
 * have any i/o problems along the way.
 **/
bool mazer2018::data::maze::save_binary(const std::string& name) {
  int num_edges;
  int x_count, y_count, e_count;

  // open the file and check that the open was successful
  std::ofstream out(name, std::ios::binary);
  if (!out) {
    std::cerr << "Failed to open file " << name << std::endl;
    return false;
  }
  // enable exception handling on the stream
  out.exceptions(std::ios::badbit | std::ios::failbit);
  // clear the visited flags so we are starting our edge navigation
  // from scratch
  set_unvisited();
  num_edges = edge_count();
  int edge_counter = 0;
  // reset the visited flags as counting the number of edges changes
  // them
  set_unvisited();
  try {
    // write out the width, height and number of edges
    out.write((char*)&_width, sizeof(int));
    out.write((char*)&_height, sizeof(int));
    out.write((char*)&num_edges, sizeof(int));

    // iterate over the cells vector, writing each edge to the
    // svg file.
    for (y_count = 0; y_count < _height; ++y_count) {
      for (x_count = 0; x_count < _width; ++x_count) {
        cell& c = _cells[y_count][x_count];
        for (e_count = 0; e_count < num_dirs; ++e_count) {
          edge& e = c.adjacents[e_count];
          // if this is a valid edge and has not been visited,
          // we write it out
          if (valid_edge(e) && !e.visited) {
            out.write((char*)&e.in_x, sizeof(int));
            out.write((char*)&e.in_y, sizeof(int));
            out.write((char*)&e.out_x, sizeof(int));
            out.write((char*)&e.out_y, sizeof(int));
            e.visited = true;
            if (e.out_y < 0 || e.out_y > int(_height)) continue;
            if (e.out_x < 0 || e.out_x > int(_width)) continue;
            ++edge_counter;
            // grab the reverse edge and also set it
            // to being visited
            cell& other = _cells[e.out_y][e.out_x];
            edge& e = other.adjacents[int(!direction(e_count))];
            e.visited = true;
          }
        }
      }
    }
  } catch (const std::ios_base::failure& f) {
    // exception occured so output it and return false
    std::cerr << f.what() << std::endl;
    return false;
  }
  // have we written the number of edges we expected to? If we
  // haven't, display an error message and return false
  if (num_edges != edge_counter) {
    std::cerr << "Error: incorrect number of edges written." << std::endl;
    std::cerr << "Expected: " << num_edges << " Actual: " << edge_counter
              << std::endl;
    return false;
  }
  // this is a successful run so return true
  return true;
}

/**
 * @param name the file name to save binary file to
 * @return true if the save was successful and false otherwise
 **/
bool mazer2018::data::maze::load_binary(const std::string& name) {
  int num_edges, cur_edges = 0, y_count;

  // open the file and check that the open is successful
  std::ifstream in(name);
  if (!in) {
    std::cerr << "oh no - there was an error opening binary file"
              << " for reading." << std::endl;
    return false;
  }
  // enable exception handling on the stream
  in.exceptions(std::ios::badbit | std::ios::failbit);
  // set all the edges in the maze to not visited as we are starting
  // from scratch
  set_unvisited();
  try {
    // read in the width and height and count of edges
    in.read((char*)&_width, sizeof(int));
    in.read((char*)&_height, sizeof(int));
    // get the number of edges from the file
    in.read((char*)&num_edges, sizeof(int));
    if (!valid_dim(_width) || !valid_dim(_height)) {
      throw args::action_failed(
          "invalid dimensions specified for "
          "the maze in the binary file");
    }
    // resize the width and height according to the numbers read in
    _cells.resize(_height);
    for (y_count = 0; y_count < _height; ++y_count) {
      _cells[y_count].resize(_width);
    }

    // set the maze to be initialized so it can be written out if
    // requested
    init();
    // read in edges from the file. We could read them in as a set of 4
    // using sizeof(edge) as well. There would be no difference in the
    // runtime as c++ uses buffered i/o by default.
    while (in) {
      edge e;
      in.read((char*)&e.in_x, sizeof(int));
      in.read((char*)&e.in_y, sizeof(int));
      in.read((char*)&e.out_x, sizeof(int));
      in.read((char*)&e.out_y, sizeof(int));
      if (!valid_edge(e)) {
        std::cerr << "Error: an invalid edge was read." << std::endl;
        return false;
      }
      // get the direction of this edge (north, south, east or west)
      // as well as its reverse direction then store this edge as well
      int in_x = e.in_x, in_y = e.in_y, out_x = e.out_x, out_y = e.out_y;
      direction dir = get_dir(in_x, in_y, out_x, out_y);
      if (dir == direction::INVALID) {
        std::cerr << "Error: there was an edge in an invalid "
                  << "direction" << std::endl;
        return false;
      }
      direction rev = !dir;
      e.visited = false;
      _cells[in_y][in_x].adjacents[int(dir)] = e;
      edge other = {out_x, out_y, in_x, in_y};
      other.visited = false;
      _cells[out_y][out_x].adjacents[int(rev)] = other;
      // we increment the count of edges for validation
      ++cur_edges;
    }
  } catch (const std::ios_base::failure& f) {
    // we have reached the end of the file
    if (in.rdstate() & std::ifstream::eofbit) {
      // validate that there were exactly the number of edges specified
      if (cur_edges != num_edges) {
        std::ostringstream oss;
        oss << "Error: the number of edges don't match what was "
            << "specified in the file. " << std::endl;
        throw args::action_failed(oss.str());
      }
      return true;
    }
    // there was an io error so display the exception message and return
    // false
    std::cerr << f.what() << std::endl;
    return false;
  }

  // everything has worked so return true
  return true;
}

/**
 * @param e the edge to validate
 **/
bool mazer2018::data::maze::valid_edge(const edge& e) const {
  // check that each component of the edge is valid
  // within the current maze
  if (e.in_x >= int(_width) || e.in_x < 0) return false;
  if (e.in_y >= int(_height) || e.in_y < 0) return false;
  if (e.out_x >= int(_width) || e.out_x < 0) return false;
  if (e.out_y >= int(_height) || e.out_y < 0) return false;
  if (e.in_x == e.out_x && e.in_y == e.out_y) return false;
  return true;
}

/**
 * @param name the name of the svg file to save
 * @return true when the file is successfully saved and false otherwise
 **/
bool mazer2018::data::maze::save_svg(const std::string& name) {
  // magnification changes with the size of the maze so we roughly
  // get an svg file that displays at the same size
  double magnification = MAXRES / std::max(_width - 1, _height - 1);
  int x, y;
  // open the file and validate it was successful
  std::ofstream out(name);
  if (!out) return false;
  // enable exception handling on the stream
  out.exceptions(std::ios::badbit | std::ios::failbit);
  // reset the visited status of each edge so we only write each
  // bidirectional edge once
  set_unvisited();

  try {
    // write the prologue for the svg file and the background
    save_svg_prologue(out, magnification);
    save_svg_background(out, magnification);
    // iterate over each cell of the maze. If it is a valid edge,
    // save it and set its visited status as well as setting the
    // visited status of the opposite edge
    for (y = 0; y < _height; ++y) {
      for (x = 0; x < _width; ++x) {
        cell& c = _cells[y][x];
        for (int e_count = 0; e_count < num_dirs; ++e_count) {
          edge& e = c.adjacents[e_count];
          if (!valid_edge(e)) continue;
          save_svg_edge(out, e, magnification);
          set_visited(e, e_count);
        }
      }
    }
    // write the closing tag for the svg file
    save_svg_epilogue(out);
  } catch (std::ios_base::failure& f) {
    // there was an io error so display an error message and return
    // false
    std::cerr << f.what() << std::endl;
    return false;
  }
  return out.good();
}

void mazer2018::data::maze::set_unvisited(void) {
  // iterate over each cell
  for (int y_count = 0; y_count < _height; ++y_count) {
    for (int x_count = 0; x_count < _width; ++x_count) {
      // set all of its edges to not been visited
      for (int e_count = 0; e_count < num_dirs; ++e_count) {
        _cells[y_count][x_count].adjacents[e_count].visited = false;
      }
    }
  }
}

int mazer2018::data::maze::edge_count(void) {
  int edge_counter = 0;
  // iterate over the cells vector
  for (int y_count = 0; y_count < _height; ++y_count) {
    for (int x_count = 0; x_count < _width; ++x_count) {
      // for each edge going out from this cell, set it to
      // visited and increment the count of edges
      for (int e_count = 0; e_count < num_dirs; ++e_count) {
        cell& c = _cells[y_count][x_count];
        edge& e = c.adjacents[e_count];
        if (valid_edge(e) && !e.visited) {
          set_visited(e, e_count);
          ++edge_counter;
        }
      }
    }
  }
  return edge_counter;
}

/**
 * @param in_x the x coordinate of the current cell
 * @param in_y the y coordinate of the current cell
 * @param out_x the destination x coordinate
 * @param out_y the destination y coordinate
 * @return the direction from the incoming cell to the outgoing
 * cell.
 **/
mazer2018::data::direction mazer2018::data::get_dir(const int in_x,
                                                    const int in_y,
                                                    const int out_x,
                                                    const int out_y) {
  using namespace mazer2018::data;
  int diff_x = in_x - out_x;
  int diff_y = in_y - out_y;
  // the cells should be just one cell away from each other in either a
  // horizontal or vertical direction
  if (diff_x != 0 && diff_y != 0) {
    std::cerr << "error: invalid coordinates given" << std::endl;
    return direction::INVALID;
  } else {
    // check that the coordinates are one space apart
    if (std::abs(diff_x) != 1 && std::abs(diff_y) != 1) {
      std::cerr << "error: invalid coordinates given" << std::endl;
    }
    // calculate direction based on difference between the coordinates
    // passed in
    if (diff_x == 0) {
      if (diff_y == -1)
        return direction::NORTH;
      else
        return direction::SOUTH;
    } else {
      if (diff_x == -1)
        return direction::WEST;
      else
        return direction::EAST;
    }
  }
  return direction::INVALID;
}

/**
 * @param out the stream to write the svg prologue to
 * @param magnification how big is the svg maze to be displayed
 * @return true when there has been no i/o error and false otherwise
 **/
void mazer2018::data::maze::save_svg_prologue(std::ofstream& out,
                                              double magnification) {
  out << "<svg width=\"";
  out << std::floor((_width - 1) * magnification) << "\" height=\""
      << std::floor((_height - 1) * magnification) << "\" xmlns=\"";
  out << "http://www.w3.org/2000/svg\" >" << std::endl;
}

/**
 * @param out the stream to write the svg data to
 * return true when there has been no i/o error and false otherwise
 **/
void mazer2018::data::maze::save_svg_epilogue(std::ofstream& out) {
  out << "</svg>" << std::endl;
}

/**
 * @param out the output stream we are writing to
 * @param e the edge data to write
 * @param magnification how big do we want to draw each edge
 * @return true when there has been no i/o error and false otherwise
 **/
void mazer2018::data::maze::save_svg_edge(std::ofstream& out,
                                          const mazer2018::data::edge& e,
                                          double magnification) {
  out << "<line stroke=\"white\" x1=\"" << e.in_x * magnification << "\" x2=\""
      << e.out_x * magnification << "\" y1=\"" << e.in_y * magnification
      << "\" y2=\"" << e.out_y * magnification << "\" stroke-width=\""
      << magnification / 2.0 << "\" />" << std::endl;
}

/**
 * @param out the stream to write svg data to
 * @param magnification how big do we want to draw each stroke
 * @return true when there has been no i/o error and false otherwise
 **/
void mazer2018::data::maze::save_svg_background(std::ofstream& out,
                                                double magnification) {
  out << "<rect width=\"" << _width * magnification << "\" height=\""
      << _height * magnification << "\" style=\"fill:black\" />" << std::endl;
}

/**
 * @param e the edge we wish to set as visited
 * @param e_index the index into the cell's edges that represents
 * the direction of the edge
 **/
void mazer2018::data::maze::set_visited(edge& e, int e_index) {
  // if this is a valid edge that we haven't visited, mark it so
  if (valid_edge(e) && !e.visited) {
    // grab the opposing edge
    cell& other_c = _cells[e.out_y][e.out_x];
    edge& other_e = other_c.adjacents[int(!direction(e_index))];
    // set them both to visited
    other_e.visited = e.visited = true;
  }
}

/**
 * @param x the x coordinate where we wish to insert a wall
 * @param y the y coordinate where we wisht to insert a wall
 * @param orient the current orientation (horizontal or vertical)
 **/
void mazer2018::data::maze::insert_wall(int x, int y, orientation orient) {
  // initialize a new edge that we are inserting
  edge e;
  e.in_y = y;
  e.in_x = x;
  e.visited = false;
  // we need to handle horizontal and vertical separately
  if (orient == orientation::VERTICAL) {
    // insert an upper edge
    if (y > 0) {
      e.out_y = y - 1;
      e.out_x = x;
      _cells[y][x].adjacents[int(direction::NORTH)] = e;
      // reverse the edge as it should be two-way
      edge other_e = !e;
      _cells[e.out_y][e.out_x].adjacents[int(direction::SOUTH)] = other_e;
    }
    // insert a lower edge
    if (y < _width - 1) {
      e.out_y = y + 1;
      _cells[y][x].adjacents[int(direction::SOUTH)] = e;
      // grab the reverse edge as it should be two-way
      edge other_e = !e;
      _cells[y + 1][x].adjacents[int(direction::NORTH)] = other_e;
    }
  } else {
    // right edge
    if (x > 0) {
      e.out_x = x - 1;
      e.out_y = y;
      _cells[y][x].adjacents[int(direction::EAST)] = e;
      edge other_e = !e;
      _cells[y][x - 1].adjacents[int(direction::WEST)] = other_e;
    }
    // left edge
    if (x < _width - 1) {
      e.out_x = x + 1;
      e.out_y = y;
      _cells[y][x].adjacents[int(direction::WEST)] = e;
      edge other_e = !e;
      _cells[y][x + 1].adjacents[int(direction::EAST)] = other_e;
    }
  }
}

/**
 * @param dim the dimension to test
 **/
bool mazer2018::data::maze::valid_dim(const int dim) {
  if (dim < constants::MIN_DIM || dim > constants::MAX_DIM) {
    return false;
  }
  return true;
}

/**
 * @param x the x coordinate of the wall to delete
 * @param y the y coordinate of the wall to delete
 * @param orient the orientation of the wall
 **/
void mazer2018::data::maze::delete_wall(int x, int y, orientation orient) {
  // we have to deal with the vertical and horizontal case separately
  if (orient == orientation::VERTICAL) {
    // grab the y coordinate of the upper cell from here
    edge& e = _cells[y][x].adjacents[int(direction::NORTH)];
    int out_y = e.out_y;
    // remove the outgoing upper edge
    e.out_y = constants::ERROR;
    // if the outgoing upper edge was valid, grab the opposite edge
    // and wipe it out as well
    if (out_y != constants::ERROR) {
      edge& other_e = _cells[out_y][x].adjacents[int(direction::SOUTH)];
      other_e.out_y = constants::ERROR;
    }
    // perform the same actions for an edge going down from here
    e = _cells[y][x].adjacents[int(direction::SOUTH)];
    out_y = e.out_y;
    e.out_y = constants::ERROR;
    if (out_y != constants::ERROR) {
      edge& other_e = _cells[out_y][x].adjacents[int(direction::NORTH)];
      other_e.out_y = constants::ERROR;
    }
  } else {
    // horizontal
    // grap the edge to the left of here and wipe it out
    edge& e = _cells[y][x].adjacents[int(direction::WEST)];
    int out_x = e.out_x;
    e.out_x = constants::ERROR;
    // if the edge was previously valid, grab the opposing edge
    // and wipe it out.
    if (out_x != constants::ERROR) {
      edge& other_e = _cells[y][out_x].adjacents[int(direction::EAST)];
      other_e.out_x = constants::ERROR;
    }
    // perform the same actions for an edge to the right of here.
    e = _cells[y][x].adjacents[int(direction::EAST)];
    out_x = e.out_x;
    e.out_x = constants::ERROR;
    if (out_x != constants::ERROR) {
      edge& other_e = _cells[y][out_x].adjacents[int(direction::WEST)];
      other_e.out_x = constants::ERROR;
    }
  }
}

/**
 * @param e the edge we wish to reverse
 * @return a reversed edge where the in coordinates are now the out
 * coordinates and vice versa
 **/
mazer2018::data::edge mazer2018::data::operator!(
    const mazer2018::data::edge& e) {
  edge new_e;
  new_e.out_x = e.in_x;
  new_e.in_x = e.out_x;
  new_e.in_y = e.out_y;
  new_e.out_y = e.in_y;
  return new_e;
}

/**
 * @param orient the orientation we wish to reverse
 * @return the reverse orientation to the one passed in so if we got
 * horizontal we should return vertical and vice-versa
 **/
mazer2018::data::orientation mazer2018::data::operator!(
    const mazer2018::data::orientation orient) {
  int intorient = int(orient);
  intorient++;
  return orientation(intorient % NUM_ORIENTATIONS);
}
