//////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////

#include "Board.hh"
#include "Action.hh"

Board::Board (istream& is, int seed) {
  set_random_seed(seed);
  *static_cast<Settings*>(this) = Settings::read_settings(is);

  player2wizards = vector<set<int>>(num_players());
  ghosts         = vector<int>(num_players());

  names                  = vector<string>(num_players());
  scr                    = vector<int>   (num_players(), 0);
  scr_accumulated        = vector<int>   (num_players(), 0);
  nb_cells               = vector<int>   (num_players(), 0); // Is computed in read grid
  overall_magic_strength = vector<int>   (num_players(), clan_ini_magic_strength());
  
  stats                  = vector<double> (num_players(), 0);

  rnd = 0;

  fresh_id = 0;
  read_generator_and_grid(is);

  for (auto& p : units) fresh_id = max(fresh_id, p.id);
  ++fresh_id;
  
  _my_assert(ok(), "Invariants are not satisfied.");
}

void Board::check_is_good_initial_fixed_board () const {
  vector<int> num_wizards(num_players(),0);
  vector<int> num_ghosts(num_players(),0);
  int num_books = 0;

  _my_assert(int(grid.size()) == board_rows(), "Fixed board has wrong number of rows.");
  _my_assert(int(grid[0].size()) == board_cols(), "Fixed board has wrong number of cols.");
  
  for (int i = 0; i < board_rows(); ++i)
    for (int j = 0; j < board_cols(); ++j) {
      Cell c = grid[i][j];
      if (c.book) ++num_books;
      if (c.id != -1) {
	int id = c.id;
	_my_assert(unit_ok(id), "Unit placed in grid has wrong unit id");
	const Unit& u = units[id];
	if (u.type == Wizard) {
	  _my_assert(player_ok(u.player), "Player not ok in check_is_good_initial_fixed_board");
	  _my_assert(c.owner == u.player,  "Wizard placed in a cell but player does not own it");
	  _my_assert(u.pos == Pos(i,j),   "Wizard does not have right position");
	  _my_assert(player2wizards[u.player].count(id) != 0, "Wizard not in player2wizards");
	  ++num_wizards[u.player];
	}
	else { // We know it is a ghost
	  _my_assert(player_ok(u.player), "Player not ok in check_is_good_initial_fixed_board");
	  _my_assert(c.owner == u.player,  "Wizard placed in a cell but player does not own it");
	  _my_assert(u.pos == Pos(i,j),   "Wizard does not have right position");
	  _my_assert(ghosts[u.player] == id, "Ghost not in ghosts");
	  ++num_ghosts[u.player];	  
	}
      }
    }
  
  _my_assert(num_books == num_ini_books(),"Fixed board has wrong number of initial books.");

  for (int p = 0; p < num_players(); ++p){
    _my_assert(int(player2wizards[p].size()) == num_wizards[p], "Fixed board has wrong number of initial wizards.");
    _my_assert(ghosts[p] == num_ghosts[p], "Fixed board has wrong number of initial ghosts.");
  }
}

void Board::print_settings (ostream& os) const {

  os <<   version()                                                                      << endl;
  os                                                                                     << endl;
  os <<  "NUM_PLAYERS"                   << "\t\t\t" <<  num_players()                   << endl;
  os <<  "NUM_ROUNDS"                    << "\t\t\t" <<  num_rounds()                    << endl;
  os <<  "BOARD_ROWS"                    << "\t\t\t" <<  board_rows()                    << endl;
  os <<  "BOARD_COLS"                    << "\t\t\t" <<  board_cols()                    << endl;
  os <<  "NUM_INI_WIZARDS_PER_CLAN"      << "\t"     <<  num_ini_wizards_per_clan()      << endl;
  os <<  "NUM_INI_BOOKS"                 << "\t\t\t" <<  num_ini_books()                 << endl;
  os <<  "CLAN_INI_MAGIC_STRENGTH"       << "\t\t"   <<  clan_ini_magic_strength()       << endl;
  os <<  "POINTS_FOR_CONVERTING_WIZARD"  << "\t"     <<  points_for_converting_wizard()  << endl;
  os <<  "POINTS_PER_OWNED_CELL"         << "\t\t"   <<  points_per_owned_cell()         << endl;
  os <<  "BOOK_MAGIC_STRENGTH"           << "\t\t"   <<  book_magic_strength()           << endl;
  os <<  "ROUNDS_FOR_CONVERTING"         << "\t\t"   <<  rounds_for_converting_wizard()         << endl;
  os <<  "ROUNDS_ATTACK_RESTING_GHOST"   << "\t"     <<  rounds_attack_resting_ghost()   << endl;
  os <<  "ROUNDS_SPELL_RESTING_GHOST"    << "\t"     <<  rounds_spell_resting_ghost()   << endl;
  os <<  "ROUNDS_NO_ATTACK_GHOST"        << "\t\t"   <<  rounds_no_attack_ghost()        << endl;
}


