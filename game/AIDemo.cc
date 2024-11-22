#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Demo

// DISCLAIMER: The following Demo player is *not* meant to do anything
// sensible. It is provided just to illustrate how to use the API.
// Please use AINull.cc as a template for your player.

struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }


  /**
   * Types and attributes for your player can be defined here.
   */
  const vector<Dir> dirs = {Up,Down,Left,Right};



  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {

    // If nearly out of time, do nothing.
    double st = status(me());
    if (st >= 0.9) return;

    // If more than halfway through, do nothing.
    //if (round() > num_rounds()/2) return;

    // Getting the wizards
    vector<int> ws = wizards(me());

    // Write debugging info about my units
    //cerr << "At round " << round() << " player " << me() << " has " << ws.size() << " wizards: ";
    for (auto id : ws) 
      //cerr << id << " at pos " << unit(id).pos << "; ";
    //cerr << endl;

    if (round()% 2 == 0) {
      // Move all wizards randomly (avoiding out of bounds and walls)
      for (int id : ws) {
	Dir d = dirs[random(0,dirs.size()-1)];
	Pos new_pos = unit(id).pos + d;
	if (pos_ok(new_pos) and cell(new_pos.i,new_pos.j).type != Wall) move(id,d);
      }
    }
    else {
      if (random(0,3) <= 1) {// Do this with 25% probability [move all units to the left]
	for (int id : ws) {
	  Dir d = Left;
	  Pos new_pos = unit(id).pos + d;
	  if (pos_ok(new_pos) and cell(new_pos.i,new_pos.j).type != Wall) move(id,d);
	}
      }
      else { // With probability 75%: attack adjacent wizard enemies
	for (int id : ws) {
	  for (auto d : dirs) {
	    Pos new_pos = unit(id).pos + d;
	    if (pos_ok(new_pos)) {
	      int id_in_cell = cell(new_pos.i,new_pos.j).id;
	      if (id_in_cell != -1 and unit(id_in_cell).type == Wizard and unit(id_in_cell).player != me()) {move(id,d); break;}
	    }
	  }
	}
      }
    }

    // Try to solve spell
    int g = ghost(me());
    spell(g,{0,2,4,1,3,2,0,1,4,1,4,0,3,3,2});
    
  }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
