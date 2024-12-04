// Compilar con: g++ -std=c++17 -pthread -o tester tester.cpp

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cstdio>
#include <sstream>
#include <string>
#include <filesystem>
#include <condition_variable>
#include <fstream>
#include <utility>
#include <algorithm>
#include <unordered_map>
#include <iomanip>

namespace fs = std::filesystem;

const int MAX_CONCURRENT_THREADS = 8;
const std::string GENERAL_TEMP_DIR = "temp_results";

struct GameResult {
    int seed;
    std::unordered_map<std::string, long> scores;
    std::string winner;
};

// Función corregida para extraer las puntuaciones y el ganador
GameResult run_game_and_get_scores(int seed, const std::vector<std::string>& players) {
    // Nombre del archivo de resultados para esta semilla
    std::string res_file_name = "default.res_seed_" + std::to_string(seed);
    fs::path res_file_path = fs::path(GENERAL_TEMP_DIR) / res_file_name;

    // Construir el comando con los jugadores
    std::string command = "./Game";
    for (const auto& player : players) {
        command += " " + player;
    }
    command += " -s " + std::to_string(seed) + " < default.cnf > " + res_file_path.string();

    // Ejecutar el comando
    int returnCode = system(command.c_str());
    if (returnCode != 0) {
        std::cerr << "El comando para la seed " << seed << " terminó con código " << returnCode << std::endl;
        return {seed, {}, ""};
    }

    // Extraer las puntuaciones del archivo de resultados
    std::ifstream infile(res_file_path);
    if (!infile.is_open()) {
        std::cerr << "No se pudo abrir " << res_file_path << std::endl;
        return {seed, {}, ""};
    }

    std::string line;
    GameResult result;
    result.seed = seed;
    std::vector<long> scores;

    // Buscar "round 200" y luego extraer las puntuaciones
    while (std::getline(infile, line)) {
        if (line.find("round 200") != std::string::npos) {
            // Leer dos líneas más
            std::getline(infile, line); // Línea intermedia (posiblemente en blanco)
            if (std::getline(infile, line)) { // Línea de puntuaciones
                std::istringstream iss(line);
                std::string label;
                iss >> label; // Leer "score"

                // Leer las puntuaciones
                long score;
                while (iss >> score) {
                    scores.push_back(score);
                }

                break;
            }
        }
    }
    infile.close();

    // Verificar que se hayan leído suficientes puntuaciones
    if (scores.size() < players.size()) {
        std::cerr << "Número insuficiente de puntuaciones en la seed " << seed << std::endl;
        return {seed, {}, ""};
    }

    // Asignar las puntuaciones a los jugadores en orden
    for (size_t i = 0; i < players.size(); ++i) {
        result.scores[players[i]] = scores[i];
    }

    // Determinar el ganador (jugador con la puntuación más alta)
    size_t winner_index = 0;
    long max_score = scores[0];
    for (size_t i = 1; i < scores.size(); ++i) {
        if (scores[i] > max_score) {
            max_score = scores[i];
            winner_index = i;
        }
    }
    result.winner = players[winner_index];

    return result;
}