void Board::print_names (ostream& os) const {
  os << "names         ";
  for (int pl = 0; pl < num_players(); ++pl) os << ' ' << name(pl);
  os << endl;
}


void Board::print_state (ostream& os) {

  // Should start with the same format of Info::read_grid.
  // Then other data describing the state.

  os << endl << endl;

  os << "   ";
  for (int j = 0; j < board_cols(); ++j)
    os << j / 10;
  os << endl;

  os << "   ";
  for (int j = 0; j < board_cols(); ++j)
    os << j % 10;
  os << endl;

  for (int i = 0; i < board_rows(); ++i) {
    os << i / 10 << i % 10 << " ";
    for (int j = 0; j < board_cols(); ++j) {
      const Cell& c = grid[i][j];
      if      (c.type   == Wall)     os << 'W';
      else if (c.owner == 0)         os << '0';
      else if (c.owner == 1)         os << '1';
      else if (c.owner == 2)         os << '2';
      else if (c.owner == 3)         os << '3';
      else                           os << '.';
    }
    os << endl;
  }

  os << endl << "units" << endl;
  os << units.size() << endl;
  os << "type\tid\tplayer\trow\tcolumn\trounds\tnext" << endl;
  for (const auto& u : units) {
    os << UnitType2char(u.type) << "\t";
    os << u.id << "\t";
    os << u.player << "\t";
    os << u.pos.i << "\t";
    os << u.pos.j << "\t";
    os << u.rounds_pending << "\t";
    os << u.next_player << endl;
  }

  os << endl << "books" << endl;
  // Collect them
  vector<Pos> books;
  for (int i = 0; i < board_rows(); ++i)
    for (int j = 0; j < board_cols(); ++j)
      if (grid[i][j].book) books.push_back(Pos(i,j));
  os << books.size() << endl;
  os << "row\tcolumn" << endl;
  for (const auto& p : books) {
    os << p.i << "\t";
    os << p.j << endl;
  }

  os << endl << "spell_ingredients" << endl;
  os << spell_vector.size() << endl;
  for (int x : spell_vector)
    os << x << " ";
  os << endl;

  os << endl << "Voldemort" << endl;
  os << p_voldemort.i << " " << p_voldemort.j << endl;
  os << endl;
  
  os << "round " << rnd << endl;
  os << endl;

  os << "score";
  for (auto s : scr) os << "\t" << s;
  os << endl;
  os << endl;

  os << "scr_acc";
  for (auto s : scr_accumulated) os << "\t" << s;
  os << endl;
  os << endl;

  os << "strength";
  for (auto s : overall_magic_strength) os << "\t" << s;
  os << endl;
  os << endl;

  os << "status";
  for (auto s : stats) os << "\t" << s;
  os << endl;
  os << endl;
}


void Board::print_results () const {
  int max_score = 0;
  vector<int> v;
  for (int pl = 0; pl < num_players(); ++pl) {

    cerr << "info: player " <<  name(pl)
         << " got score "   << score(pl) << endl;

    if      (score(pl) == max_score)  v.push_back(pl);
    else if (score(pl) >  max_score) {
      max_score = score(pl);
      v = vector<int>(1, pl);
    }
  }

  cerr << "info: player(s)";
  for (int pl : v) //cerr << " " << name(pl);
  cerr << " got top score" << endl;
}

// -2: u1 loses and is immediately converted
// -1: u1 loses and starts transition
//  1: u2 loses and starts transition
//  2: u2 loses and is immediately converted
int Board::result_attack (const Unit& u1, const Unit& u2) {

  int u1_strength = magic_strength(u1.player);
  int u2_strength = magic_strength(u2.player);

  if (u1_strength > 2*u2_strength) {
    // u2 immediate conversion to u1
    return 2;    
  }
  else if (u2_strength > 2*u1_strength) {
    // U1 immediate conversion to U2
    return -2;
  }

  int S = 30; // 30% times attack is a surprise and u1 wins
  if (random(0,100) < S) {
    return 1;
  }

  // Otherwise, it depends on the strength
  if (u1_strength + u2_strength == 0) { // If both no strength --> 50-50
    int r = random(0,1);
    if (r == 0) return 1;
    else return -1;
    return random(0,1) == 0; 
  }
  
  int M = 1000;
  int num = random(0,M);
  double threshold = double(u1_strength)/(u1_strength + u2_strength)*M;
  if (num < threshold) return 1;
  else return -1;
}

