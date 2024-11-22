#ifndef Settings_hh
#define Settings_hh


#include "Structs.hh"


/** \file
 * Contains a class to store all the game settings that do not change
 * during a game, except the names of the players.
 */


/**
 * Stores most of the game settings.
 */
class Settings {

public:
    /**
   * Returns a string with the game name and version.
   */
  static string version ();

  /**
   * Returns the number of players in the game.
   */
  int num_players () const;

  /**
   * Returns the number of rounds a match lasts.
   */
  int num_rounds () const;

  /**
   * Returns the number of rows of the board.
   */
  int board_rows () const;

  /**
   * Returns the number of columns of the board.
   */
  int board_cols () const;

  /**
   * Returns the initial number of units per clan
   */
  int num_ini_wizards_per_clan () const;

  /**
   * Returns the initial number of book items on the board
   */
  int num_ini_books () const;

  /**
   * Returns the initial magic strength of each clan
   */
  int clan_ini_magic_strength () const;

  /**
   * Returns the points obtained after converting a wizard
   */
  int points_for_converting_wizard () const;

  /**
   * Returns the points obtained for each owned cell at the end of a round
   */
  int points_per_owned_cell () const;

  /**
   * Returns the units of magic strength obtained by reading a book
   */
  int book_magic_strength () const;

  /**
   * Returns the number of rounds needed to complete the converting process of a wizard
   */
  int rounds_for_converting_wizard () const;

  /**
   * If a ghost is attacked, she will have to wait an additional rounds_attack_resting_ghost() number
   * of rounds before  * it can perform an spell again
   */
  int rounds_attack_resting_ghost ( ) const;

  /**
   * If a ghost is attacked, she cannot be attacked again for the next rounds_no_attack_ghost()
   * rounds
   */
  int rounds_no_attack_ghost ( ) const;

  /**
   * If a ghost performs an spell, it has to wait for this number of rounds to be able to perform another spell.
     No that if she is attacked, she will this number of rounds increases
   */
  int rounds_spell_resting_ghost ( ) const;

  /**
   * Returns whether pl is a valid player identifier.
   */
  bool player_ok (int pl) const;

  /**
   * Returns whether (i, j) is a position inside the board.
   */
  bool pos_ok (int i, int j) const;

  /**
   * Returns whether p is a position inside the board.
   */
  bool pos_ok (Pos p) const;

private:
  
  friend class Info;
  friend class Board;
  friend class Game;
  friend class SecGame;
  friend class Player;

  int NUM_PLAYERS;
  int NUM_ROUNDS;
  int BOARD_ROWS;
  int BOARD_COLS;
  int NUM_INI_WIZARDS_PER_CLAN;
  int NUM_INI_BOOKS;
  int CLAN_INI_MAGIC_STRENGTH;
  int POINTS_FOR_CONVERTING_WIZARD;
  int POINTS_PER_OWNED_CELL;
  int BOOK_MAGIC_STRENGTH;
  int ROUNDS_FOR_CONVERTING;
  int ROUNDS_ATTACK_RESTING_GHOST;
  int ROUNDS_SPELL_RESTING_GHOST;
  int ROUNDS_NO_ATTACK_GHOST;
  
  /**
   * Reads the settings from a stream.
   */
  static Settings read_settings (istream& is);

  
  bool ok () const;
};


inline string Settings::version () {
  return "HarryPotter 1.0";
}

inline int Settings::num_players                   () const { return NUM_PLAYERS                    ;}
inline int Settings::num_rounds                    () const { return NUM_ROUNDS                     ;}
inline int Settings::board_rows                    () const { return BOARD_ROWS                     ;}
inline int Settings::board_cols                    () const { return BOARD_COLS                     ;}
inline int Settings::num_ini_wizards_per_clan      () const { return NUM_INI_WIZARDS_PER_CLAN       ;}
inline int Settings::num_ini_books                 () const { return NUM_INI_BOOKS                  ;}
inline int Settings::clan_ini_magic_strength       () const { return CLAN_INI_MAGIC_STRENGTH        ;}
inline int Settings::points_for_converting_wizard  () const { return POINTS_FOR_CONVERTING_WIZARD   ;}
inline int Settings::points_per_owned_cell         () const { return POINTS_PER_OWNED_CELL          ;}
inline int Settings::book_magic_strength           () const { return BOOK_MAGIC_STRENGTH            ;}
inline int Settings::rounds_for_converting_wizard  () const { return ROUNDS_FOR_CONVERTING          ; }
inline int Settings::rounds_attack_resting_ghost   () const { return ROUNDS_ATTACK_RESTING_GHOST    ; }
inline int Settings::rounds_spell_resting_ghost    () const { return ROUNDS_SPELL_RESTING_GHOST    ; }
inline int Settings::rounds_no_attack_ghost        () const { return ROUNDS_NO_ATTACK_GHOST         ; }

inline bool Settings::player_ok (int pl) const {
  return pl >= 0 and pl < num_players();
}

inline bool Settings::pos_ok (int i, int j) const {
  return i >= 0 and i < board_rows() and j >= 0 and j < board_cols();
}

inline bool Settings::pos_ok (Pos p) const {
  return pos_ok(p.i, p.j);
}


inline bool Settings::ok() const {
  if (num_players() != 4) return false;
  return true;
}

#endif
