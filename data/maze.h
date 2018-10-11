#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include "../args/action.h"
#include "../constants/constants.h"

#pragma once
namespace mazer2018 {
/**
 * namespace that keeps track of data for the application. Currently
 * that is only the maze and associated classes / structs but this
 * may grow.
 **/
namespace data {
/**
 * enumeration that defines the current orientation for the
 * algorithm. At each recursive call, we swap orientation
 **/
enum orientation {
  /// the orientation is currently horizontal
  HORIZONTAL,
  /// the orientation is currently vertical
  VERTICAL
};

/**
 * ! (not) operator implemented for an orientation enumeration
 * so that it reverses direction (say HORIZONTAL becomes
 * VERTICAL, etc)
 **/
orientation operator!(const orientation);
/**
 * two orientations - HORIZONTAL and VERTICAL
 **/
const int NUM_ORIENTATIONS = 2;

/**
 * enumeration that represents a direction from a cell in terms
 * of the traditional cardinal directions: north, south, etc.
 **/
enum class direction {
  /**
   * represents a cell above the current cell
   **/
  NORTH,
  /**
   * represents a cell below the current cell
   **/
  SOUTH,
  /**
   * represents a cell to the left of the current cell
   **/
  EAST,
  /**
   * represents a cell to the right of the current cell
   **/
  WEST,
  /**
   * represents an invalid direction
   **/
  INVALID
};

/**
 * operator function that turns its direction into its opposite -
 * north becomes south, etc.
 **/
const direction operator!(const direction);

/**
 * the number of valid directions from a cell
 **/
const int num_dirs = 4;

/**
 * calculates the direction of a specified x,y-x,y coordinates.
 * Note that the direction will ge invalid if they are in a
 * diagonal direction from each other or not adjacent.
 **/
direction get_dir(const int in_x, const int in_y, const int out_x,
                  const int out_y);

/**
 * struct that represents an edge in the maze. We have the in
 * coordinates which are the coordinates of the current cell and
 * out coordinates which are the coordinates of the destination
 * cell. We also have a boolean for whether this edge has been
 * visited so we don't process an edge twice.
 **/
struct edge {
  /// the cell's x coordinate
  int in_x,
      /// the cell's y coordinate
      in_y,
      /// the edge's destination x coordinate
      out_x,
      /// the edge's destination y coordinate
      out_y;
  /// whether this edge has been visited in the current algorithm
  bool visited;

  /**
   * I needed to change the default initialisation of an edge
   * as an empty edge should have all coordinates intialized to
   * -1 in my implementation.
   **/
  edge(void)
      : in_x(constants::ERROR),
        in_y(constants::ERROR),
        out_x(constants::ERROR),
        out_y(constants::ERROR),
        visited(false) {}

  /**
   * need to provide a parameterised constructor if I provide
   * a default or it becomes hidden
   **/
  edge(int x1, int y1, int x2, int y2)
      : in_x(x1), in_y(y1), out_x(x2), out_y(y2) {}
};

/**
 * provide the ! operator which will swap the in and out
 * coordinates
 **/
edge operator!(const edge&);

/**
 * struct that represents a cell in the maze. It has an x,y
 * coordinate, and a vector of edges from this cell to other
 * cells
 **/
struct cell {
  /// the x coordinate of the current cell
  int x
      /// the cell's y coordinate
      ,
      y;
  /// vector of edges that link to adjacent cells
  std::vector<edge> adjacents;

  /**
   * default constructor - initializes the adjacents vector to
   * a vector of size 4 and this cell's coordinates to an
   * empty location
   **/
  cell(void) : x(constants::ERROR), y(constants::ERROR), adjacents(num_dirs) {}

  /**
   * constructor which takes the coordinates of the cells and
   * initializes the adjacents vector to a size of 4.
   **/
  cell(int _x, int _y) : x(_x), y(_y), adjacents(num_dirs) {}
};

struct HashFunc{
  std::size_t operator()(const cell &cell) const{
    return (std::size_t)(&cell);
  }
};

struct EqualFunc{
  bool operator() (const cell &c1, const cell &c2) const{
    return c1.x == c2.x && c1.y == c2.y;
  }
};


/**
 * the maze class itself - represents a maze which has been
 * generated or loaded
 **/
class maze {
  /// the width of the maze
  int _width,
      /// the height of the maze
      _height;
  /// the vector of vector of cells that represents the maze itself
  std::vector<std::vector<cell>> _cells;
  /// has this maze been initialized?
  bool _initialized;