void Board::perform_attack (Unit& orig_u, Unit& dest_u){

  _my_assert(orig_u.rounds_pending == 0, "Attack only between non-converting units.");
  _my_assert(dest_u.rounds_pending == 0, "Attack only between non-converting units.");
  
  int r = result_attack(orig_u,dest_u);

  if (r == 1) {
    dest_u.rounds_pending = rounds_for_converting_wizard();
    dest_u.next_player = orig_u.player;
  }
  else if (r == -1) {
    orig_u.rounds_pending = rounds_for_converting_wizard();
    orig_u.next_player = dest_u.player;
  }
  else if (r == 2) {
    // dest_u is automatically part of clau(orig_u)
    int pl_loses = dest_u.player;
    int pl_wins = orig_u.player;
    
    scr_accumulated[pl_wins] += points_for_converting_wizard();
    int id = dest_u.id;
    _my_assert(player2wizards[pl_loses].count(id) == 1, "Trying to remove wizard from player but not found.");
    player2wizards[pl_loses].erase(id);
    player2wizards[pl_wins].insert(id);
    dest_u.player = pl_wins;
    grid[dest_u.pos.i][dest_u.pos.j].owner = pl_wins;
  }
  else { // r == -2
    // orig_u is automatically part of clau(dest_u)
    int pl_loses = orig_u.player;
    int pl_wins = dest_u.player;

    scr_accumulated[pl_wins] += points_for_converting_wizard();
    int id = orig_u.id;
    _my_assert(player2wizards[pl_loses].count(id) == 1, "Trying to remove wizard from player but not found.");
    player2wizards[pl_loses].erase(id);
    player2wizards[pl_wins].insert(id);
    orig_u.player = pl_wins;
    grid[orig_u.pos.i][orig_u.pos.j].owner = pl_wins;
  }
}      

void Board::apply_effects_spell (int pl) {

  int empty_cells = 0;
  vector<Pos> positions;
  for (int i = 0; i < board_rows(); ++i)
    for (int j = 0; j < board_cols(); ++j){
      if (grid[i][j].id == -1 and grid[i][j].type == Corridor) { // Empty
	++empty_cells;
	if (grid[i][j].owner != pl) positions.push_back({i,j});
      }
    }     

  int cells_to_capture = empty_cells*0.15;
  
  vector<int> perm = random_permutation(positions.size());

  for (uint i = 0; cells_to_capture > 0 and i < perm.size(); ++i) {
    Pos p = positions[perm[i]];
    _my_assert(grid[p.i][p.j].owner != pl, "Wrong computation of cells to capture in spell.");
    grid[p.i][p.j].owner = pl;
    --cells_to_capture;
  }
}

bool Board::execute (const Command&  m,
                     vector<int>&            books_to_regenerate,
		     vector<vector<int>>&    w_killed,
		     vector<bool>&           g_killed
		     ) {
  int         id     =                  m.id;
  CommandType c_type = CommandType(m.c_type);


  if (not command_type_ok(c_type)) {
    //cerr << "warning: invalid command type in command: " << c_type << endl;
    return false;
  }


  if (c_type == Move) {
    Dir dir = Dir(m.dir);
    if (not dir_ok(dir)) {
      //cerr << "warning: invalid dir in command: " << dir << endl;
      return false;
    }
    
    Unit&      un = units[id];
    UnitType type = un.type;    
    int        pl = un.player;
    Pos        op = un.pos;
    Cell&      oc = grid[op.i][op.j];
    
    if (type == Wizard and (dir == DR or dir == RU or dir == UL or dir == LD)) return false; // Wizard not diagonal
    
    Pos np = op + dir;
    if (not pos_ok(np)) {
      //cerr << "warning: cannot move to position " << np << " out of the board." << endl;
      return false;
    }
    
    Cell& nc = grid[np.i][np.j];

    if (voldemort_attacks_pos(np)) {
      oc.id = -1;
      if (type == Ghost)
	g_killed[pl] = true;
      else if (type == Wizard){
	w_killed[pl].push_back(id);
	_my_assert(player2wizards[pl].count(id) > 0, "Trying to remove non-existing wizard.");
	player2wizards[pl].erase(id);
      }
    }    
    else if (type == Ghost) {
      if (nc.type == Wall) { // Cannot move to wall
	//cerr << "warning: cannot move to position " << np << " with wall." << endl;
	return false;
      }
      else if (nc.book) { // Cell with book
	++books_to_regenerate[pl];
	nc.book = false;
	nc.owner = pl;
	nc.id = id;
	un.pos = np;
	oc.id = -1;          
      }
      else if (nc.id == -1) { // Cell with no unit
	nc.owner = pl;
	nc.id = id;
	un.pos = np;
	oc.id = -1;
      }
      else { // Cell with unit
	return false;
      }
    }
    else if (type == Wizard) {
      if (nc.type == Wall) {
	//cerr << "warning: cannot move to position " << np << " with wall." << endl;
	return false;
      }
      else if (nc.book) { // Cell with book
	++books_to_regenerate[pl];
	nc.book = false;
	nc.owner = pl;
	nc.id = id;
	un.pos = np;
	oc.id = -1;    
      }
      else if (nc.id == -1) { // Cell with no unit
	nc.owner = pl;
	nc.id = id;
	un.pos = np;
	oc.id = -1;
      }
      else { // Cell with unit (wizard or ghost)
	Unit &au = units[nc.id]; // attacked unit
	
	if (un.is_in_conversion_process()) return false; // wizard in conversion process cannot attack anybody
	
	if (au.player == pl) { // attack between same clan
	  // attacks between same clan only allows to stop converting process on wizards
	  if (au.type == Ghost) return false;
	  if (not au.is_in_conversion_process()) return false;
	  
	  // strop converting process
	  au.rounds_pending = 0;
	  au.next_player = -1;
	}
	else if (au.type == Ghost) { // Attack on ghost of a different clan
	  // If ghost recently attacked do nothing
	  if (au.next_player != -1 and round() - au.next_player < rounds_no_attack_ghost()) return false;

	  au.rounds_pending += rounds_attack_resting_ghost();
	  au.next_player = round();
	}
	else { // attack on wizard of a different clan
	  _my_assert(au.type == Wizard, "Should find wizard to attack");
	  if (au.is_in_conversion_process()) return false; // wizard already in converting process
	  perform_attack(un,au);
	}
      }
    }
  }
  else { // Spell command
    Unit&      un = units[id];
    UnitType type = un.type;
    const vector<int>& s = m.sol;

    if (type == Wizard) return false;
    if (num_rounds() - rnd <= 50) return false; // No spell during last 50 rounds
    if (un.resting_rounds() != 0) return false;

    if (spell_in_this_round) return false;
    
    if (not is_spell_solution_correct(s)) return false; // If solution is not correct

    spell_in_this_round = true;
    un.rounds_pending = rounds_spell_resting_ghost();
    
    apply_effects_spell(un.player);
  }
  
  return true;
}

