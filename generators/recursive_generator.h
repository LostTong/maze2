#pragma once

#include "../data/maze.h"
#include "grow_tree_generator.h"

namespace mazer2018 {
	namespace generators {
		class recursive_generator : public grow_tree_generator
		{
		public:
			recursive_generator(data::maze& m, int seed, int width, int height) : grow_tree_generator(m, seed, width, height) {};

			mazer2018::data::cell *get_next_cell();
		};

		mazer2018::data::cell *recursive_generator::get_next_cell() {
			return maze_set.get_end();
		}

	}

}