  /// the maximum resolution for outputting as svg - required
  /// by my algorithm. Note that under c++11 onwards if I
  /// initalize a static inside the class it must be a constexpr
  ///, that is, it must be resolvable at compile time.
  constexpr static const double MAXRES = 500;

 public:
  /**
   * default constructor - constructs a maze of 0 width and
   * height and puts the maze in an uninitialized state.
   **/
  maze(void) : _width(0), _height(0), _cells(0), _initialized(false) {}

  /**
   * parameterized constructor - constructs a maze of the
   * specified width and height and puts the maze in an
   * uninitialized state.
   * @param width the width of the maze to create
   * @param height the height of the maze to create
   **/
  maze(int width, int height)
      : _width(width),
        _height(height),
        // initialize the _cells vector to be the correct
        // width and height
        _cells(height, std::vector<cell>(width)),
        _initialized(false) {}

  /**
   * returns whether this maze has been initialized or not.
   * It has not been initialized until we build the maze -
   * we don't want to write out a non-existent maze.
   **/
  bool initialized(void) { return _initialized; }

  /**
   * @return a reference to the cells vector to manipulate
   * that locally. If you have a handle to a maze you should
   * have access to the cells. I thought about making cells
   * public but by implementing this in the class declaration,
   * there is no time overhead to call this function and
   * there is no space overhead for it being an inline function
   * as there is only a return statement anyway.
   **/
  std::vector<std::vector<cell>>& get_cells(void) { return _cells; }

  /**
   * getter for the height of the maze
   * @return the maze height
   **/
  int height() const { return _height; }

  /**
   * setter for the maze height. It sets the height then
   * resizes the _cells vector to match that height and
   * each row vector to match the current width
   * @param h the new height for this maze.
   **/
  void height(const int h) {
    _height = h;
    // change the height of the adjacency matrix
    _cells.resize(h);
    // change the width of the adjacency matrix
    for (auto& element : _cells) element.resize(_width);
  }

  /**
   * @return the width of this maze
   **/
  int width() const { return _width; }

  /**
   * change the width of this maze.
   * changing the width of this maze means resizing each row
   * of the adjacency matrix
   **/
  void width(const int w) {
    _width = w;
    // resize each row
    for (auto& element : _cells) {
      element.resize(w);
    }
  }

  /**
   * sets the initialized variable so that we don't try to
   * write out a maze that has not been generated.
   **/
  void init(void);

  /**
   * saves this maze in binary format.
   **/
  bool save_binary(const std::string&);

  /**
   * loads a maze in binary format from a file and initializes
   * the instance data of this maze with that data.
   **/
  bool load_binary(const std::string&);

  /**
   * saves this maze as an svg file.
   **/
  bool save_svg(const std::string&);

  /**
   * writes the svg prolog to a file.
   **/
  void save_svg_prologue(std::ofstream&, double);

  /**
   * writes the closing tag for an svg file to the file
   * specified.
   **/
  void save_svg_epilogue(std::ofstream&);

  /**
   * saves an edge from this maze to an svg file.
   **/
  void save_svg_edge(std::ofstream&, const edge&, double);

  /**
   * writes the background rectangle for this maze to an
   * svg file
   **/
  void save_svg_background(std::ofstream&, double);

  /**
   * sets every edge in this maze to be unvisited.
   * This is necessary for some algorithms such as the
   * saving of the svg file as we don't want to write
   * edges to the file twice
   **/
  void set_unvisited(void);

  /**
   * gets the number of edges in this maze
   **/
  int edge_count(void);

  /**
   * sets a particular edge to be visited. We pass in the
   * e_index within a cell so we can set the opposing edge
   * as well.
   **/
  void set_visited(edge& e, int e_index);

  /**
   * is this a valid edge? a valid edge is one where all
   * coordinates a valid reference within the _cells
   * vector.
   **/
  bool valid_edge(const edge&) const;

  bool valid_dim(const int);
  /**
   * inserts a series of edges between nodes when we divide
   * a submaze / chamber
   **/
  void insert_wall(int, int, orientation);

  /**
   * deletes the edges between nodes for the space inserted
   **/
  void delete_wall(int, int, orientation);
};

// set
template <typename ValueType>
class Set
{
public:
  Set();
  ~Set();

  void add(const ValueType *value);
  bool contain(const ValueType *value) const;
  void remove(ValueType *value);
  void clear();
  ValueType *pop();
  ValueType *get_last();
  int get_size() const;
  ValueType *get_value(int idx);

