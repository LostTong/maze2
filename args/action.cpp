#include "action.h"
#include "../data/maze.h"
#include "../generators/recursivegen_stack.h"
#include "../generators/recursive_generator.h"
#include "../generators/prim_generator.h"


/**
 * @file action.cpp contains implementations of concrete child classes of
 * the action class which represent a request to do something from the
 * command line.
 **/

/**
 * @param m the maze data structure to initialise and write the generated
 * maze to.
 **/
mazer2018::data::maze& mazer2018::args::generate_action::do_action(
    mazer2018::data::maze& m) {

	// recursive
	if (_gen_type == 0) {
		generators::recursive_generator generator(m, _seed, _width, _height);
		generator.generate();
	}
	else if (_gen_type == 1) {
		generators::prim_generator generator(m, _seed, _width, _height);
		generator.generate();
	}
	return m;
	

  // create the generator
  if (stack) {
    generators::recursivegen_stack generator(m, _width, _height, _seed);
    generator.generate();
  } else {
    generators::recursivegen generator(m, _width, _height, _seed);
    // generate the maze
    generator.generate();
  }
  return m;
}

/************************************************************************/
/* maze path finding                                                                     */
/************************************************************************/
mazer2018::data::maze& mazer2018::args::path_finding_action::do_action(
	mazer2018::data::maze& m) {
	std::cout << "start maze path finding" << std::endl;

	auto start_time = std::chrono::system_clock::now();

	std::vector<std::vector<data::cell>> &cells = m.get_cells();
	for (int y = 0; y < m.height(); y++) {
		for (int x = 0; x < m.width(); x++) {
			cells[y][x].is_visited = false;
		}
	}

	data::cell &start_cell = m.get_cells()[0][0];
	data::cell &finish_cell = m.get_cells()[m.height() - 1][m.width() - 1];

	bool find_res = is_finish_cell(m, &start_cell, &finish_cell);
	if (!find_res) {
		std::cout << "Can not find maze path!" << std::endl;

		data::cell *cell = &(m.get_cells()[m.height() - 1][m.width() - 1]);
		while (true) {
			if (cell->prev == nullptr) {
				break;
			}
			if (cell->prev->x == 0 && cell->prev->y == 0) {
				break;
			}
			std::cout << cell->x << ", " << cell->y << " -> " << cell->prev->x << ", " << cell->prev->y << std::endl;
			cell = cell->prev;
		}
	}

	auto finish_time = std::chrono::system_clock::now();
	std::chrono::duration<double> total_time = finish_time - start_time;
	std::cout << "path finding time:" << total_time.count() << std::endl;

	return m;
}

bool mazer2018::args::path_finding_action::cell_valid(int x, int y, int width, int height) {
	return !(x < 0 || x >= width || y < 0 || y >= height);
}

bool mazer2018::args::path_finding_action::is_finish_cell(data::maze& m, data::cell *start_cell, data::cell *finish_cell) {
	bool find_path = false;
	std::stack<data::cell *> cell_stack;
	cell_stack.push(start_cell);
	start_cell->is_visited = true;
	while (!cell_stack.empty()) {
		data::cell *cur_cell = cell_stack.top();
		cell_stack.pop();
		for (int i = 0; i < cur_cell->adjacents.size(); i++) {
			data::edge &cur_edge = cur_cell->adjacents[i];

			if (!cell_valid(cur_edge.out_x, cur_edge.out_y, m.width(), m.height())) {
				continue;
			}
			if (m.get_cells()[cur_edge.out_y][cur_edge.out_x].is_visited) {
				continue;
			}
			cell_stack.push(&(m.get_cells()[cur_edge.out_y][cur_edge.out_x]));
			m.get_cells()[cur_edge.out_y][cur_edge.out_x].is_visited = true;
			m.get_cells()[cur_edge.out_y][cur_edge.out_x].prev = cur_cell;
		}
	}
	data::cell *cur_cell = finish_cell;
	while (cur_cell->prev != nullptr) {
		for (int i = 0; i < cur_cell->prev->adjacents.size(); i++) {
			data::edge &edge = cur_cell->prev->adjacents[i];
			if (edge.out_x == cur_cell->x && edge.out_y == cur_cell->y) {
				edge.is_solve = true;
			}
			if (cur_cell->prev->x == start_cell->x && cur_cell->prev->y == start_cell->y) {
				find_path = true;
			}
		}
		cur_cell = cur_cell->prev;
	}
	return find_path;
}


/**
 * perform a save action - save a maze either as a binary or svg file
 **/
mazer2018::data::maze& mazer2018::args::save_action::do_action(
    mazer2018::data::maze& m) {
  /**
   * test if the maze has been initialized yet and if it has not
   * throw an exception
   **/
  if (m.initialized()) {
    // save a binary file
    if (_type == save_type::BINARY) {
      if (!m.save_binary(_name)) {
        std::ostringstream oss;
        oss << "There was an error saving the binary file " << _name
            << std::endl;
        throw action_failed(oss.str());
      }
    } else {
      // save an svg file
      if (!m.save_svg(_name)) {
        std::ostringstream oss;
        oss << "There was an error saving the svg file " << _name << std::endl;
        throw action_failed(oss.str());
      }
    }
  } else {
    // the maze is not initialized
    std::ostringstream oss;
    oss << "Error: the maze is not yet initialized. "
        << "I can't save a non-existent maze." << std::endl;
    throw action_failed(oss.str());
  }
  return m;
}

mazer2018::data::maze& mazer2018::args::load_action::do_action(
    mazer2018::data::maze& m) {
  if (!m.load_binary(_name)) {
    std::ostringstream oss;
    oss << "There was an error loading the binary file " << _name << std::endl;
    throw action_failed(oss.str());
  }
  return m;
}
