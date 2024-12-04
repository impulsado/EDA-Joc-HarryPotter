// g++ -std=c++17 -pthread -o tester tester.cpp

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
#include <utility> // Para std::pair
#include <algorithm> // Para std::min_element

namespace fs = std::filesystem;

const int MAX_CONCURRENT_THREADS = 8;
const std::string GENERAL_TEMP_DIR = "temp_results";

long extract_score(const fs::path& res_file_path) {
    std::ifstream infile(res_file_path);
    if (!infile.is_open()) {
        std::cerr << "No se pudo abrir " << res_file_path << std::endl;
        return 0;
    }

    std::string line;
    bool round_found = false;
    long score = 0;

    while (std::getline(infile, line)) {
        if (line.find("round 200") != std::string::npos) {
            round_found = true;
            // Leer las siguientes dos líneas
            std::getline(infile, line); // Línea intermedia
            if (std::getline(infile, line)) { // Línea de score
                std::istringstream iss(line);
                std::string label;
                iss >> label; // Leer "score"
                if (label.find("score") != std::string::npos) {
                    iss >> score;
                }
            }
            break;
        }
    }

    infile.close();

    if (!round_found) {
        std::cerr << "No se encontró la ronda 200 en " << res_file_path << std::endl;
    }

    return score;
}

long run_game_and_get_score(int seed, const std::string& player_name) {
    // Nombre del archivo de resultados para esta semilla
    std::string res_file_name = "default.res_seed_" + std::to_string(seed);
    fs::path res_file_path = fs::path(GENERAL_TEMP_DIR) / res_file_name;

    // Comando para ejecutar el juego
    std::string command = "./Game " + player_name + " Demo Demo Demo -s " + std::to_string(seed) +
                          " < default.cnf > " + res_file_path.string();

    // Ejecutar el comando
    int returnCode = system(command.c_str());
    if (returnCode != 0) {
        std::cerr << "El comando para la seed " << seed << " terminó con código " << returnCode << std::endl;
        return 0;
    }

    // Extraer la puntuación del archivo de resultados
    long score = extract_score(res_file_path);

    return score;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <NombreDelPersonaje>" << std::endl;
        return 1;
    }

    std::string player_name = argv[1];

    const int start_seed = 0;
    const int end_seed = 50;
    const int total_seeds = end_seed - start_seed + 1;

    // Vector para almacenar pares de semilla y puntuación
    std::vector<std::pair<int, long>> seed_scores;
    seed_scores.reserve(total_seeds);

    // Vector para almacenar semillas fallidas
    std::vector<int> failed_seeds;

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
        long score = run_game_and_get_score(seed, player_name);

        // Bloquear el mutex para actualizar las puntuaciones y el contador de hilos
        {
            std::lock_guard<std::mutex> lock(mtx);
            seed_scores.emplace_back(seed, score);
            if (score == 0) { // Condición para considerar la seed como fallida
                failed_seeds.push_back(seed);
            }
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

    // Verificar el contenido de seed_scores para depuración
    std::cout << "Total de semillas ejecutadas: " << seed_scores.size() << std::endl;
    for (const auto& pair : seed_scores) {
        std::cout << "Semilla " << pair.first << ": Puntuación " << pair.second << std::endl;
    }

    // Calcular la media
    long total_score = 0;
    for (const auto& pair : seed_scores) {
        total_score += pair.second;
    }
    double average = static_cast<double>(total_score) / seed_scores.size();

    // Encontrar la semilla con la puntuación mínima usando std::min_element, excluyendo fallidas si es necesario
    std::vector<std::pair<int, long>> valid_seed_scores;
    for (const auto& pair : seed_scores) {
        if (pair.second > 0) { // Considerar solo semillas con puntuación válida
            valid_seed_scores.emplace_back(pair);
        }
    }

    if (!valid_seed_scores.empty()) {
        auto min_pair = std::min_element(valid_seed_scores.begin(), valid_seed_scores.end(),
            [](const std::pair<int, long>& a, const std::pair<int, long>& b) -> bool {
                return a.second < b.second;
            });
        std::cout << "Semilla con la menor puntuación: " << min_pair->first
                  << " (Puntuación: " << min_pair->second << ")" << std::endl;
    } else {
        std::cout << "No se encontraron semillas válidas para determinar la menor puntuación." << std::endl;
    }

    // Imprimir el resultado de la media
    std::cout << "AVG: " << average << std::endl;

    // Verificar e imprimir las semillas fallidas
    if (!failed_seeds.empty()) {
        std::cout << "Semillas que fallaron (" << failed_seeds.size() << "): ";
        for (size_t i = 0; i < failed_seeds.size(); ++i) {
            std::cout << failed_seeds[i];
            if (i != failed_seeds.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
    } else {
        std::cout << "Todas las semillas se ejecutaron correctamente." << std::endl;
    }

    // Eliminar la carpeta general y sus contenidos
    try {
        fs::remove_all(general_dir);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error al eliminar el directorio general: " << e.what() << std::endl;
    }

    return 0;
}