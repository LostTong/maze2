#pragma once

#include <chrono>
#include <functional>
#include <random>
#include <ctime>
#include <vector>
#include <algorithm>

#include "../data/maze.h"

namespace mazer2018 {
	namespace generators {
		class grow_tree_generator
		{
		public:
			grow_tree_generator(data::maze& m, int width, int height, int seed);

			virtual mazer2018::data::cell *get_next_cell() = 0;
			void generate();

			data::maze& mymaze;
			int seed, width, height;
			std::mt19937 rndgen;
			data::set<data::cell> maze_set;

		};
	}
	
}