  void insert(ValueType *value);
  bool remove2(ValueType *value);
  bool search(ValueType *value);

private:
  void add_recursive(const ValueType &value, TreeNode<ValueType> *cur_node);
  bool contain_recursive(const ValueType &value, TreeNode<ValueType> *cur_node) const;
  void remove_recursive(ValueType *value, TreeNode<ValueType> *cur_node, TreeNode<ValueType> *prev_node);
  void clear_recursive(const TreeNode<ValueType> *cur_node);
  TreeNode<ValueType> *get_last_node() const;

  TreeNode<ValueType> *search(ValueType *value, TreeNode<ValueType> *cur_node);
  TreeNode<ValueType> *left_rotate(TreeNode<ValueType> *cur_node);
  TreeNode<ValueType> *right_rotate(TreeNode<ValueType> *cur_node);
  TreeNode<ValueType> *left_right_rotate(TreeNode<ValueType> *cur_node);
  TreeNode<ValueType> *right_left_rotate(TreeNode<ValueType> *cur_node);
  int get_height(TreeNode<ValueType> *cur_node);
  TreeNode<ValueType> *insert(ValueType *value, TreeNode<ValueType> *cur_node);
  TreeNode<ValueType> *remove2(ValueType *value, TreeNode<ValueType> *cur_node);

