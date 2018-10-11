#pragma once
#include "../data/maze.h"
#include "grow_tree_generator.h"

namespace mazer2018 {
	namespace generators {
		class prim_generator : public grow_tree_generator
		{
		public:

			mazer2018::data::cell *get_next_cell() {};
		};

		mazer2018::data::cell *prim_generator::get_next_cell() {
			//return maze_set.get_by_idx(rndgen() % maze_set.get_size());
			return nullptr;
		}
	}

}

