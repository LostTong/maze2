#pragma once

#include "../data/maze.h"
#include "grow_tree_generator.h"

namespace mazer2018 {
	namespace generators {
		class recursive_generator : public grow_tree_generator
		{
		public:

			mazer2018::data::cell *get_next_cell() {};
		};

		mazer2018::data::cell *recursive_generator::get_next_cell() {
			//return maze_set.get_end();
			return nullptr;
		}
	}

}

