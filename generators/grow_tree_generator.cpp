#include "grow_tree_generator.h"


namespace mazer2018 {
	namespace generators {
		
		void grow_tree_generator::generate() {
			/*
			auto start = std::chrono::system_clock::now();
			maze_set.clear();
			std::vector<std::vector<data::cell>> &cells = mymaze.get_cells();
			//maze_set.insert(&(cells[0][0]));
			std::vector<data::cell *> adj_cells;
			while (maze_set.get_size() > 0) {
				data::cell *cell = get_next_cell();
				adj_cells.clear();
				for (auto &edge : cell->adjacents) {
					if (!mymaze.valid_edge(edge)) {
						continue;
					}
					int x = edge.out_x;
					int y = edge.out_y;
					data::cell &tmp_cell = cells[y][x];
					if (!tmp_cell.is_visited) {
						adj_cells.push_back(&tmp_cell);
					}
				}
				if (adj_cells.empty()) {
					maze_set.remove(cell);
				}
				data::cell *new_cell = adj_cells[rndgen() % adj_cells.size()];
				//maze_set.insert(new_cell);
				cell->adjacents.push_back(data::edge(cell->x, cell->y, new_cell->x, new_cell->y));
				new_cell->adjacents.push_back(data::edge(new_cell->x, new_cell->y, cell->x, cell->y));
			}
			*/
		}

	}

}