bool Board::is_spell_solution_correct (const vector<int>& s) {
  // Sets are 0,1,2,3,4.....
  int num_parts = s.size()/3;
  
  vector<int> size_part(num_parts,0);
  vector<int> sum_part(num_parts,0);

  if (s.size() != spell_vector.size()) return false;
  
  for (uint i = 0; i < s.size(); ++i) {
    if (s[i] < 0 or s[i] >= num_parts) return false;
    ++size_part[s[i]];
    sum_part[s[i]] += spell_vector[i];  
  }

  int goal = sum_part[0];
  for (int i = 0; i < num_parts; ++i) {
    if (size_part[i] != 3) return false;
    if (sum_part[i] != goal) return false;
  }
  return true;
}

bool Board::is_good_pos_to_regen ( const Pos& p) const {

  if (not grid[p.i][p.j].is_empty()) return false;
  
  for (int i = p.i - 2; i <= p.i + 2; ++i) { // Check nobody is nearby
    for (int j = p.j - 2; j <= p.j + 2; ++j) {
      if (pos_ok(Pos(i,j)) and (grid[i][j].id != -1 or grid[i][j].book or p_voldemort == Pos(i,j)))
	return false;  
    }
  }
  return true;
}

Pos Board::get_random_pos_where_regenerate ( ) {
  vector<Pos> res;
  for (int i = 0; i < board_rows(); ++i){
    for (int j = 0; j < board_cols(); ++j) {
      if (is_good_pos_to_regen(Pos(i,j))) res.push_back(Pos(i,j));
    }
  }
  
  if (res.size() != 0)  return res[random(0,res.size()-1)];
  else                  return get_empty_pos();
}

bool Board::is_good_pos_to_regen_voldemort (const Pos& p) const {

  if (not grid[p.i][p.j].is_empty()) return false;
  if (not pos_ok_for_voldemort(p)) return false;
    
  
  for (int i = p.i - 2; i <= p.i + 2; ++i) { // Check nobody is nearby
    for (int j = p.j - 2; j <= p.j + 2; ++j) {
      if (pos_ok(Pos(i,j)) and (grid[i][j].id != -1 or grid[i][j].book or p_voldemort == Pos(i,j))) return false;  
    }
  }
  return true;
}

Pos Board::get_random_pos_where_regenerate_voldemort ( ) {
  vector<Pos> res;
  for (int i = 0; i < board_rows(); ++i){
    for (int j = 0; j < board_cols(); ++j) {
      if (is_good_pos_to_regen_voldemort(Pos(i,j))) res.push_back(Pos(i,j));
    }
  }
  
  if (res.size() != 0)  return res[random(0,res.size()-1)];
  else                  return get_empty_pos();
}

