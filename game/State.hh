#ifndef State_hh
#define State_hh


#include "Structs.hh"
#include "Settings.hh"

/**
 * Contains a class to store the current state of a game.
 */


/**
 * Stores the game state.
 */
class State {

public:

  /**
   * Returns the current round.
   */
  int round () const;

  /**
   * Returns a copy of the cell at (i, j).
   */
  Cell cell (int i, int j) const;

  /**
   * Returns a copy of the cell at p.
   */
  Cell cell (Pos p) const;

  /**
   * Returns the a copy of the unit with identifier id.
   */
  Unit unit (int id) const;

  /**
   * Returns the ids of the wizards of a player
   */
  vector<int> wizards (int pl) const;

  /**
   * Returns the id of the ghost of a player
   */
  int ghost (int pl) const;

  /**
   * Returns the position of Voldemort
   */
  Pos pos_voldemort ( ) const;
  
  /**
   * Returns the current magic_strength of a player ( strength_points/alive_units )
   */
  int magic_strength (int pl) const;
  
  /**
   * Returns the current score of a player.
   */
  int score (int pl) const;

  /*
   * Returns the ingredients of the current spell
   *
   */
  vector<int> spell_ingredients () const;
  
  /**
   * Returns the percentage of cpu time used so far, in the
   * range [0.0 - 1.0] or a value lesser than 0 if the player is dead.
   */
   // NOTE: only returns a sensible value in server executions.
   // In local executions the returned value is meaningless.
  double status (int pl) const;

  //////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////  

private:

  friend class Info;
  friend class Board;
  friend class Game;
  friend class SecGame;
  friend class Player;

  vector< vector<Cell> >   grid;
  
  vector<int>              scr; // score of each player
  vector<int>              scr_accumulated; // score referring to conversions completed
  vector<int>              nb_cells; // current number of cells conquered by player
  vector<int>              overall_magic_strength; // magic strength of player (to be distributed among units)

  vector<double>           stats; // -1 -> dead, 0..1 -> % of cpu time limit
  int                      rnd;
  
  vector<Unit>             units; // from 0 to 4+4*NUM_INI_WIZARDS_PER_CLAN-1
  vector< set<int> >       player2wizards;
  vector<int>              ghosts;

  Pos                      p_voldemort;
  
  vector<int>              spell_vector;

  
  /**
   * Returns whether id is a valid unit identifier.
   */
  inline bool unit_ok (int id) const {
    return 0 <= id and id < int(units.size());
  }

  // Checks that there is no unit next to Voldemort
  inline bool voldemort_attacks ( ) const {
    for (int d = 0; d < 8; ++d){
      Pos p = p_voldemort + Dir(d);
      if (grid[p.i][p.j].id != -1) return true;
    }
    return false;
  }

  // It has to be inside cell but not in the border
  inline bool pos_ok_for_voldemort (const Pos& p) const {
    return p.i >= 1 and p.j >= 1 and p.i <= (int)grid.size() - 2 and p.j <= (int)grid.size() - 2;
  }

};

inline int State::round () const {
  return rnd;
}

inline Cell State::cell (int i, int j) const {
  if (i >= 0 and i < (int)grid.size() and j >= 0 and j < (int)grid[i].size())
    return grid[i][j];
  else {
    //cerr << "warning: cell requested for position " << Pos(i, j) << endl;
    return Cell();
  }
}

inline Cell State::cell (Pos p) const {
  return cell(p.i, p.j);
}

inline Unit State::unit (int id) const {
  if (not unit_ok(id)) {
    //cerr << "warning: unit requested for identifier " << id << endl;
    return Unit();
  }
  else return units[id];
}

inline vector<int> State::wizards (int pl) const {
  if (pl >= 0 and pl < (int) player2wizards.size())
    return vector<int>(player2wizards[pl].begin(), player2wizards[pl].end());
  else {
    //cerr << "warning: wizards requested for player " << pl << endl;
    return vector<int>();
  }
}

inline int State::ghost (int pl) const {
  if (pl >= 0 and pl < int(ghosts.size()))
    return ghosts[pl];
  else {
    //cerr << "warning: ghost requested for player " << pl << endl;
    return -1;
  }
}

inline Pos State::pos_voldemort ( ) const {
  return p_voldemort;
}

inline int State::magic_strength (int pl) const {
  if (pl >= 0 and pl < (int) scr.size()) {
    return overall_magic_strength[pl]/(1 + player2wizards[pl].size()); // 1 ghost and some wizards
  }
  else {
    //cerr << "warning: magic_strength requested for player " << pl << endl;
    return -1;
  }
}

inline int State::score (int pl) const {
  if (pl >= 0 and pl < (int) scr.size())
    return scr[pl];
  else {
    //cerr << "warning: score requested for player " << pl << endl;
    return -1;
  }
}


inline double State::status (int pl) const {
  if (pl >= 0 and pl < (int)stats.size())
    return stats[pl];
  else {
    //cerr << "warning: status requested for player " << pl << endl;
    return -2;
  }
}

inline vector<int> State::spell_ingredients () const {
  return spell_vector;
}

#endif
