#include "Player.hh"

#define PLAYER_NAME AIRex_0_0_0
#define SIZE 60

/**
PRIORITIES:
0. Si hi ha algún enemig que trec el doble --> ATACAR
1. Si hi ha algún meu perill perquè doble --> FUGIR
2. Salvar mag en procés de convertir-se
3. Atacar enemig

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

  const vector<Dir> dirs = {Up,Down,Left,Right};
  enum cell_content {
    WALL,     // Paret
    OWNED,    // No té res però és meva
    EMPTY,    // No és meva. Pot ser del enemic o buida
    WIZARD,   // Mag enemig
    GHOST,    // Fantasma enemig
    BOOK,     // Llibre
    VOLDEMORT,
  };

  struct Movement {
    int priority;
    Dir d;
  };

  typedef set<int> SI;
  typedef set<Movement> SV;

  typedef vector<int> VI;
  
  typedef pair<int, int> P;
  typedef vector<P> VP;

  typedef vector<cell_content> VC;
  typedef vector<VC> VVC;

  VI vstr = VI(4);  // Força del i-essim equip
  SI smy_units;  // Set amb les meves unitats vives
  SV swizards_movments;  // Set amb les moviments dels meus mags
  VVC board = VVC(SIZE, VC(SIZE));  // Tauler amb els objectes

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
  cell_content getCellContent(int x, int y) {
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
        board[i][j] = getCellContent(i,j);
      }
    } 
  }

  // Inicialitzar la informació actual sobre: Meves unitats vives, Força de cada Team, Mapa
  void getAllData() {
    getMyUnits();
    getTeamsStr();
    getBoardData();
  }

  void makeAllMoves() {
    //moveWizards();
    //moveGhost();
  }

  virtual void play () {
    // JUTGE OPTIONS
    double st = status(me());
    if (st >= 0.9) return;

    // MY OPTIONS
    getAllData();
    makeAllMoves();
  }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
