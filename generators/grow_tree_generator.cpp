#include <iostream>

#include "grow_tree_generator.h"


namespace mazer2018 {
	namespace generators {
		
		grow_tree_generator::grow_tree_generator(data::maze& m, int seed, int width, int height) : mymaze(m), seed(seed), width(width), height(height) {
			mymaze.height(height);
			mymaze.width(width);
			mymaze.init();
			std::vector<std::vector<data::cell>> &cells = mymaze.get_cells();
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					cells[y][x] = data::cell(x, y);
					//cells[y][x].adjacents.clear();
				}
			}
			rndgen = std::mt19937(seed);
		}

		void grow_tree_generator::generate() {
			auto start_time = std::chrono::system_clock::now();
			maze_set.clear();
			std::vector<std::vector<data::cell>> &cells = mymaze.get_cells();
			maze_set.insert(&(cells[0][0]));
			cells[0][0].is_visited = true;
			std::vector<data::cell *> adj_cells;
			while (maze_set.get_size() > 0) {
				data::cell *cell = get_next_cell();//
				adj_cells.clear();
				std::vector<data::edge> edges;
				int x = cell->x;
				int y = cell->y;
				edges.push_back(data::edge(x, y, x, y - 1));
				edges.push_back(data::edge(x, y, x, y + 1));
				edges.push_back(data::edge(x, y, x - 1, y));
				edges.push_back(data::edge(x, y, x + 1, y));
				for (int i = 0; i < edges.size(); i++) {
					data::edge &edge = edges[i];
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
					//std::cout << cnt++ << std::endl;
					continue;
				}
				data::cell *new_cell = adj_cells[rndgen() % adj_cells.size()];
				maze_set.insert(new_cell);
				new_cell->is_visited = true;
				int idx = 0;
				int diff_y = cell->y - new_cell->y;
				int diff_x = cell->x - new_cell->x;
				if (diff_y == 1) {
					idx = 0;
				}
				if (diff_y == -1) {
					idx = 1;
				}
				if (diff_x == 1) {
					idx = 2;
				}
				if (diff_x == -1) {
					idx = 3;
				}
				cell->adjacents[idx] = data::edge(cell->x, cell->y, new_cell->x, new_cell->y);
				new_cell->prev = cell;
				//new_cell->adjacents.push_back(data::edge(new_cell->x, new_cell->y, cell->x, cell->y));
			}
			auto finish_time = std::chrono::system_clock::now();
			std::chrono::duration<double> total_time = finish_time - start_time;
			std::cout << "generate cost time:" << total_time.count() << std::endl;
		}

	}

}