void Board::next (const vector<Action>& act, ostream& os) {

  _my_assert(ok(), "Invariants are not satisfied.");

  int npl = num_players();
  _my_assert(int(act.size()) == npl, "Size should be number of players.");

  // Elements to be regenerated
  vector<int> books_to_regenerate(num_players()+1,0); // we know how much books each clan has read, in order to change overall_strength at the end of the round. Last position is for books consumed by Voldemort
  vector<vector<int>> w_killed(num_players());
  vector<bool> g_killed(num_players(),false);

  
  // No spell so far
  spell_in_this_round = false;
  
  // Chooses (at most) one command per unit.
  set<int> seen;
  vector<vector<Command>> v(npl);
  for (int pl = 0; pl < npl; ++pl)
    for (const Command& m : act[pl].v) {
      int id     = m.id;
      int c_type = m.c_type;
      int dir    = m.dir;
      const vector<int>& s = m.sol;

      auto it = units[id];

      if (it.player != pl){
        //cerr << "warning: unit " << id << " of player " << it.player << " not owned by " << pl << endl;
      }
      else {
        // Here an assert as repetitions should have already been filtered out.
        _my_assert(not seen.count(id), "More than one command for the same unit.");
        seen.insert(id);

        v[pl].push_back(Command(id, c_type, dir, s));
      }
    }


  // Makes all players' commands using a random order,
  // but respecting the relative order of the units of the same player.
  // Permutations are not equally likely to avoid favoring leading clans.
  int num = 0; // Counts number of pending commands
  for (int pl = 0; pl < npl; ++pl) num += v[pl].size();

  set<int> killed;
  vector<Command> commands_done;
  vector<int> index(npl, 0);
  while (num--) {
    int q = 0; // Counts number of players with some action pending
    for (int pl = 0; pl < npl; ++pl) q += index[pl] < (int)v[pl].size();
    _my_assert(q > 0, "q > 0 in next.");
    int ran = random(1,q); // Decide whether 1st, 2nd, 3rd,, player with something pending is chosen
    int pl = -1;
    int acum = 0;
    while (acum < ran) {
      ++pl;
      acum += index[pl] < (int)v[pl].size(); // If index > ..., then player has nothing pending
                                             // and does not count
    }

    
    const Command& m = v[pl][index[pl]++];
    if (execute(m, books_to_regenerate, w_killed, g_killed)) 
      commands_done.push_back(m);


  }
  
  os << "commands" << endl;
  Action::print(commands_done, os);

  int b;
  move_voldemort(b, w_killed, g_killed);
  books_to_regenerate.back() = b;
  
  regenerate_killed_units(w_killed, g_killed);
  
  decrement_rounds_for_ghost_available();

  decrement_rounds_for_wizard_clan_change();
  
  regenerate_books_and_update_strength(books_to_regenerate);

  update_nb_cells();

  update_score();

  spell_vector = generate_spell_ingredients(15);
  
  ++rnd;

  _my_assert(ok(), "Invariants are not satisfied.");

}


void Board::decrement_rounds_for_ghost_available ( ) {
  for (auto& id : ghosts) {
    Unit& u = units[id];
    _my_assert(u.rounds_pending >= 0, "rounds_pending for ghost cannot be negative");
    if (u.rounds_pending > 0) --u.rounds_pending;
  }
}

void Board::decrement_rounds_for_wizard_clan_change ( ){
  vector<vector<int>> changes; // triples of the form [id,player_orig,player_dest]

  for (auto& u : units) {
    _my_assert(u.rounds_pending >= 0, "rounds_pending for wizard cannot be negative");
    if (u.type == Wizard and u.rounds_pending > 0) {
      --u.rounds_pending;
      if (u.rounds_pending == 0){
	changes.push_back({u.id,u.player,u.next_player});
	u.next_player = -1;
      }
    }
  }

  for (auto& t : changes) {
    int id = t[0];
    int pl_orig = t[1];
    int pl_dest = t[2];
    scr_accumulated[pl_dest] += points_for_converting_wizard();
    _my_assert(player2wizards[pl_orig].count(id) == 1, "Trying to remove wizard from player but not found.");
    player2wizards[pl_orig].erase(id);
    player2wizards[pl_dest].insert(id);
    Unit& u = units[id];
    u.player = pl_dest;
    grid[u.pos.i][u.pos.j].owner = pl_dest;
  }
}

void Board::generate_book_item ( ){
  Pos p =  get_random_pos_where_regenerate( );
  Cell& c = grid[p.i][p.j];
  _my_assert(c.id == -1 and c.type == Corridor and not(c.book), "Generated book in already full cell");  
  c.book = true;
}

void Board::regenerate_books_and_update_strength (vector<int>& books_to_regenerate) {
  for (int p = 0; p < num_players(); ++p) {
    overall_magic_strength[p] += book_magic_strength()*books_to_regenerate[p];
    for (int k = 0; k < books_to_regenerate[p];++k)
      generate_book_item();
  }
  
  for (int k = 0; k < books_to_regenerate.back(); ++k)
    generate_book_item();
}

