#include "action.h"
#include "../data/maze.h"
#include "../generators/recursivegen_stack.h"

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
	if (_gen_type == mazer2018::data::generate_type::RECURSIVE) {

	}
	else if (_gen_type == mazer2018::data::generate_type::PRIM) {

	}

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
