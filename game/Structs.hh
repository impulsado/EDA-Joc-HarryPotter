#ifndef Structs_hh
#define Structs_hh


#include "Utils.hh"


/**
 * Contains the Dir enumeration,
 * the CommandType enumeration,
 * the Pos struct,
 * the CellType enumeration, the Cell struct,
 * the UnitType enumeration, the Unit struct,
 * and some useful little functions.
 */


/**
 * Enum to encode directions. Remember that wizards cannot move diagonally
 */
enum Dir {
  Down, DR, Right, RU, Up, UL, Left, LD
};

inline ostream& operator << (ostream& out, Dir d) {
  switch (d) {
  case Down:   out << "Down";  break;
  case DR:     out << "DR";    break;
  case Right:  out << "Right"; break;
  case RU:     out << "RU";    break;
  case Up:     out << "Up";    break;
  case UL:     out << "UL";    break;
  case Left:   out << "Left";  break;
  case LD:     out << "LD";    break;
  default:     out << int(d);  break;
  }
  return out;
}


/**
 * Simple struct to handle positions.
 */
struct Pos {

  int i, j;

  /**
   * Default constructor (0, 0).
   */
  Pos ();
  /**
   * Constructor with all defining fields.
   */
  Pos (int i, int j);

  /**
   * Print operator.
   */
  friend ostream& operator<< (ostream& os, const Pos& p);

  /**
   * Comparison operator.
   */
  friend bool operator== (const Pos& a, const Pos& b);

  /**
   * Comparison operator.
   */
  friend bool operator!= (const Pos& a, const Pos& b);

  /**
   * Comparison operator, mostly needed for sorting.
   */
  friend bool operator< (const Pos& a, const Pos& b);

  /**
   * Increment operator: moves a position according to a direction.
   */
  Pos& operator+= (Dir d);

  /**
   * Addition operator: Returns a position by adding a direction.
   */
  Pos operator+ (Dir d) const;

  /**
   * Increment operator: moves a position according to another position.
   */
  Pos& operator+= (Pos p);

  /**
   * Addition operator: Returns a position by adding another position.
   */
  Pos operator+ (Pos p) const;
};

/**
 * Defines kinds of cells.
 */
enum CellType {
  Corridor,
  Wall
};


inline ostream& operator << (ostream& out, CellType c) {
  switch (c) {
  case Corridor: out << "Corridor";   break;
  case Wall:     out << "Wall"; break;
  default:       out << int(c);     break;    
  }
  return out;
}


/**
 * Describes a cell on the board, and its contents.
 */
struct Cell {

  CellType         type; // The kind of cell (Corridor or Waste).
  int             owner; // The player that owns it, otherwise -1.
  int                id; // The id of a unit if present, or -1 otherwise.
  bool             book; // Whether it contains book

  /**
   * Default constructor (Corridor, -1, -1, false).
   */
  Cell ();

  /**
   * Constructor with all defining fields.
   */
  Cell (CellType t, int o, int i, bool b);

  bool is_empty( ) const;
};


/**
 * Defines the type of the unit.
 */
enum UnitType {
  Wizard,
  Ghost
};


inline ostream& operator << (ostream& out, UnitType c) {
  switch (c) {
  case Wizard:  out << "Wizard";  break;
  case  Ghost:  out << "Ghost";  break;
  default:      out << int(c);    break;    
  }
  return out;
}


/**
 * Describes an unit on the board and its properties.
 */
struct Unit {

  UnitType type;             // The type of unit.
  int id;                    // The unique id of this unit during the game.
                             // Ids are consecutive numbers between 0 and 4+4*NUM_INI_WIZARDS_PER_CLAN-1
  int player;                // The player that owns this unit
  Pos pos;                   // The position on the board.    
  int rounds_pending;        // For Wizard: number of rounds before conversion is completed (zero no process)
                             //             If N > 0 and we are in round R, it will change clan at the end of round R + (N-1)
                             // For Ghost: number of rounds before it can act. If zero it means it can now.
                             //            If N > 0 and we are in round R, it cannot act at rounds R, R + 1, ..., R + (N-1)
  int next_player;           // For Wizard and in betwiching process, the owner once the process is completed (-1 if not in co process)
                             // For Ghost: last round where it was attacked (-1 if none)
  
  
  /**
   * Default constructor (Wizard, -1, -1, (0, 0), -1, -1).
   */
  Unit ();

  /**
   * Constructor with all defining fields.
   */
  Unit (UnitType t, int i, int pl,  Pos p, int r, int n);

  // Next 3 only for wizards
  
  // Returns whether a wizard in in conversion process
  bool is_in_conversion_process( );

  // Returns, for a wizard in conversion process, the number of rounds before completing the process
  int  rounds_for_converting ( );

  // Returns, for a wizard in conversion process, the player it will convert to
  int  player_to_be_converted_to();
  
  // Next 2 only for ghosts

  // Returns the last round when a ghost was attacked. Returns -1 if none.
  int  last_attack_received ( );

  // Returns, for a ghost, the number of rounds it needs to wait until it can perform an spell
  int  resting_rounds ( );
};


