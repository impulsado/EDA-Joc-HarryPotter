#include <iostream>
#include <vector>
 
using namespace std;
 
typedef vector<int> VI;
 
bool solved = false;
 
void solve(const VI& ingredients, VI& solucio, VI& suma_grups, int i, int& sum, const int& max_sum) {
    int n = ingredients.size();
 
    // === BASE CASE
    if (i == n) solved = true;
    if (solved) return;
 
    // === GENERAL CASE
    // 1. Determinar qui agafa el ingredient i-essim
    // 1.1. El primer grup l'agafa
    if (!solved && suma_grups[0] < max_sum && (suma_grups[0]+ingredients[i])<=max_sum) {
        suma_grups[0] += ingredients[i];
        sum -= ingredients[i];
        solucio[i] = 0;
        solve(ingredients, solucio, suma_grups, i+1, sum, max_sum);
        suma_grups[0] -= ingredients[i];
        sum += ingredients[i];
    }
    // 1.2. El segon grup l'agafa
    if (!solved && suma_grups[1] < max_sum && (suma_grups[1]+ingredients[i])<=max_sum) {
        suma_grups[1] += ingredients[i];
        sum -= ingredients[i];
        solucio[i] = 1;
        solve(ingredients, solucio, suma_grups, i+1, sum, max_sum);
        suma_grups[1] -= ingredients[i];
        sum += ingredients[i];
    }
    // 1.3. El tercer grup l'agafa (Per descarte)
    // Aqui no faig comprovacions perque m'asseguren que hi ha una solució
    if (!solved) {
        suma_grups[2] += ingredients[i];
        sum -= ingredients[i];
        solucio[i] = 2;
        solve(ingredients, solucio, suma_grups, i+1, sum, max_sum);
        suma_grups[2] -= ingredients[i];
        sum += ingredients[i];
    }

    return;
}
 
void solve_spell(const VI& ingredients) {
    int i = 0;
    int n = ingredients.size();
    int num_grups = 3;  // 3 grups sempre
    VI solucio(n);
    VI suma_grups(num_grups);
    int suma = 0;
    for (int j = 0; j<n; j++) suma += ingredients[j];
    solve(ingredients, solucio, suma_grups, i, suma, suma/num_grups);
    for (int j = 0; j<n; j++) cout << solucio[j];
    cout << endl;
}
 
int main(void) {
    int n;  // Tot i que ens asseguren que sempre seran 15
    cin >> n;
    VI ingredients(n);
    for (int i = 0; i<n; i++) cin >> ingredients[i];
    solve_spell(ingredients);
}