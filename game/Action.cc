//////// STUDENTS DO NOT NEED TO READ BELOW THIS LINE ////////

#include "Action.hh"

Action::Action (istream& is) {
  u.clear();
  v.clear();

  // warning: all read operations must be checked for SecGame.
  int l;
  is >> l;
  for (int k = 0; k < l; ++k) {
    int i;
    is >> i; // id
    char c;
    if (not(is >> c)) {
      //cerr << "warning: only partially read command for unit " << i << endl;
      return;
    }

    if (char2CommandType(c) == Move) {
      char d;
      if (is >> d) {
	u.insert(i);
	v.push_back(Command(i, char2CommandType(c), char2Dir(d),{}));
      }
      else {
	//cerr << "warning: only partially read command for unit " << i << endl;
	return;
      }
    }
    else {
      int n;
      if (char2CommandType(c) != Spell or not(is >> n)) {
	//cerr << "warning: only partially read command for unit " << i << endl;
	return;
      }

      vector<int> s; int x;     
      for (int k = 0; k < n; ++k) {
	if (not(is >> x)) {
	  //cerr << "warning: only partially read command for unit " << i << endl;
	  return;
	}
	s.push_back(x);
      }

      u.insert(i);
      v.push_back(Command(i, char2CommandType(c), -1, s));
    }
  }
}

void Action::print (const vector<Command>& commands, ostream& os) {
  os << commands.size() << endl;
  for (const Command& com : commands) {
    if (com.c_type == Move) {
      os <<                  com.id       << '\t'
	 << CommandType2char(com.c_type)  << '\t'
	 <<         Dir2char(com.dir   )  << '\t'
	 << endl;
    }
    else {
      os <<                       com.id << '\t'
	 << CommandType2char(com.c_type) << '\t'
	 <<               com.sol.size() << '\t'; // First write size so that it can be easily read
      for (int x : com.sol) os << " " << x;
      os << endl;	
    }
  }
}