//////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////


/**
 * Enum to encode commands.
 */
enum CommandType {
  Move,
  Spell
};


inline ostream& operator << (ostream& out, CommandType c) {
  switch (c) {
  case Move:       out << "Move";      break;
  case Spell:      out << "Spell";      break;
  default:         out << int(c);      break;
  }
  return out;
}


inline bool dir_ok (Dir dir) {
  return dir >= Down and dir <= LD;
}

inline Pos::Pos (            ) : i(0), j(0) { }
inline Pos::Pos (int i, int j) : i(i), j(j) { }

inline ostream& operator<< (ostream& os, const Pos& p) {
  return os << "(" << p.i << ", " << p.j << ")";
}

inline bool operator== (const Pos& a, const Pos& b) {
  return a.i == b.i and a.j == b.j;
}

inline bool operator!= (const Pos& a, const Pos& b) {
  return not (a == b);
}

inline bool operator< (const Pos& a, const Pos& b) {
  if (a.i != b.i) return a.i < b.i;
  return a.j < b.j;
}

inline Pos& Pos::operator+= (Dir d) {
  switch (d) {
  case Down:   ++i;  break;
  case DR: ++i; ++j; break;
  case Right:  ++j;  break;
  case RU: --i; ++j; break;
  case Up:     --i;  break;
  case UL: --i; --j; break;
  case Left:   --j;  break;
  case LD: ++i; --j; break;
  default: ; // do nothing
  }
  return *this;
}

inline Pos Pos::operator+ (Dir d) const {
  Pos p = *this;
  p += d;
  return p;
}

inline Pos& Pos::operator+= (Pos p) {
    this->i += p.i;
    this->j += p.j;
    return *this;
  }

inline Pos Pos::operator+ (Pos p) const {
    Pos p2 = *this;
    p2 += p;
    return p2;
  }


inline Cell::Cell (                              ) :
  type(Corridor), owner(-1), id(-1), book(false) { }
inline Cell::Cell (CellType t, int o, int i, bool b) :
  type(t),      owner(o),     id(i), book(false) { }

inline bool Cell::is_empty ( ) const {
  return
    type == Corridor and
    id == -1 and
    not book;
}

inline bool command_type_ok (CommandType c_type) {
  return (c_type == Move or c_type == Spell);
}

/**
 * Conversion from command type to char.
 */
inline char CommandType2char (int t) {
  switch (t) {
  case Move:      return 'm';
  case Spell:     return 's';
  default:        return '_';
  }
}


/**
 * Conversion from char to command type.
 */
inline int char2CommandType (char c) {
  switch (c) {
  case 'm': return Move;
  case 's': return Spell;
  }
  return -1; // Can't abort: if data were corrupted, master would fail.
}


/**
 * Conversion from direction to char.
 * u, d, l, r are obvious
 * diagonals are named after the position of letters in the keyboard:
 * qwe
 * asd
 * zxc
 */
inline char Dir2char (int d) {
  switch (d) {
  case Down:   return 'd';
  case DR:     return 'c';
  case Right:  return 'r';
  case RU:     return 'e';
  case Up:     return 'u';
  case UL:     return 'q';
  case Left:   return 'l';
  case LD:     return 'z';
  default:     return '_';
  }
}

/**
 * Conversion from char to direction.
 */
inline int char2Dir (char c) {
  switch (c) {
  case 'd': return Down;
  case 'c': return DR;
  case 'r': return Right;
  case 'e': return RU;
  case 'u': return Up;
  case 'q': return UL;
  case 'l': return Left;
  case 'z': return LD;
  }
  return -1; // Can't abort: if data is corrupted, master will fail.
}


/**
 * Conversion from unit type to char.
 */
inline char UnitType2char (int t) {
  switch (t) {
  case Wizard:  return 'w';
  case Ghost:   return 'g';
  default:     return '_';
  }
}


/**
 * Conversion from char to unit type.
 */
inline int char2UnitType (char c) {
  switch (c) {
  case 'w': return Wizard;
  case 'g': return Ghost;
  }
  return -1; // Can't abort: if data is corrupted, master will fail.
}


inline Unit::Unit ()
  : type(Wizard), id(-1), player(-1), pos(0, 0), rounds_pending(-1), next_player(-1) { }

inline Unit::Unit (UnitType t, int i, int pl, Pos p, int r, int n)
  : type(t), id(i),  player(pl), pos(p), rounds_pending(r), next_player(n) { }

// For wizards
inline bool Unit::is_in_conversion_process ( ) {
  return type == Wizard and rounds_pending > 0;
}

inline int  Unit::rounds_for_converting ( ) {
  if (type != Wizard) return -1;
  else return rounds_pending;
}

inline int  Unit::player_to_be_converted_to ( ) {
  if (type != Wizard or next_player == -1) return -1;
  else return next_player;
}

// For ghosts
inline int  Unit::last_attack_received ( ){
  if (type == Wizard) return -1;
  else return next_player;
}

inline int  Unit::resting_rounds ( ){
  if (type == Wizard) return -1;
  else return rounds_pending;
}


#endif

