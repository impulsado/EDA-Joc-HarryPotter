#include <iostream>
#include <vector>
 
using namespace std;
 
typedef vector<int> VI;
 
bool solved = false;
 
// Añade un vector para contar los elementos en cada grupo
void solve_spell_rec(const VI& ingredients, VI& solucio, VI& suma_grups, VI& count_grups, int i, const int& max_sum) {
    int n = ingredients.size();

    // === CASO BASE
    if (i == n) {
        bool all_groups_valid = true;
        for(int j = 0; j < suma_grups.size() && all_groups_valid; j++) {
            if(suma_grups[j] != max_sum || count_grups[j] != 3) all_groups_valid = false;
        }
        if(all_groups_valid) solved = true;
    }

    if (solved) return;

    // === CASO GENERAL
    for(int j = 0; j < suma_grups.size(); j++) {
        if(!solved && suma_grups[j] + ingredients[i] <= max_sum && count_grups[j] < 3) {
            suma_grups[j] += ingredients[i];
            count_grups[j] += 1;
            solucio[i] = j;
            solve_spell_rec(ingredients, solucio, suma_grups, count_grups, i+1, max_sum);
            suma_grups[j] -= ingredients[i];
            count_grups[j] -= 1;
        }
    }

    return;
}
 
VI solve_spell(const VI& ingredients) {
    int i = 0;
    int n = ingredients.size();
    int suma = 0;
    for (int j = 0; j < n; j++) suma += ingredients[j];
    VI solucio(n);
    int num_grups = 5;  // Debería ser 5 grupos
    VI suma_grups(num_grups, 0);
    VI count_grups(num_grups, 0); // Inicializa el contador de elementos por grupo
    solve(ingredients, solucio, suma_grups, count_grups, i, suma / num_grups);
    return solucio;
}
 
int main(void) {
    int n;  // Tot i que ens asseguren que sempre seran 15
    cin >> n;
    VI ingredients(n);
    for (int i = 0; i<n; i++) cin >> ingredients[i];
    solve_spell(ingredients);
}