#include "Settings.hh"


Settings Settings::read_settings (istream& is) {
  Settings r;
  string s, v;

  // Version, compared part by part.
  istringstream vs(version());
  while (!vs.eof()) {
    is >> s;
    vs >> v;
    assert(s == v);
  };
  
  is >> s >> r.NUM_PLAYERS;
  _my_assert(s == "NUM_PLAYERS", "Expected 'NUM_PLAYERS' while parsing.");
  _my_assert(r.NUM_PLAYERS == 4, "NUM_PLAYERS should be 4.");

  is >> s >> r.NUM_ROUNDS;
  _my_assert(s == "NUM_ROUNDS", "Expected 'NUM_ROUNDS' while parsing.");
  _my_assert(r.NUM_ROUNDS >= 1,   "NUM_ROUNDS should be >=1.");

  is >> s >> r.BOARD_ROWS;
  _my_assert(s == "BOARD_ROWS", "Expected 'BOARD_ROWS' while parsing.");
  _my_assert(r.BOARD_ROWS == 60, "BOARD_COLS should be 60.");
  
  is >> s >> r.BOARD_COLS;
  _my_assert(s == "BOARD_COLS", "Expected 'BOARD_COLS' while parsing.");
  _my_assert(r.BOARD_COLS == 60, "BOARD_COLS should be 60.");

  is >> s >> r.NUM_INI_WIZARDS_PER_CLAN;
  _my_assert(s == "NUM_INI_WIZARDS_PER_CLAN", "Expected 'NUM_INI_WIZARDS_PER_CLAN' while parsing.");
  _my_assert(r.NUM_INI_WIZARDS_PER_CLAN >= 1, "NUM_INI_WIZARDS_PER_CLAN should be >= 1.");
  
  is >> s >> r.NUM_INI_BOOKS;
  _my_assert(s == "NUM_INI_BOOKS", "Expected 'NUM_INI_BOOKS' while parsing.");
  _my_assert(r.NUM_INI_BOOKS >=1, "NUM_INI_BOOKS should be >=1.");

  is >> s >> r.CLAN_INI_MAGIC_STRENGTH;
  _my_assert(s == "CLAN_INI_MAGIC_STRENGTH", "Expected 'CLAN_INI_MAGIC_STRENGTH' while parsing.");
  _my_assert(r.CLAN_INI_MAGIC_STRENGTH >=1, "CLAN_INI_MAGIC_STRENGTH should be >=1.");

  is >> s >> r.POINTS_FOR_CONVERTING_WIZARD;
  _my_assert(s == "POINTS_FOR_CONVERTING_WIZARD", "Expected 'POINTS_FOR_CONVERTING_WIZARD' while parsing.");
  _my_assert(r.POINTS_FOR_CONVERTING_WIZARD >=1, "POINTS_FOR_CONVERTING_WIZARD should be >=1.");

  is >> s >> r.POINTS_PER_OWNED_CELL;
  _my_assert(s == "POINTS_PER_OWNED_CELL", "Expected 'POINTS_PER_OWNED_CELL' while parsing.");
  _my_assert(r.POINTS_PER_OWNED_CELL >= 1, "POINTS_PER_OWNED_CELL should be >= 1");

  is >> s >> r.BOOK_MAGIC_STRENGTH;
  _my_assert(s == "BOOK_MAGIC_STRENGTH", "Expected 'BOOK_MAGIC_STRENGTH' while parsing.");
  _my_assert(r.BOOK_MAGIC_STRENGTH >= 1, "BOOK_MAGIC_STRENGTH should be >= 1");

  is >> s >> r.ROUNDS_FOR_CONVERTING;
  _my_assert(s == "ROUNDS_FOR_CONVERTING", "Expected 'ROUNDS_FOR_CONVERTING' while parsing.");
  _my_assert(r.ROUNDS_FOR_CONVERTING >= 1, "ROUNDS_FOR_CONVERTING should be >= 1");

  is >> s >> r.ROUNDS_ATTACK_RESTING_GHOST;
  _my_assert(s == "ROUNDS_ATTACK_RESTING_GHOST", "Expected 'ROUNDS_ATTACK_RESTING_GHOST' while parsing.");
  _my_assert(r.ROUNDS_ATTACK_RESTING_GHOST >= 1, "ROUNDS_ATTACK_RESTING_GHOST should be >= 1");

  is >> s >> r.ROUNDS_SPELL_RESTING_GHOST;
  _my_assert(s == "ROUNDS_SPELL_RESTING_GHOST", "Expected 'ROUNDS_SPELL_RESTING_GHOST' while parsing.");
  _my_assert(r.ROUNDS_SPELL_RESTING_GHOST >= 1, "ROUNDS_SPELL_RESTING_GHOST should be >= 1");

  is >> s >> r.ROUNDS_NO_ATTACK_GHOST;
  _my_assert(s == "ROUNDS_NO_ATTACK_GHOST", "Expected 'ROUNDS_NO_ATTACK_GHOST' while parsing.");
  _my_assert(r.ROUNDS_NO_ATTACK_GHOST >= 1, "ROUNDS_NO_ATTACK_GHOST should be >= 1");

  return r;
}