void Board::update_nb_cells ( ) {
  // Set all to zero
  for (auto& x : nb_cells) x = 0;

  // Add the ones in the grid
  for (int i = 0; i < board_rows(); ++i)
    for (int j = 0; j < board_cols(); ++j) 
      if (grid[i][j].owner != -1) 
	++nb_cells[grid[i][j].owner];
}

void Board::update_score ( ) {
  for (int p = 0; p < num_players(); ++p)
    scr[p] = scr_accumulated[p] + nb_cells[p]*points_per_owned_cell();
}

vector<Dir> Board::dir_permutation ( ) {
  vector<Dir> dirs = {Up, Down, Left, Right};
  vector<int> p = random_permutation(4);
  vector<Dir> new_dirs(4);
  for (int i = 0; i < 4; ++i) new_dirs[i] = dirs[p[i]];
  return new_dirs;
}

void Board::generate_triple(int T, int& x, int& y, int& z) {
  int m = T/4;
  int M = T/2;
  bool stop = false;
  while (not stop) {
    x = random(m,M);
    y = random(m,M);
    z = T - x - y;
    if (z >= m and z <= M) stop = true;
  }
  
}

vector<int> Board::generate_spell_ingredients (int n) {
  _my_assert(n%3 == 0, "Only spell ingredients of size multiple of 3 should be generated");

  vector<int> res;
  int m = n/3;
  int each_partition = random(5,30);
  for (int i = 0; i < m; ++i) {
    int x, y, z;
    generate_triple(each_partition,x,y,z);
    res.push_back(x);
    res.push_back(y);
    res.push_back(z);
  }
  // Random shuffle
  vector<int> aux = random_permutation(res.size());
  vector<int> res_shuff(res.size());
  for (uint i = 0; i < aux.size(); ++i) res_shuff[aux[i]] = res[i];

  return res_shuff;
}

Pos Board::new_pos_for_voldemort ( ) {
  // First collect (in possib) the positions of all units at mininum distance
  Pos p1 = p_voldemort;
  int minim = 1e8;
  vector<Pos> possib;
  for (const Unit& u : units) {
    Pos p2 = u.pos;
    int dist = abs(p2.i - p1.i) + abs(p2.j - p1.j);
    if (dist < minim) {
      minim = dist;
      possib = {p2};
    }
    else if (dist == minim) possib.push_back(p2);
  }

  _my_assert(possib.size() > 0, "Voldemort cannot find position to move");
  Pos goal = possib[random(0,possib.size()-1)];
  
  // Now check to which position should move to get closer to goal
  int res = 8;
  int maxim = 1e8;
  for (int d = 0; d < 8; d += 2) {
    Pos p2 = p1 + Dir(d);
    if (pos_ok(p2)) {
      int di = abs(p2.i - goal.i);
      int dj = abs(p2.j - goal.j);
      int mx = (di > dj ? di : dj);
      int dist = di + dj;
      if (dist < minim or (dist == minim and mx < maxim)) {
	res = d;
	minim = dist;
	maxim = mx;
      }
    }
  }

  return p1 + Dir(res);
}

void Board::move_voldemort ( int &books_consumed, vector<vector<int>>& wizards_killed, vector<bool>& ghost_killed ) {
  books_consumed = 0;

  p_voldemort = new_pos_for_voldemort();  

  for (int d = 0; d < 8; ++d) {
    Pos p = p_voldemort + Dir(d);
    _my_assert(pos_ok(p), "Pos not ok in Balrog surroundings.");
    Cell& c = grid[p.i][p.j];
    if (c.book) {grid[p.i][p.j].book = false; ++books_consumed;}
    c.owner = -1;
    if (c.id != -1) {
      const Unit& u = unit(c.id);
      int id = c.id;
      c.id = -1; // Removes from cell
      if (u.type == Ghost) {
	ghost_killed[u.player] = true;
      }
      else if (u.type == Wizard) {
	_my_assert(player2wizards[u.player].count(id) > 0, "Trying to remove non-existing wizard.");
	player2wizards[u.player].erase(id);
	wizards_killed[u.player].push_back(id);
      }
    }
  }
}

void Board::regenerate_wizard (int id){

  Pos p =  get_random_pos_where_regenerate( );
  int pl = random(0,3);  
  Cell& c = grid[p.i][p.j];
  _my_assert(c.id == -1 and not(c.book) and c.type == Corridor, "Generated wizard in already full cell");  

  c.owner = pl;
  c.id = id;
  c.book = false;

  Unit& u = units[id];
  u.player = pl;
  u.pos = p;
  u.rounds_pending = 0;
  u.next_player = -1;

  player2wizards[pl].insert(id);
}

void Board::regenerate_ghost (int pl) {
  Pos p =  get_random_pos_where_regenerate( );
  Cell& c = grid[p.i][p.j];
  _my_assert(c.id == -1 and not (c.book) and c.type == Corridor, "Generated wizard in already full cell");

  c.owner = pl;
  c.id = ghosts[pl];
  c.book = false;

  Unit& u = units[c.id];
  u.pos = p;
  u.rounds_pending = rounds_spell_resting_ghost();
  u.next_player = -1;
}

