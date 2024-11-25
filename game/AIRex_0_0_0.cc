#include "Player.hh"

#define PLAYER_NAME AIRex_0_0_0
#define SIZE 60

/**
PRIORITIES:
0. Si hi ha algún enemig que trec el doble --> ATACAR
1. Si hi ha algún meu perill perquè doble --> FUGIR
2. Fugir de Voldemort
3. Salvar mag en procés de convertir-se
4. Atacar enemig
5. Buscar Llibre
6. Conquerir terreny no meu
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
    ATTACK_STRONG_ENEMY = 0,
    ATTACK_STRONG_ENEMY = 1,
    ESCAPE_FROM_VOLDEMORT = 2,
    SAVE_FRIENDLY_WIZARD = 3,
    ATTACK_ENEMY = 4,
    COLLECT_BOOK = 5,
    EXPLORE = 6,
    NOT_DEFINED = INT32_MAX,
  };

  enum Content {
    WALL,     // Paret
    OWNED,    // No té res però és meva
    EMPTY,    // No és meva. Pot ser del enemic o buida
    WIZARD,   // Mag enemig
    GHOST,    // Fantasma enemig
    BOOK,     // Llibre
    VOLDEMORT,
  };

  const vector<Dir> dirs_wizard = {Up,Down,Left,Right};
  const vector<Dir> dirs_ghost = {Down,DR,Right,RU,Up,UL,Left,LD};

  struct Movement {
    int id = -1;  // id de l'unitat que realitza el moviment
    Priority prio = NOT_DEFINED;  // Prioritat de l'unitat a fer aquest moviment
    Dir d;  // Direcció que s'ha de moure l'unitat (No es fa ús en tauler de moviments)
    Pos p;
  };

  struct CompMovementPriority {
    // true: m2,m1
    // false: m1,m2
    // m1 abans que m2 si m1 té menys prioritat.
    bool operator() (const Movement& m1, const Movement& m2) {
      if (m1.prio<=m2.prio) return false;
      else return true;
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

  VI vstr = VI(4);  // Força del i-essim equip
  SI smy_units;  // Set amb les meves unitats vives
  SM smovements;  // Set amb els moviments de les meves unitats
  VVM movement_board = VVM(SIZE, VM(SIZE, Movement()));
  VVC content_board = VVC(SIZE, VC(SIZE));  // Tauler amb els objectes


  /**
   * Play method, invoked once per each round.
   */
  // Ficar en el set tots els id dels meus mags actuals i al fantasma.
  void getMyUnits() {
    smy_units.clear();
    VI vtemp = wizards(me());
    for (const auto& id : vtemp) smy_units.insert(id);
    smy_units.insert(ghost(me()));
  }

  // Ficar en vector totes les forçes. Posicio iessima és la força del equip iessim.
  void getTeamsStr() {
    for (int i = 0; i<4; i++) vstr[i] = magic_strength(i);
  }

  // Retorna quin tipus d'element hi ha a aquesta posició.
  Content getCellContent(int x, int y) {
    Cell ctemp = cell(x,y);
    if (ctemp.type == Wall) return WALL;
    else if (ctemp.book) return BOOK;
    else if (ctemp.owner == -1) return EMPTY;
    else if (ctemp.owner == me()) return OWNED;
    else if (ctemp.id == -1) return EMPTY;  // És del enemic però no té res
    else if (unit(ctemp.id).type == Wizard) return WIZARD;
    else if (unit(ctemp.id).type == Ghost) return GHOST;
    else {
      Pos ptemp = pos_voldemort();
      if (ptemp.i == x && ptemp.j == y) return VOLDEMORT;
    }
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
/*
  int calcPriority(int prioridadBase, int distancia, int rondasRestantes) {
      return prioridadBase + factorDistancia * distancia + factorRondas * rondasRestantes;
  }
*/

  // BFS per trobar elements pròxims a una Unitat. Hi ha limit de búsqueda.
  void general_bfs(const Unit& u, PQM& pqmovements, const int& max_bfs_depth) {
    VVB seen = VVB(SIZE, VB(SIZE, false));
    int depth = 0;

    if (u.type == Ghost) {  // (8 Direccions)
      QPOS q;
      q.push(u.pos);

      while (!q.empty() && depth<max_bfs_depth) {
        Pos act = q.front();
        seen[u.pos.i][u.pos.j] = true;
        q.pop();

        random_shuffle(dirs_ghost.begin(), dirs_ghost.end());  // Aleatori perquè si.
        for (const auto& dir : dirs_ghost) {
          Pos new_pos = act + dir;
          
          // === BASE CASE
          if (!pos_ok(new_pos)) continue;  // No és vàlida
          if (!seen[new_pos.i][new_pos.j]) continue;  // Ja l'he vista
          if (content_board[new_pos.i][new_pos.j] == WALL) continue;  // Es una paret

          // === GENERAL CASE

        }
        depth++;
      }
    }
    else {  // Wizard (4 Direccions)

    }
  }

  // Troba el millor moviment a l'unitat passada com a paràmetre.
  Movement findBestMove(int id) {
    int max_bfs_depth = 50;
    if (max_bfs_depth>num_rounds()-round()) max_bfs_depth = num_rounds()-round();
    Unit unit_act = unit(id);
    PQM possible_movements;
    general_bfs(unit_act, possible_movements, max_bfs_depth);

  }

  // Per a cada unitat viva, assigna el millor moviment.
  void getAllMovements() {
    while (smy_units.empty()) {
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
    getAllData();
    makeAllMovements();
  }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