  int size;
  TreeNode<ValueType> *root;
  TreeNode<ValueType> *last;
  std::unordered_map<const ValueType *, int> value_map;
  std::stack<ValueType *> node_stack;
};

template <typename ValueType>
Set<ValueType>::Set()
{
  root = nullptr;
  last = nullptr;
  size = 0;
  while (!node_stack.empty()) {
    node_stack.pop();
  }
}

template <typename ValueType>
Set<ValueType>::~Set()
{
  clear();
}

template <typename ValueType>
int Set<ValueType>::get_height(TreeNode<ValueType> *node) {
  return node == nullptr ? -1 : node->height;
}

// search
template <typename ValueType>
bool Set<ValueType>::search(ValueType *value) {
  return serach(value, root) == nullptr ? false : true;
}

template <typename ValueType>
TreeNode<ValueType> *Set<ValueType>::search(ValueType *value, TreeNode<ValueType> *node) {
  if(node == nullptr){
    return nullptr;
  }
  if(value == node->value){
    return node;
  }
  if(*value < *(node->value)){
    return search(value, node->left);
  }
  return search(value, node->right);
}

// left rotate
template <typename ValueType>
TreeNode<ValueType> *Set<ValueType>::left_rotate(TreeNode<ValueType> *cur_node){
  TreeNode<ValueType> *left_node = cur_node->right->left;
  TreeNode<ValueType> *new_root = cur_node->right;
  cur_node->right->left = cur_node;
  cur_node->right = left_node;
  cur_node->height = std::max(get_height(cur_node->left), get_height(cur_node->right)) + 1;
  if(cur_node->right != nullptr){
    cur_node->right->height = std::max(get_height(cur_node->right->left), get_height(cur_node->right->right)) + 1;
  }
  return new_root;
}

// right rotate
template <typename ValueType>
TreeNode<ValueType> *Set<ValueType>::right_rotate(TreeNode<ValueType> *cur_node)
{
  TreeNode<ValueType> *right_node = cur_node->left->right;
  TreeNode<ValueType> *new_root = cur_node->left;
  cur_node->left->right = cur_node;
  cur_node->left = right_node;
  cur_node->height = std::max(get_height(cur_node->left), get_height(cur_node->right))+1;
  if(cur_node->left != nullptr){
    cur_node->left->height = std::max(get_height(cur_node->left->left),get_height(cur_node->left->right))+1;
  }
  return new_root;
}

// left and right rotate
template <typename ValueType>
TreeNode<ValueType> *Set<ValueType>::left_right_rotate(TreeNode<ValueType> *cur_node)
{
  cur_node->left = left_rotate(cur_node->left);
  return right_rotate(cur_node);
}

// right and left rotate
template <typename ValueType>
TreeNode<ValueType> *Set<ValueType>::right_left_rotate(TreeNode<ValueType> *cur_node)
{
  cur_node->right = right_rotate(cur_node->right);
  return left_rotate(cur_node);
}

// insert
template <typename ValueType>
void Set<ValueType>::insert(ValueType *value)
{
  root = insert(value, root);
  size += 1;
  value_map.insert({ value, get_size() - 1 });
}

template <typename ValueType>
TreeNode<ValueType> *Set<ValueType>::insert(ValueType *value, TreeNode<ValueType> *cur_node)
{
  if(cur_node == nullptr){
      cur_node = new TreeNode<ValueType>(value);
    node_stack.push(value);
    //std::cout << "stack size:" << node_stack.size() << std::endl;
    return cur_node;
  }
  else 
  {
    if(value == cur_node->value){
      return cur_node;
    }
    else if(*value < *(cur_node->value)){
      cur_node->left = insert(value,cur_node->left);
    }
    else {
      cur_node->right = insert(value,cur_node->right);
    }
  }
  //cur_node->height = max(get_height(cur_node->left),get_height(cur_node->right))+1;
  if(get_height(cur_node->left) - get_height(cur_node->right) == 2)
  {
    if(*value < *(cur_node->left->value)){
      cur_node = right_rotate(cur_node);
    }
    else {
      cur_node = left_right_rotate(cur_node);
    }
  }
  else if(get_height(cur_node->left) - get_height(cur_node->right) == -2)
  {
    if(*value > *(cur_node->right->value)){
      cur_node = left_rotate(cur_node);
    }
    else {
      cur_node = right_left_rotate(cur_node);
    }
  }
  cur_node->height = std::max(get_height(cur_node->left), get_height(cur_node->right))+1;
  return cur_node;
}

// delete
template <typename ValueType>
bool Set<ValueType>::remove2(ValueType *value)
{
  root = remove2(value, root);
  value_map.erase(value);
  return true;
}

template <typename ValueType>
TreeNode<ValueType> *Set<ValueType>::remove2(ValueType *value, TreeNode<ValueType> *cur_node)
{
  if(cur_node == nullptr)
    return nullptr;
  else 
  {
    if(value == cur_node->value)
    {
      if(cur_node->right == nullptr)
      {
        TreeNode<ValueType> *cur = cur_node;
        cur_node = cur_node->left;
        size -= 1;
        node_stack.pop();
        delete cur;
        return cur_node;
      }
      else 
      {
        TreeNode<ValueType> *cur = cur_node->right;
        while(cur->left != nullptr){
          cur = cur->left;
        }
        cur_node->value = cur->value;
        cur_node->right = remove2(cur->value, cur_node->right);
      }
    }
    else if(*value < *(cur_node->value)){
      cur_node->left = remove2(value, cur_node->left);
    }
    else {
      cur_node->right = remove2(value, cur_node->right);
    }

    if(get_height(cur_node->left)- get_height(cur_node->right) == -2)
    {
      if(get_height(cur_node->right->right) >= get_height(cur_node->right->left)){
        cur_node = left_rotate(cur_node);
      }
      else {
        cur_node = right_left_rotate(cur_node);
      }
    }
    else if(get_height(cur_node->left) - get_height(cur_node->right) == 2)
    {
      if(get_height(cur_node->left->left) >= get_height(cur_node->left->right)){
        cur_node = right_rotate(cur_node);
      }
      else {
        cur_node = left_right_rotate(cur_node);
      }
    }
    cur_node->height = std::max(get_height(cur_node->left), get_height(cur_node->right)) + 1;
  }
  return cur_node;
}




template <typename ValueType>
void Set<ValueType>::add(const ValueType *value) {
  add_recursive(*value, root);
  value_map.insert({value, get_size() - 1});
}

template <typename ValueType>
void Set<ValueType>::add_recursive(const ValueType &value, TreeNode<ValueType> *cur_node) {
  //std::cout << "add_recursive" << std::endl;
  // root
  if (cur_node == nullptr) {
    //std::cout << "add1" << std::endl;
    cur_node = new TreeNode<ValueType>(&value);
    last = cur_node;
    size += 1;
    root = cur_node;
  }
  else {
    // left
    if (value < *(cur_node->value)) {
      if (cur_node->left == nullptr) {
        cur_node->left = new TreeNode<ValueType>(&value);
        cur_node->left->prev = last;
        last = cur_node->left;
        size += 1;
        //std::cout << "prev x,y:" << cur_node->value->get_x_position() << ", " << cur_node->value->get_y_position() << std::endl;
        //std::cout << "left value x,y:" << cur_node->left->value->get_x_position() << ", " << cur_node->left->value->get_y_position() << std::endl;
      }
      else {
        add_recursive(value, cur_node->left);
      }
    }
    // right
    else {
      if (cur_node->right == nullptr) {
        cur_node->right = new TreeNode<ValueType>(&value);
        cur_node->right->prev = last;
        last = cur_node->right;
        size += 1;
        //std::cout << "prev x,y:" << cur_node->value->get_x_position() << ", " << cur_node->value->get_y_position() << std::endl;
        //std::cout << "right value x,y:" << cur_node->right->value->get_x_position() << ", " << cur_node->right->value->get_y_position() << std::endl;
      }
      else {
        add_recursive(value, cur_node->right);
      }
    }
  }
}

template <typename ValueType>
bool Set<ValueType>::contain(const ValueType *value) const {
  return contain_recursive(*value, root);
}

template <typename ValueType>
bool Set<ValueType>::contain_recursive(const ValueType &value, TreeNode<ValueType> *cur_node) const {
  if (cur_node == nullptr) {
    return false;
  }
  if (&value == cur_node->value) {
    return true;
  }
  if (value < *(cur_node->value)) {
    return contain_recursive(value, cur_node->left);
  }
  return contain_recursive(value, cur_node->right);
}

template <typename ValueType>
void Set<ValueType>::remove(ValueType *value) {
  remove_recursive(value, root, nullptr);
}

template <typename ValueType>
void Set<ValueType>::remove_recursive(ValueType *value, TreeNode<ValueType> *cur_node, TreeNode<ValueType> *prev_node) {
  //std::cout << "value x, y:" << value->x_pos << ", " << value->y_pos << " cur_node x,y:" << cur_node->value->x_pos << ", " << cur_node->value->y_pos << std::endl;
  //std::cout << "remove_recursive" << std::endl;
  if (cur_node != nullptr) {
    if (value == cur_node->value) {
      if (value == last->value) {
        last = last->prev;
      }
      size -= 1;

      //std::cout << "value x, y:" << value->x_pos << ", " << value->y_pos << " cur_node x,y:" << cur_node->value->x_pos << ", " << cur_node->value->y_pos << std::endl;

      // left
      if (cur_node->left != nullptr) {
        // root     
        if (prev_node == nullptr) {
          root = cur_node->left;
        }
        // left
        else if (prev_node->left == cur_node) {
          prev_node->left = cur_node->left;
        }
        // right
        else {
          prev_node->right = cur_node->left;
        }
        if (cur_node->right != nullptr) {
          TreeNode<ValueType> *tmp_node = cur_node->left;
          while (tmp_node->right != nullptr) {
            tmp_node = tmp_node->right;
          }
          tmp_node->right = cur_node->right;
        }
      }
      // right
      else if (cur_node->right != nullptr) {
        // root
        if (prev_node == nullptr) {
          root = cur_node->right;
        }
        // left
        else if (prev_node->left == cur_node) {
          prev_node->left = cur_node->right;
        }
        // right
        else {
          prev_node->right = cur_node->right;
        }
      }
      // leaf
      else {
        // root
        if (prev_node == nullptr) {
          root = nullptr;
        }
        // left
        else if (prev_node->left == cur_node) {
          prev_node->left = nullptr;
        }
        // right
        else {
          prev_node->right = nullptr;
        }
      }
      value_map.erase(value);
      delete cur_node;
    }
    else {
      if (*(cur_node->value) > *value) {
        remove_recursive(value, cur_node->left, cur_node);
      }
      else {
        remove_recursive(value, cur_node->right, cur_node);
      }
    }
  }
}

template <typename ValueType>
void Set<ValueType>::clear() {
  clear_recursive(root);
  root = last = nullptr;
}

template <typename ValueType>
void Set<ValueType>::clear_recursive(const TreeNode<ValueType> *cur_node) {
  if (cur_node == nullptr) {
    return;
  }
  if (cur_node->left != nullptr) {
    clear_recursive(cur_node->left);
  }
  if (cur_node->right != nullptr) {
    clear_recursive(cur_node->right);
  }
  delete cur_node;
}

template <typename ValueType>
TreeNode<ValueType> *Set<ValueType>::get_last_node() const {
  return last;
}

template <typename ValueType>
int Set<ValueType>::get_size() const {
  return size;
}

template <typename ValueType>
ValueType *Set<ValueType>::pop() {
  if (last == nullptr) {
    return nullptr;
  }
  TreeNode<ValueType> *tmp = last;
  remove(last->value);
  last = last->prev;
  return tmp->value;
}

template <typename ValueType>
ValueType *Set<ValueType>::get_last() {
  if (node_stack.empty()) {
    return nullptr;
  }
  return node_stack.top();

  if (last == nullptr) {
    return nullptr;
  }
  return last->value;
}


template <typename ValueType>
ValueType *Set<ValueType>::get_value(int idx) {
  int tmp = idx % value_map.size();
  auto it = value_map.begin();
  std::advance(it, tmp);
  return const_cast<ValueType *>(it->first);
}

}  // namespace data
}  // namespace mazer2018