void Board::regenerate_killed_units (const vector<vector<int>>& w_killed, const vector<bool>& g_killed) {
  for (uint p = 0; p < w_killed.size(); ++p) {
    for (int id : w_killed[p])
      regenerate_wizard(id);
  }
  for (uint p = 0; p < g_killed.size(); ++p)
    if (g_killed[p]) regenerate_ghost(p);
}

bool Board::voldemort_attacks_pos (const Pos& p) const {
  return abs(p.i - p_voldemort.i) <= 1 and abs(p.j - p_voldemort.j) <= 1;  
}

//************************************************************
//                     BOARD GENERATION
//************************************************************

int Board::generate_wall (int s_id, int length) {
  int filled = 0;
  vector<Dir> dirs = {Up, Down, Left, Right};
  Dir last_dir = dirs[random(0,dirs.size()-1)];
  Pos p = get_ok_pos_for_initial_wall();
  wall_plan[p.i][p.j] = s_id;
  ++filled;
  while (length > 0) {
    //random_shuffle(dirs.begin(),dirs.end()); // Careful: this does not behave the same in MAC
    dirs = dir_permutation();
    Dir new_possible_dir = Up; // Explore the possibility of turning
    bool dir_found = false;
    for (auto& d:dirs) {
      if (pos_ok_for_wall(s_id,p+d)){
	new_possible_dir = d;
	dir_found = true;
	break;
      }
    }
    
    if (random(1,8) != 1 and pos_ok_for_wall(s_id,p+last_dir)){ // Continue same direction
      p += last_dir;
      wall_plan[p.i][p.j] = s_id;
      --length;
      ++filled;
    }
    else if (dir_found){ // Turn
      last_dir = new_possible_dir;
      _my_assert(cell(p+new_possible_dir).is_empty(), "Cell no empty");
      p += new_possible_dir;
      _my_assert(pos_ok_for_wall(s_id,p), "Pos not ok for street.");
      wall_plan[p.i][p.j] = s_id;
      --length;
      ++filled;
    }
    else // Stop
      return filled;
  }
  return filled;
}

void Board::generate_all_walls (int num_wall_cells, int num_walls){
  wall_plan = vector<vector<int>>(board_rows(),vector<int>(board_cols(),0));
  
  int num_walls_pending = num_walls;
  while (num_walls_pending > 0) {
    int length;
    if (num_walls_pending != 1) length = num_wall_cells/num_walls_pending;
    else length = num_wall_cells;
    num_wall_cells -= generate_wall(num_walls_pending,length);
    --num_walls_pending;
  }

  for (int i = 0; i < board_rows(); ++i) {
    for (int j = 0; j < board_cols(); ++j){
      if (wall_plan[i][j] != 0) {
	grid[i][j].type = Wall;
      }
    }
  }
  
}

Pos Board::get_empty_pos ( ){
  while (true) {
    int i = random(0,board_rows()-1);
    int j = random(0,board_cols()-1);
    if (cell(i,j).is_empty()) return Pos(i,j);
  }
}

Pos Board::get_ok_pos_for_wall(int s_id) {
  while (true) {
    int i = random(1,board_rows()-2);
    int j = random(1,board_cols()-2);
    if (pos_ok_for_wall(s_id, Pos(i,j))) return Pos(i,j);
  }
}

Pos Board::get_ok_pos_for_initial_wall() {
  while (true) {
    int i = random(1,board_rows()-2);
    int j = random(1,board_cols()-2);
    if (pos_ok_for_initial_wall(Pos(i,j))) return Pos(i,j);
  }
}

bool Board::pos_ok_for_wall(int s_id, const Pos& p){
  int i = p.i, j = p.j;
  if (not pos_ok(p)) return false;
  if (wall_plan[i][j] != 0) return false;
  if (i == 0) return false;
  if (i == board_rows() -1) return false;
  if (j == 0) return false;
  if (j == board_cols() -1) return false;


  
  int num_occupied = 0;
  vector<Dir> dirs = {Up, Down, Left, Right};
  for (auto& d: dirs) {
    Pos newPos = p + d; // Will exists because p is not on a border
    int ni = newPos.i, nj = newPos.j;
    if (wall_plan[ni][nj] != 0 and wall_plan[ni][nj] != s_id) return false;
    else if (wall_plan[ni][nj] == s_id) ++num_occupied;
  }

  vector<pair<int,int>> diags = {{1,1},{1,-1},{-1,1},{-1,-1}};
  for (auto& d : diags) {
    Pos newPos = Pos(p.i + d.first, p.j + d.second);
    int ni = newPos.i, nj = newPos.j;
    if (wall_plan[ni][nj] != 0 and wall_plan[ni][nj] != s_id) return false;
    else if (wall_plan[ni][nj] == s_id) ++num_occupied;
  }


  return num_occupied <= 2;
}

