#include "Player.hh"

#define PLAYER_NAME AIRex_0_0_0
#define SIZE 60

#define iCLOSE 0
#define iMID 1
#define iFAR 2
#define iLOW 0
#define iMEDIUM 1
#define iHIGH 2

/**

*/

/** TO-DO
- [ ] Spell Fantasma 
*/


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
  enum Priority {
    // === WIZARD ===
    // -- MAG ALIAT PER SALVAR
    WIZARD_ALLIED_CLOSE = 1,
    WIZARD_ALLIED_MID = 4,
    WIZARD_ALLIED_FAR = 9,    
    // -- MAG ENEMIG --
    WIZARD_ENEMY_WIZARD_CLOSE_HIGH = 2,
    WIZARD_ENEMY_WIZARD_CLOSE_MEDIUM = 7,
    WIZARD_ENEMY_WIZARD_CLOSE_LOW = 28,
    WIZARD_ENEMY_WIZARD_MID_HIGH = 13,
    WIZARD_ENEMY_WIZARD_MID_MEDIUM = 19,
    WIZARD_ENEMY_WIZARD_MID_LOW = 20,
    WIZARD_ENEMY_WIZARD_FAR_HIGH = 21,
    WIZARD_ENEMY_WIZARD_FAR_MEDIUM = 22,
    WIZARD_ENEMY_WIZARD_FAR_LOW = 23,   
    // -- FANTASMA ENEMIG --
    WIZARD_ENEMY_GHOST_CLOSE = 3,
    WIZARD_ENEMY_GHOST_MID = 5,
    WIZARD_ENEMY_GHOST_FAR = 11,    
    // -- LLIBRE --
    WIZARD_BOOK_CLOSE = 4,
    WIZARD_BOOK_MID = 10,
    WIZARD_BOOK_FAR = 12,   
    // -- VOLDEMORT --
    WIZARD_VOLDEMORT_CLOSE = 0,
    WIZARD_VOLDEMORT_MID = 8,
    WIZARD_VOLDEMORT_FAR = 14,    
    // -- EMPTY --
    WIZARD_EMPTY_CLOSE = 6,
    WIZARD_EMPTY_MID = 15,
    WIZARD_EMPTY_FAR = 16,    
    
    // === GHOSTS
    // --- MAG ENEMIG --
    GHOST_ENEMY_WIZARD_CLOSE = 10,
    GHOST_ENEMY_WIZARD_MID = 7,
    GHOST_ENEMY_WIZARD_FAR = 2,   
    // -- LLIBRE --
    GHOST_BOOK_CLOSE_LOW = 4,
    GHOST_BOOK_MID = 12,
    GHOST_BOOK_FAR = 13,    
    // -- VOLDEMORT --       
    GHOST_VOLDEMORT_CLOSE = 0,
    GHOST_VOLDEMORT_MID = 8,
    GHOST_VOLDEMORT_FAR = 14,   
    // -- EMPTY --
    GHOST_EMPTY_CLOSE = 6,
    GHOST_EMPTY_MID = 15,
    GHOST_EMPTY_FAR = 16,   
    
    // == DEFAULT 
    NOT_DEFINED = INT32_MAX,
  };

  enum DistRanges {
    CLOSE = 2,
    MID = 5,
    FAR = 10,
  };

  enum ProbRanges {
    LOW = 33,
    MEDIUM = 66,
    HIGH = 100,
  };

  enum Content {
    WALL,     // Paret
    OWNED,    // No té res però és meva
    EMPTY,    // No és meva. Pot ser del enemic o buida
    ENEMY_WIZARD,   // Mag enemig
    ENEMY_GHOST,    // Fantasma enemig
    ALLIED,   // Company    
    BOOK,     // Llibre
    VOLDEMORT,
  };

  vector<Dir> dirs_wizard = {Up,Down,Left,Right};
  vector<Dir> dirs_all = {Down,DR,Right,RU,Up,UL,Left,LD};

  struct Movement {
    int id = -1;  // id de l'unitat que realitza el moviment
    Priority prio = NOT_DEFINED;  // Prioritat de l'unitat a fer aquest moviment
    Dir d;  // Direcció que s'ha de moure l'unitat (No es fa ús en tauler de moviments)
    Pos p;

    bool operator< (const Movement& other) const {
      if (prio != other.prio) return prio < other.prio;  // Menor valor de prioridad primero
      return id < other.id;      // Menor ID primero
    }
  };

  struct CompMovementPriority {
    // true: m2,m1
    // false: m1,m2
    // m1 abans que m2 si m1 té menys prioritat.
    bool operator() (const Movement& m1, const Movement& m2) {
      return m1.prio > m2.prio;
    }
  };

  typedef set<int> SI;
  typedef set<Movement> SM;

  typedef vector<int> VI;
  
  typedef pair<int, int> P;
  typedef vector<P> VP;

  typedef vector<Content> VC;
  typedef vector<VC> VVC;

  typedef vector<bool> VB;
  typedef vector<VB> VVB;

  typedef vector<Movement> VM;
  typedef vector<VM> VVM;

  typedef queue<Pos> QPOS;

  typedef priority_queue<Movement, vector<Movement>, CompMovementPriority> PQM;
  
  typedef unordered_map<Content, vector<vector<Priority>>> PT;

  PT probability_table_wizards = PT();
  PT probability_table_ghost = PT();

  VI vstr = VI(4);  // Força del i-essim equip
  SI smy_units;  // Set amb les meves unitats vives
  SM smovements;  // Set amb els moviments de les meves unitats
  VVM movement_board = VVM(SIZE, VM(SIZE, Movement()));
  VVC content_board = VVC(SIZE, VC(SIZE));  // Tauler amb els objectes
  bool initialized = false;

  /**
   * Play method, invoked once per each round.
   */
  // Inicialitzar totes les PT.
  void initPT() {
    initPTWizards();
    initPTGhost();
  }

  // Inicialitzar la PT relacionada amb prioritats de Mags.
  void initPTWizards() {
    // probability_table[TYPE][<=dist][<=winrate] = Priority
    int dist_ranges = 3;  // CLOSE | MID | FAR
    int prob_ranges = 3;  // LOW | MEDIUM  | HIGH

    // -- MAG ALIAT PER SALVAR --
    probability_table_wizards[ALLIED] = std::vector<std::vector<Priority>>(dist_ranges, std::vector<Priority>(1, NOT_DEFINED));
    probability_table_wizards[ALLIED][iCLOSE][0]     = WIZARD_ALLIED_CLOSE;
    probability_table_wizards[ALLIED][iMID][0]       = WIZARD_ALLIED_MID;
    probability_table_wizards[ALLIED][iFAR][0]       = WIZARD_ALLIED_FAR;

    // -- MAG ENEMIG --
    probability_table_wizards[ENEMY_WIZARD] = std::vector<std::vector<Priority>>(dist_ranges, std::vector<Priority>(prob_ranges, NOT_DEFINED));
    probability_table_wizards[ENEMY_WIZARD][iCLOSE][iLOW]     = WIZARD_ENEMY_WIZARD_CLOSE_LOW;
    probability_table_wizards[ENEMY_WIZARD][iCLOSE][iMEDIUM]  = WIZARD_ENEMY_WIZARD_CLOSE_MEDIUM;
    probability_table_wizards[ENEMY_WIZARD][iCLOSE][iHIGH]    = WIZARD_ENEMY_WIZARD_CLOSE_HIGH;
    probability_table_wizards[ENEMY_WIZARD][iMID][iLOW]       = WIZARD_ENEMY_WIZARD_MID_LOW;
    probability_table_wizards[ENEMY_WIZARD][iMID][iMEDIUM]    = WIZARD_ENEMY_WIZARD_MID_MEDIUM;
    probability_table_wizards[ENEMY_WIZARD][iMID][iHIGH]      = WIZARD_ENEMY_WIZARD_MID_HIGH;
    probability_table_wizards[ENEMY_WIZARD][iFAR][iLOW]       = WIZARD_ENEMY_WIZARD_FAR_LOW;
    probability_table_wizards[ENEMY_WIZARD][iFAR][iMEDIUM]    = WIZARD_ENEMY_WIZARD_FAR_MEDIUM;
    probability_table_wizards[ENEMY_WIZARD][iFAR][iHIGH]      = WIZARD_ENEMY_WIZARD_FAR_HIGH;

    // -- FANTASMA ENEMIG --
    probability_table_wizards[ENEMY_GHOST] = std::vector<std::vector<Priority>>(dist_ranges, std::vector<Priority>(1, NOT_DEFINED));
    probability_table_wizards[ENEMY_GHOST][iCLOSE][0]   = WIZARD_ENEMY_GHOST_CLOSE;
    probability_table_wizards[ENEMY_GHOST][iMID][0]     = WIZARD_ENEMY_GHOST_MID;
    probability_table_wizards[ENEMY_GHOST][iFAR][0]     = WIZARD_ENEMY_GHOST_FAR;

    // -- LLIBRE --
    probability_table_wizards[BOOK] = std::vector<std::vector<Priority>>(dist_ranges, std::vector<Priority>(1, NOT_DEFINED));
    probability_table_wizards[BOOK][iCLOSE][0]     = WIZARD_BOOK_CLOSE;
    probability_table_wizards[BOOK][iMID][0]       = WIZARD_BOOK_MID;
    probability_table_wizards[BOOK][iFAR][0]       = WIZARD_BOOK_FAR;

    // -- VOLDEMORT --
    probability_table_wizards[VOLDEMORT] = std::vector<std::vector<Priority>>(dist_ranges, std::vector<Priority>(1, NOT_DEFINED));
    probability_table_wizards[VOLDEMORT][iCLOSE][0]     = WIZARD_VOLDEMORT_CLOSE;
    probability_table_wizards[VOLDEMORT][iMID][0]       = WIZARD_VOLDEMORT_MID;
    probability_table_wizards[VOLDEMORT][iFAR][0]       = WIZARD_VOLDEMORT_FAR;

    // -- EMPTY --
    probability_table_wizards[EMPTY] = std::vector<std::vector<Priority>>(dist_ranges, std::vector<Priority>(prob_ranges, NOT_DEFINED));
    probability_table_wizards[EMPTY][iCLOSE][0]     = WIZARD_EMPTY_CLOSE;
    probability_table_wizards[EMPTY][iMID][0]       = WIZARD_EMPTY_MID;
    probability_table_wizards[EMPTY][iFAR][0]       = WIZARD_EMPTY_FAR;
  }

  // Inicialitzar la PT relacionada amb prioritats de Ghosts.
  void initPTGhost() {
    // probability_table[TYPE][<=dist][<=winrate] = Priority
    int dist_ranges = 3;  // CLOSE | MID | FAR
    int prob_ranges = 3;  // LOW | MEDIUM  | HIGH

    // -- MAG ENEMIG --
    probability_table_ghost[ENEMY_WIZARD] = std::vector<std::vector<Priority>>(dist_ranges, std::vector<Priority>(1, NOT_DEFINED));
    probability_table_ghost[ENEMY_WIZARD][iCLOSE][0]  = GHOST_ENEMY_WIZARD_CLOSE;
    probability_table_ghost[ENEMY_WIZARD][iMID][0]    = GHOST_ENEMY_WIZARD_MID;
    probability_table_ghost[ENEMY_WIZARD][iFAR][0]    = GHOST_ENEMY_WIZARD_FAR;

    // -- LLIBRE --
    probability_table_ghost[BOOK] = std::vector<std::vector<Priority>>(dist_ranges, std::vector<Priority>(1, NOT_DEFINED));
    probability_table_ghost[BOOK][iCLOSE][0]  = GHOST_BOOK_CLOSE_LOW;
    probability_table_ghost[BOOK][iMID][0]    = GHOST_BOOK_MID;
    probability_table_ghost[BOOK][iFAR][0]    = GHOST_BOOK_FAR;

    // -- VOLDEMORT --
    probability_table_ghost[VOLDEMORT] = std::vector<std::vector<Priority>>(dist_ranges, std::vector<Priority>(1, NOT_DEFINED));
    probability_table_ghost[VOLDEMORT][iCLOSE][0]     = GHOST_VOLDEMORT_CLOSE;
    probability_table_ghost[VOLDEMORT][iMID][0]       = GHOST_VOLDEMORT_MID;
    probability_table_ghost[VOLDEMORT][iFAR][0]       = GHOST_VOLDEMORT_FAR;

    // -- EMPTY --
    probability_table_ghost[EMPTY] = std::vector<std::vector<Priority>>(dist_ranges, std::vector<Priority>(1, NOT_DEFINED));
    probability_table_ghost[EMPTY][iCLOSE][0]     = GHOST_EMPTY_CLOSE;
    probability_table_ghost[EMPTY][iMID][0]       = GHOST_EMPTY_MID;
    probability_table_ghost[EMPTY][iFAR][0]       = GHOST_EMPTY_FAR;
  }

  // Ficar en el set tots els id dels meus mags actuals i al fantasma.
  void getMyUnits() {
    smy_units.clear();
    VI vtemp = wizards(me());
    for (const auto& id : vtemp) smy_units.insert(id);
    int ghost_id = ghost(me());
    if (ghost_id != -1) smy_units.insert(ghost_id);
  }

  // Ficar en vector totes les forçes. Posicio iessima és la força del equip iessim.
  void getTeamsStr() {
    for (int i = 0; i<4; i++) vstr[i] = magic_strength(i);
  }

  // Retorna quin tipus d'element hi ha a aquesta posició.
  Content getCellContent(int x, int y) {
    Cell ctemp = cell(x, y);
    if (ctemp.type == Wall) return WALL;
    else if (ctemp.book) return BOOK;
    else if (ctemp.id != -1) {
        Unit u = unit(ctemp.id);
        if (u.player == me()) return ALLIED;
        else if (u.type == Wizard) return ENEMY_WIZARD;
        else if (u.type == Ghost) return ENEMY_GHOST;
    }
    else if (ctemp.owner != me()) return EMPTY;
    return OWNED;
  }
  
  // Ficar en el mapa tots els tipus d'elements que hi ha al mapa.
  void getBoardData() {
    for (int i = 0; i<SIZE; i++) {
      for (int j = 0; j<SIZE; j++) {
        content_board[i][j] = getCellContent(i,j);
        movement_board[i][j] = Movement();  // Resetejar moviments
      }
    } 
  }

  // Inicialitzar la informació actual sobre: Meves unitats vives, Força de cada Team, Mapa.
  void getAllData() {
    getMyUnits();
    getTeamsStr();
    getBoardData();
  }

  // Calcular probabilitat de guanyar de l'atacant
  double calcProbWizards(int attacker_player, int defender_player) {
    double N = magic_strength(attacker_player);
    double M = magic_strength(defender_player);

    // === BASE CASE
    if (N == 0 && M == 0) return 0.5; // Probabilitat 50%
    if (N >= 2 * M) return 1.0; // Atacant li treu el doble
    if (M >= 2 * N) return 0.0; // Defensor li treu el doble

    // Probabilitat que l'atacant guany per sorpresa
    double surprise_prob = 0.3;

    // Probabilitat que l'atacant guany sense sorpresa
    double attack_prob = N / (N + M);

    // Probabilitat total que l'atacant té de guanyar
    double total_attack_prob = surprise_prob + (1.0 - surprise_prob) * attack_prob;

    return total_attack_prob;
  }

  // Calcular Probabilitat donat un tipus, contingut, distància i winrate.
  Priority calcPriority(UnitType t, Content c, int d, double w) {
    // Convertir a rang de distancies
    int idist;
    if (d<=CLOSE) idist = iCLOSE;
    else if (d<=MID) idist = iMID;
    else idist = iFAR;

    // Convertir rang probabilitats
    int iprob;
    if (t == Wizard && probability_table_wizards[c][idist].size() == 1) iprob = 0;
    else if (t == Ghost && probability_table_ghost[c][idist].size() == 1) iprob = 0;
    else {
      if (w*100 <= LOW) iprob = iLOW;
      else if (w*100 <= MEDIUM) iprob = iMEDIUM;
      else iprob = iHIGH;
    }

    // Retornar Valor
    if (t == Wizard) return probability_table_wizards[c][idist][iprob];
    else return probability_table_ghost[c][idist][iprob];
  }

  // BFS per trobar elements pròxims al Mag. Hi ha limit de búsqueda.
  void wizard_bfs(const Unit& u, PQM& pqmovements, const int& max_bfs_depth) {
    VVB seen = VVB(SIZE, VB(SIZE, false));
    queue<pair<pair<Pos, int>, Dir>> q;  // <<"Pos", "Dist fins Pos">, "Dir inicial">
    seen[u.pos.i][u.pos.j] = true;
    
    for (auto dir : dirs_wizard) {
      Pos new_pos = u.pos + dir;

      if (pos_ok(new_pos) && content_board[new_pos.i][new_pos.j] != WALL) {
        seen[new_pos.i][new_pos.j] = true;
        q.push({{new_pos,1}, dir});
      }
    }
    

    while (!q.empty()) {
      pair<Pos, int> act = q.front().first;
      Dir dir_ini = q.front().second;
      q.pop();

      if (act.second>=max_bfs_depth) continue;  // He superat el maxim de búsqueda

      random_shuffle(dirs_all.begin(), dirs_all.end());  // Aleatori perquè si.
      
      // Buscar en totes les direccions (Donat que no podem anar diagonal)
      for (const auto& dir : dirs_all) {
        Pos new_pos = act.first + dir;
        
        // === BASE CASE
        if (!pos_ok(new_pos)) continue;  // No és vàlida
        else if (seen[new_pos.i][new_pos.j]) continue;  // Ja l'he vista
        seen[new_pos.i][new_pos.j] = true;
        if (content_board[new_pos.i][new_pos.j] == WALL) continue;  // És una paret

        // === GENERAL CASE
        int dist = act.second + 1;
        Priority priority = NOT_DEFINED;

        // -- MAG ALIAT PER SALVAR --
        if (content_board[new_pos.i][new_pos.j] == ALLIED) {
          Cell temp_cell = cell(new_pos.i, new_pos.j);
          Unit temp_allied_unit = unit(temp_cell.id);

          if (!temp_allied_unit.is_in_conversion_process()) continue;  // No s'està convertint
          
          priority = calcPriority(Wizard, ALLIED, dist, 1);
        }
        // -- MAG ENEMIG --
        else if (content_board[new_pos.i][new_pos.j] == ENEMY_WIZARD) {
          Cell temp_cell = cell(new_pos.i, new_pos.j);
          Unit temp_enemy_unit = unit(temp_cell.id);

          if (temp_enemy_unit.is_in_conversion_process()) continue;  // Si s'esta transformant no m'importa
          
          double probability = calcProbWizards(me(), temp_enemy_unit.player);  // Jo ataco
          priority = calcPriority(Wizard, ENEMY_WIZARD, dist, probability);
        }
        // -- FANTASMA ENEMIG -- 
        else if (content_board[new_pos.i][new_pos.j] == ENEMY_GHOST) {
          priority = calcPriority(Wizard, ENEMY_GHOST, dist, 1);
        }
        // -- LLIBRE --
        else if (content_board[new_pos.i][new_pos.j] == BOOK) {
          priority = calcPriority(Wizard, BOOK, dist, 1);
        }
        // -- VOLDEMORT -- 
        else if (content_board[new_pos.i][new_pos.j] == VOLDEMORT) {
          priority = calcPriority(Wizard, VOLDEMORT, dist, 0);
        }
        // -- EMPTY --
        else if (content_board[new_pos.i][new_pos.j] == EMPTY) {
          priority = calcPriority(Wizard, EMPTY, dist, 1);
        }

        // Crear el moviment i guardar-lo
        Movement temp;
        temp.id = u.id;
        temp.prio = priority;
        temp.d = dir_ini;
        temp.p = new_pos;
        pqmovements.push(temp);

        q.push({{new_pos, dist}, dir_ini});
      }
    }
  }

  // BFS per trobar elements pròxims al Fantasma. Hi ha limit de búsqueda.
  void ghost_bfs(const Unit& u, PQM& pqmovements, const int& max_bfs_depth) {
    VVB seen = VVB(SIZE, VB(SIZE, false));
    queue<pair<Pos, int>> q;
    seen[u.pos.i][u.pos.j] = true;
    q.push({u.pos,0});

    while (!q.empty()) {
      pair<Pos,int> act = q.front();
      q.pop();

      if (act.second>=max_bfs_depth) continue;

      random_shuffle(dirs_all.begin(), dirs_all.end());  // Aleatori perquè si.
      for (const auto& dir : dirs_all) {
        Pos new_pos = act.first + dir;
        
        // === BASE CASE
        if (!pos_ok(new_pos)) continue;  // No és vàlida
        else if (seen[new_pos.i][new_pos.j]) continue;  // Ja l'he vista
        seen[new_pos.i][new_pos.j] = true;
        if (content_board[new_pos.i][new_pos.j] == WALL) continue;  // És una paret

        // === GENERAL CASE
        int dist = act.second + 1;
        Priority priority = NOT_DEFINED;

        // -- MAG ENEMIG --
        if (content_board[new_pos.i][new_pos.j] == ENEMY_WIZARD) {
          priority = calcPriority(Ghost, ENEMY_WIZARD, dist, 0);
        }
        // -- LLIBRE --
        else if (content_board[new_pos.i][new_pos.j] == BOOK) {
          priority = calcPriority(Ghost, BOOK, dist, 1);
        }
        // -- VOLDEMORT -- 
        else if (content_board[new_pos.i][new_pos.j] == VOLDEMORT) {
          priority = calcPriority(Ghost, VOLDEMORT, dist, 0);
        }
        // -- EMPTY --
        else if (content_board[new_pos.i][new_pos.j] == EMPTY) {
          priority = calcPriority(Ghost, EMPTY, dist, 1);
        }

        // Crear el moviment i guardar-lo
        Movement temp;
        temp.id = u.id;
        temp.prio = priority;
        temp.d = dir;
        temp.p = new_pos;
        pqmovements.push(temp);

        q.push({new_pos, dist});
      }
    }
  }

  // Troba el millor moviment a l'unitat passada com a paràmetre.
  Movement findBestMove(int id) {
    int max_bfs_depth = 20;
    if (max_bfs_depth>num_rounds()-round()) max_bfs_depth = num_rounds()-round();
    Unit unit_act = unit(id);
    PQM possible_movements;
    if (unit_act.type == Wizard) wizard_bfs(unit_act, possible_movements, max_bfs_depth);
    else ghost_bfs(unit_act, possible_movements, max_bfs_depth);
    

    // Comprovar que el millor moviment no hagi sigut seleccionat previament
    // FI: Mirar-ho com equip.
    while (!possible_movements.empty()) {
      Movement best_move = possible_movements.top();
      possible_movements.pop();
      if (movement_board[best_move.p.i][best_move.p.j].id == -1) {
        movement_board[best_move.p.i][best_move.p.j] = best_move;
        return best_move;
      }
    }

    // Això és moooolt dificil que passi
    return Movement();
  }

  // Per a cada unitat viva, assigna el millor moviment.
  void getAllMovements() {
    while (!smy_units.empty()) {
      int id_actual = *smy_units.begin();
      smy_units.erase(id_actual);
      Movement best_movement = findBestMove(id_actual);
      smovements.insert(best_movement);
    }
  }

  // Agafa el set de moviments, els ordena per prioritat (PQ) i envia les comandes.
  void executeAllMovements() {
    PQM temp;
    for (const auto& it : smovements) temp.push(it);
    while (!temp.empty()) {
      Movement act = temp.top();
      temp.pop();
      move(act.id, act.d);
    }
    smovements.clear();  // Una vegada tots executats, netejem la llista
  }

  void makeAllMovements() {
    getAllMovements();
    executeAllMovements();
  }

  virtual void play () {
    // JUTGE OPTIONS
    double st = status(me());
    if (st >= 0.9) return;

    // MY OPTIONS
    if (!initialized) {
      initialized = true;
      initPT();
    }
    getAllData();
    makeAllMovements();
  }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);