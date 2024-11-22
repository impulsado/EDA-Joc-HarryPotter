//////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////  

#include "Info.hh"

bool Info::ok() const {

  if (int(grid.size()) != board_rows()) {
    //cerr << "error: mismatch in number of rows" << endl;
    return false;
  }

  if (int(grid[0].size()) != board_cols()) {
    //cerr << "error: mismatch in number of columns" << endl;
    return false;
  }

  if (not (rnd >= 0 and rnd <= num_rounds())) {
    //cerr << "error: wrong number of rounds" << endl;
    return false;
  }
  
  for (int pl = 0; pl < num_players(); ++pl) {
    double st = stats[pl];
    if (st != -1 and not (0 <= st and st <= 1)) {
      //cerr << "error: status should be -1 or within [0, 1]" << endl;
      return false;
    }
  }

  if (int(player2wizards.size()) != num_players()) {
    //cerr << "error: size of player2wizards should be number of players" << endl;
    return false;
  }

  if (int(ghosts.size()) != num_players()) {
    //cerr << "error: size of ghosts should be number of players" << endl;
    return false;
  }

  uint expected_units = num_players() * (num_ini_wizards_per_clan()+1);
  if (units.size() != expected_units) {
    //cerr << "error: units has a wrong number of units" << endl;
    return false;
  }
  
  vector<int> tmp_nb_cells(num_players(), 0);
  uint total_ids_in_cells = 0;
  
  for (int i = 0; i < board_rows(); ++i) {
    for (int j = 0; j < board_cols(); ++j) {
      const Cell& c = grid[i][j];

      if (c.type == Wall) {
	if (c.owner != -1) {
	  //cerr << "error: wall cells cannot have owner" << endl;
	  return false;
	}
	if (c.id != -1) {
	  //cerr << "error: wall cells cannot have a unit on them" << endl;
	  return false;
	}
	if (c.book) {
	  //cerr << "error: wall cells cannot have books" << endl;
	  return false;
	}
      }
      else if (c.type == Corridor) {
	if (c.book and c.id != -1) {
	  //cerr << "error: corridor cells cannot have books and a unit on them" << endl;
	  return false;
	}
	
	if (c.id != -1) { // Some unit on the cell
	  if (not unit_ok(c.id)) {
            //cerr << "error: could not find unit identifier " << c.id << endl;
            return false;
          }
	  
	  const Unit& u = units[c.id];
          if (u.pos != Pos(i, j)) {
            //cerr << "error: mismatch in the position of unit " << c.id << endl;
            return false;
          }
	  if (u.id != c.id) {
	    //cerr << "error: mismatch in the identifier of unit at pos " << Pos(i,j) << endl;
	    return false;
	  }

	  ++total_ids_in_cells;
	  	  
	  if (u.player != c.owner) { // It has wizard/ghost
	    //cerr << "error: cell contains a unit but is not owned by her player" << endl;
	    return false;
	  }

	  if (u.type == Ghost and ghosts[u.player] != u.id) {
	    //cerr << "error: ghost at position " << Pos(i,j) << " is not in ghosts" << endl;
	    return false;
	  }

	  if (u.type == Wizard and player2wizards[u.player].count(u.id) == 0) {
	    //cerr << "error: wizard at posision " << Pos(i,j) << " is not in player2wizards" << endl;
	    return false;
	  }
	}
	
	if (c.owner != -1) {
	  if (c.owner < 0 or c.owner >= num_players()) {
	    //cerr << "error: owner of a cell should be a number between 0 and " << num_players() - 1 << endl;
	    return false;
	  }
	  ++tmp_nb_cells[c.owner];	  
	}
      }
      else {
        //cerr << "error: cells should be either Corridor or Waste" << endl;
        return false;	
      }
    }
  }

  if (total_ids_in_cells != expected_units) {
    //cerr << "error: wrong number of units in the grid" << endl;
    return false;
  }

  if (nb_cells != tmp_nb_cells) {
    //cerr << "error: nb_cells structure does not match the contents of the grid" << endl;
    return false;
  }
  
  uint tmp_units = 0;
  for (auto& x : player2wizards) tmp_units += x.size();
  tmp_units += ghosts.size();

  if (tmp_units != expected_units) {
    //cerr << "error: wrong number of units in player2wizards + ghosts" << endl;
    return false;
  }

  if (not (pos_ok_for_voldemort(p_voldemort))) {
    //cerr << "error: Voldemort not in right position " << p_voldemort << endl;    
    return false;
  }

  if (voldemort_attacks( )) {
    //cerr << "error: Voldemort attacks " << endl;    
    return false;
  }
  
  for (const auto& u : units) {
    if (not player_ok(u.player)) {
      //cerr << "error: wrong player identifier" << endl;
      return false;
    }
    
    if (u.id < 0 or u.id >= int(expected_units)){
      //cerr << "error: wrong identifier for unit" << endl;
      return false;
    }


    if (u.type == Wizard) {
      if (u.rounds_pending > 0 and u.next_player == -1) {
 	//cerr << "error: wizard not in converting process cannot have next_player" << u.next_player << endl;
	return false;
      }
    }

    if (u.type == Ghost) {
      if (u.rounds_pending < 0) {
	//cerr << "error: ghost cannot have negative rounds_pending" << endl;
	return false;
      }
    }
  }
  return true;
}