bool Board::pos_ok_for_initial_wall(const Pos& p){
  int i = p.i, j = p.j;
  if (not pos_ok(p)) return false;
  if (wall_plan[i][j] != 0) return false;

  if (i == 0) return false;
  if (i == board_rows() -1) return false;
  if (j == 0) return false;
  if (j == board_cols() -1) return false;

  int num_occupied = 0;
  vector<Dir> dirs = {Up, Down, Left, Right};
  for (auto& d: dirs) {
    Pos newPos = p + d; // Will exists because p is not on a border
    int n_i = newPos.i, n_j = newPos.j;
    if (wall_plan[n_i][n_j] != 0) return false;
  }

  vector<pair<int,int>> diags = {{1,1},{1,-1},{-1,1},{-1,-1}};
  for (auto& d : diags) {
    Pos newPos = Pos(p.i + d.first, p.j + d.second);
    int n_i = newPos.i, n_j = newPos.j;
    if (wall_plan[n_i][n_j] != 0) return false;
  }
      
  return true;
}

void Board::explore_from(vector<vector<int>>& G, int i, int j, int n) {
  G[i][j] = n;
  for (auto& d:{Up,Down,Left,Right}) {
    Pos np = Pos(i,j) + d;
    if (pos_ok(np) and G[np.i][np.j] == -1) explore_from(G,np.i,np.j,n);
  }
}

int Board::num_connected_components( ){
  vector<vector<int>> G(board_rows(),vector<int>(board_cols(),-1));

  for (int i = 0; i < board_rows(); ++i) 
    for (int j = 0; j < board_cols(); ++j)
      if (grid[i][j].type == Wall) G[i][j] = -2;

  int n = 0;    
  for (int i = 0; i < board_rows(); ++i) {
    for (int j = 0; j < board_cols(); ++j){
      if (G[i][j] == -1) {
	explore_from(G,i,j,n);
	++n;
      }
    }
  }

  return n;
}

void Board::create_new_ghost (Pos& p, int player) {
  int id = fresh_id;
  ++fresh_id;

  units.push_back(Unit(Ghost, id, player, p, rounds_spell_resting_ghost(), -1));
  _my_assert((int)units.size() - 1  == id, "Wrong unit id usage for ghosts");

  ghosts[player] = id;
  
  _my_assert(grid[p.i][p.j].is_empty(),        "Cell is already full.");

  grid[p.i][p.j].id = id;
  grid[p.i][p.j].owner = player;
}

void Board::create_new_wizard (Pos& p, int player) {
  int id = fresh_id;
  ++fresh_id;

  units.push_back(Unit(Wizard, id, player, p, 0, -1));
  _my_assert((int)units.size() - 1  == id, "Wrong unit id usage for wizards");

  _my_assert(player2wizards[player].count(id) == 0, "Id already used in wizards");
  player2wizards[player].insert(id);
  
  _my_assert(grid[p.i][p.j].is_empty(),        "Cell is already full.");

  grid[p.i][p.j].id = id;
  grid[p.i][p.j].owner = player;
}

void Board::generate_random_board ( ){
  int rows = board_rows();
  int cols = board_cols();

  
  // Generate buildings (leaving space for citizens)  
  static const int num_wall_cells = 0.5*rows*cols; // goal of 50% waste
  static const int num_walls = 8;

  do {
    // Create grid
    grid = vector<vector<Cell>>(rows, vector<Cell>(cols));
    generate_all_walls(num_wall_cells, num_walls);
  } while (num_connected_components() != 1);

  for (int i = 0; i < board_rows(); ++i) 
    for (int j = 0; j < board_cols(); ++j)
      if (wall_plan[i][j] != 0) 
	grid[i][j].type = Wall;
  

  p_voldemort = {-1,-1}; // So that it is initialized (out of the grid for now)
  
  // Generate ghosts
  for (int pl = 0; pl < num_players(); ++pl) {
    Pos p = get_random_pos_where_regenerate();
    create_new_ghost(p,pl);
    ++nb_cells[pl];
  }

  // Generate wizards
  for (int pl = 0; pl < num_players(); ++pl) {
    for (int i = 0; i < num_ini_wizards_per_clan(); ++i) {
      Pos p = get_random_pos_where_regenerate( );
      create_new_wizard(p,pl);
      ++nb_cells[pl];
    }
  }
  
  // Generate books  
  for (int i = 0; i < num_ini_books(); ++i) {
    Pos p = get_random_pos_where_regenerate( );
    grid[p.i][p.j].book = true;
  }

  // Generate spell ingredients
  spell_vector = generate_spell_ingredients(15);

  // Generate Voldemort
  p_voldemort = get_random_pos_where_regenerate_voldemort( );
  
}