int main(int argc, char* argv[]) {
    // Aceptar hasta 4 jugadores como argumentos
    std::vector<std::string> players;
    for (int i = 1; i < argc && i <= 4; ++i) {
        players.push_back(argv[i]);
    }
    // Completar con "Demo" si hay menos de 4 jugadores
    while (players.size() < 4) {
        players.push_back("Demo");
    }

    // Ajustar el rango de semillas según tus necesidades
    const int start_seed = 0;
    const int end_seed = 100; // Cambiado de 1 a 50 para mayor cobertura
    const int total_seeds = end_seed - start_seed + 1;

    // Vector para almacenar resultados de las partidas
    std::vector<GameResult> game_results;
    game_results.reserve(total_seeds);

    // Crear la carpeta general para los resultados
    fs::path general_dir = fs::path(GENERAL_TEMP_DIR);
    try {
        if (!fs::exists(general_dir)) {
            fs::create_directory(general_dir);
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error al crear el directorio general: " << e.what() << std::endl;
        return 1;
    }

    // Variables para manejar la concurrencia
    std::mutex mtx;
    std::condition_variable cv;
    int active_threads = 0;

    // Función lambda para ejecutar cada seed
    auto execute_seed = [&](int seed) -> void {
        GameResult result = run_game_and_get_scores(seed, players);

        // Bloquear el mutex para actualizar los resultados y el contador de hilos
        {
            std::lock_guard<std::mutex> lock(mtx);
            game_results.push_back(result);
            active_threads--;
        }

        // Notificar a otros hilos que hay espacio disponible
        cv.notify_one();
    };

    // Vector para almacenar threads
    std::vector<std::thread> threads;

    // Lanzar hilos para cada seed
    for (int seed = start_seed; seed <= end_seed; ++seed) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]() { return active_threads < MAX_CONCURRENT_THREADS; });

        // Incrementar el contador de hilos activos
        active_threads++;

        // Lanzar un hilo y almacenarlo en el vector
        threads.emplace_back(std::thread([&execute_seed, seed]() { execute_seed(seed); }));
    }

    // Unir todos los hilos
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

    // Mapas para almacenar estadísticas
    std::unordered_map<std::string, long> total_scores;
    std::unordered_map<std::string, int> win_counts;
    std::vector<int> failed_seeds;
    int worst_game_seed = -1;
    long worst_game_total_score = 0;
    bool first_valid_game = true;

    // Procesar los resultados
    for (const auto& result : game_results) {
        if (result.scores.empty()) {
            failed_seeds.push_back(result.seed);
            continue;
        }
        // Actualizar puntuaciones totales y conteo de victorias
        for (const auto& [player_name, score] : result.scores) {
            total_scores[player_name] += score;
        }
        if (!result.winner.empty()) {
            win_counts[result.winner]++;
        }

        // Calcular la puntuación total de la partida
        long total_score = 0;
        for (const auto& score_pair : result.scores) {
            total_score += score_pair.second;
        }

        // Determinar si esta es la peor partida
        if (first_valid_game || total_score < worst_game_total_score) {
            worst_game_total_score = total_score;
            worst_game_seed = result.seed;
            first_valid_game = false;
        }
    }

    // Calcular promedios y winrates
    std::unordered_map<std::string, double> avg_scores;
    std::unordered_map<std::string, double> winrates;
    int successful_seeds = total_seeds - failed_seeds.size();
    for (const auto& player : players) {
        if (successful_seeds > 0) {
            avg_scores[player] = static_cast<double>(total_scores[player]) / successful_seeds;
            winrates[player] = static_cast<double>(win_counts[player]) / successful_seeds * 100.0;
        } else {
            avg_scores[player] = 0.0;
            winrates[player] = 0.0;
        }
    }

    // Mostrar resultados en forma de tabla
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\nResultados:\n";
    std::cout << "Jugador\t\tAVG Score\tWinrate (%)\n";
    for (const auto& player : players) {
        std::cout << player << "\t\t" << avg_scores[player] << "\t\t" << winrates[player] << "%\n";
    }

    // Mostrar información de la peor partida
    if (worst_game_seed != -1) {
        std::cout << "\nPeor partida en seed " << worst_game_seed << " con puntuación total de " << worst_game_total_score << ".\n";
    } else {
        std::cout << "No se encontraron partidas válidas para determinar la peor partida.\n";
    }

    // Verificar e imprimir las semillas fallidas
    if (!failed_seeds.empty()) {
        std::cout << "\nSemillas que fallaron (" << failed_seeds.size() << "): ";
        for (size_t i = 0; i < failed_seeds.size(); ++i) {
            std::cout << failed_seeds[i];
            if (i != failed_seeds.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
    } else {
        std::cout << "\nTodas las semillas se ejecutaron correctamente." << std::endl;
    }

    // Eliminar la carpeta general y sus contenidos
    try {
        fs::remove_all(general_dir);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error al eliminar el directorio general: " << e.what() << std::endl;
    }

    return 0;
}