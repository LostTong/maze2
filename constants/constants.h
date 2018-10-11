/**
 * @file constants.h contains global constants for the application.
 * Created by Paul Miller as part of the assignment 1 solution for
 * COSC1252/COSC1254 - Programming Using C++, semester 2, 2018.
 **/
#pragma once
/**
 * the base namespace for this project
 **/
namespace mazer2018 {
/**
 * namespace that holds all constants used by the system
 **/
namespace constants {
/**
 * a global int constant for the application representing an
 * error state
 **/
const int ERROR = -1;
/**
 * defines the number of parameters passed in when generating a
 * maze where no seed is provided
 **/
const int NOSEED = 2;

/**
 * the default width for a maze
 **/
const unsigned DEFAULT_WIDTH = 10;

/**
 * default height for a maze
 **/
const unsigned DEFAULT_HEIGHT = 10;

/**
 * minimum value for width or height
 **/
const int MIN_DIM = 2;

/**
 * maximum value for width or height
 **/
const int MAX_DIM = 5000;
}  // namespace constants
}  // namespace mazer2018
