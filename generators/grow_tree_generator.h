#pragma once

#include <chrono>
#include <functional>
#include <random>
#include <ctime>
#include <vector>

#include "../data/maze.h"

namespace mazer2018 {
	namespace generators {
		class grow_tree_generator
		{
		public:
			grow_tree_generator(data::maze& m, int width, int height, int seed);
			~grow_tree_generator() {};

			virtual mazer2018::data::cell *get_next_cell() = 0;
			void generate();

			data::maze& mymaze;
			int seed, width, height;
			std::mt19937 rndgen;
			data::set<data::cell> maze_set;

		};

		grow_tree_generator::grow_tree_generator(data::maze& m, int seed, int width, int height) : mymaze(m), seed(seed), width(width), height(height){
			mymaze.height(this->height);
			mymaze.width(this->width);
			rndgen = std::mt19937(seed);
		}
	}
	
}


