//////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////  

#ifndef Info_hh
#define Info_hh


#include "Settings.hh"
#include "State.hh"


/**
 * Contains a class to store most of the information of the game.
 */


/**
 * Stores all the information of the game,
 * except the vector of names and the random generator of the board.
 */
class Info : public Settings, public State {

  friend class Game;
  friend class SecGame;

public:

  /**
   * Returns the cell defined by the char c.
   */
  inline static Cell char2Cell (char c) {
    //                       owner  id    food
    Cell cell; // (Corridor, -1,    -1,   false) by default
    switch (c) {
    case '.':
      cell.type = Corridor;
      break; // empty cell
    case 'W':
      cell.type = Wall;
      break;
    case '0':
      cell.type = Corridor;
      cell.owner = 0;
      break;
    case '1':
      cell.type = Corridor;
      cell.owner = 1;
      break;
    case '2':
      cell.type = Corridor;
      cell.owner = 2;
      break;
    case '3':
      cell.type = Corridor;
      cell.owner = 3;
      break;
    default:
      _my_assert(false, string(1, c) + " in grid definition.");
    }
    return cell;
  }
  
  /**
   * Reads the grid of the board.
   * Should fill the same data structures as a board generator.
   */
  void read_grid (istream& is) {
    for (auto x : nb_cells) _my_assert(x == 0, "nb_cells should be empty when starting to read_grid");
    
    // Read grid with corridors and walls
    string l;
    is >> l >> l; // Read 1st and 2nd line of column labels.
    
    grid = vector< vector<Cell> >(board_rows(), vector<Cell>(board_cols()));

    for (int i = 0; i < board_rows(); ++i) {
      string s;
      is >> l >> s;      // Read row label in l and row in s.
      _my_assert((int)s.size() == board_cols(),
                 "The read map has a line with incorrect lenght.");
      for (int j = 0; j < board_cols(); ++j) {
	grid[i][j] = char2Cell(s[j]);
	if (grid[i][j].owner != -1) ++nb_cells[grid[i][j].owner];	
      }
    }
    
    // Read units
    is >> l; _my_assert(l == "units", "Expected *units* in grid format");
    int num; is >> num; // Read number of citizens
    is >> l >> l >> l >> l >> l >> l >> l; // Read type id player row colum rounds next
    for (int i = 0; i < num; ++i) {
      char type;
      int id, pl, row, col, rounds, next;
      is >> type >> id >> pl >> row >> col >> rounds >> next;
      _my_assert(pos_ok(row,col), "Citizen placed out of board");
      _my_assert(grid[row][col].is_empty(), "Citizen placed in non-empty cell");
      units.push_back(Unit(UnitType(char2UnitType(type)),id,pl,Pos(row,col),rounds,next));
      _my_assert(id < int(units.size()) and units[id].id == id, "Problem reading units"); // I assume that the units are always fixed, from 0 no N-1 and are written/read in order
      grid[row][col].id = id;

      if      (type == 'w') player2wizards[pl].insert(id);
      else if (type == 'g') ghosts[pl] = id;
      else _my_assert(false, "Wrong type of unit in grid format");
    }
    
    // Read books
    is >> l; _my_assert(l == "books", "Expected 'books' in grid format");
    is >> num; _my_assert(num == num_ini_books(), "Number food in grid should be equal to num_ini_books()");
    is >> l >> l; // Read "row column"
    for (int i = 0; i < num; ++i) {
      int row, col;
      is >> row >> col;
      _my_assert(pos_ok(row,col), "Food placed out of board");
      _my_assert(grid[row][col].is_empty(),
		 "Book placed in non-empty cell");
      grid[row][col].book = true;
    }

    // Read spells
    is >> l; _my_assert(l == "spell_ingredients", "Expected 'spell_ingredients' in grid format");
    is >> num; _my_assert(num > 0, "Number spell ingredients should be > 0");
    spell_vector.clear();
    for (int i = 0; i < num; ++i) {
      int x; is >> x;
      spell_vector.push_back(x);
    }

    // Read Voldemort position
    is >> l;
    _my_assert(l == "Voldemort", "Expected 'Voldemort' in grid format");
    int row, col;
    is >> row >> col;
    _my_assert(pos_ok(row,col), "Voldemort placed out of board");
    p_voldemort = Pos(row,col);
    _my_assert(not voldemort_attacks(), "Voldemort attacks in grid");
  }
  
  /**
   * Checks invariants are preserved.
   */
  bool ok() const;
};


#endif
