#include "../include/UserManager.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <limits>
#include <cstdlib>
#include <cctype>

#include "../include/Utils.h"

void crear_usuario(const string& nombreArchivo) {

    ifstream archivoLectura(nombreArchivo);

    if (archivoLectura.good()) {

        cout << "El usuario ya existe" << endl;

        archivoLectura.close();

        return;
    }

    archivoLectura.close();

    ofstream archivoEscritura(nombreArchivo);

    if (archivoEscritura.is_open()) {

        archivoEscritura.close();

        cout << "Archivo creado correctamente." << endl;

    } else {

        cout << "Error al crear el archivo." << endl;
    }
}

void buscarYDarLike(SearchEngine& engine, const string& usuario) {

    string query;

    while (true) {

        cout << "\n====================================\n";
        cout << "Buscar pelicula ('exit' para salir)\n";
        cout << "====================================\n";
        cout << "Busqueda: ";

        getline(cin, query);

        // evitar búsquedas vacías
        if (query.empty()) {

            cout << "\nIngrese una busqueda valida.\n";

            continue;
        }

        // salir
        if (query == "exit")
            break;

        vector<int> res = engine.search(query);

        vector<int> ids =
            engine.rankResults(res, query);

        if (ids.empty()) {

            cout << "\nNo hay resultados.\n";

            continue;
        }

        int index = 0;

        while (index < ids.size()) {

            cout << "\n============= RESULTADOS =============\n";

            vector<int> visibles;

            for (
                int i = 0;
                i < 5 && index < ids.size();
                i++, index++
            ) {

                cout
                    << i + 1
                    << ". "
                    << engine.getMovie(ids[index]).title
                    << endl;

                visibles.push_back(ids[index]);
            }

            cout << "======================================\n";
            cout << "[1-5] Ver pelicula\n";
            cout << "[9]   Ver mas\n";
            cout << "[0]   Nueva busqueda\n";
            cout << "======================================\n";
            cout << "Seleccione: ";

            string entrada;

            getline(cin, entrada);

            if (entrada.empty()) {

                cout << "\nOpcion invalida.\n";
                continue;
            }

            bool esNumero = true;

            for (char c : entrada) {

                if (!isdigit(c)) {

                    esNumero = false;
                    break;
                }
            }

            if (!esNumero) {

                cout << "\nOpcion invalida.\n";
                break;
            }

            int op = atoi(entrada.c_str());

            // nueva búsqueda
            if (op == 0)
                break;

            // siguientes 5
            if (op == 9)
                continue;

            // película válida
            if (
                op >= 1 &&
                op <= visibles.size()
            ) {

                int elegido = visibles[op - 1];

                cout << "\n====================================\n";
                cout << engine.getMovie(elegido).title
                     << endl;
                cout << "====================================\n";

                cout << "\nSINOPSIS:\n\n";

                cout
                    << normalize(engine.getMovie(elegido).plot)
                    << endl;

                int accion;

                while (true) {

                    cout << "\n====================================\n";
                    cout << "[1] Dar like\n";
                    cout << "[2] Ver despues\n";
                    cout << "[0] Volver\n";
                    cout << "====================================\n";
                    cout << "Seleccione: ";

                    cin >> accion;

                    if (cin.fail()) {

                        cin.clear();

                        cin.ignore(
                            numeric_limits<streamsize>::max(),
                            '\n'
                        );

                        cout << "\nOpcion invalida.\n";

                        continue;
                    }

                    cin.ignore(
                        numeric_limits<streamsize>::max(),
                        '\n'
                    );

                    if (accion == 0)
                        break;

                    if (accion == 1) {

                        dar_me_gusta(
                            usuario,
                            "[like] | " +
                            engine.getMovie(elegido).genre
                        );

                        break;
                    }

                    if (accion == 2) {

                        dar_me_gusta(
                            usuario,
                            "[ver luego] " +
                            engine.getMovie(elegido).title
                        );

                        break;
                    }

                    cout << "\nOpcion invalida.\n";
                }
            }

            else {

                cout << "\nOpcion invalida.\n";
            }
        }
    }
}

bool existe_usuario(const string& nombre) {

    ifstream file(nombre + ".txt");

    return file.good();
}

void recomendaciones_nuevo(SearchEngine& engine) {

    cout << "---------------------" << endl;

    cout << "Recomendaciones de peliculas:\n";

    for (int i = 0; i < 5; i++) {

        int m = rand() % 34886;

        cout << m << ". "
             << engine.getMovie(m).title
             << endl;
    }

    cout << "-------------------" << endl;
}

string obtener_ultimo_like(const string& usuario) {
    ifstream file(usuario + ".txt");
    string linea, ultima;

    while (getline(file, linea)) {
        if (linea.find("[like]") != string::npos) {
            ultima = linea;
        }
    }

    file.close();
    return ultima;
}

string extraer_genero(const string& linea) {
    size_t pos = linea.find("|");
    if (pos != string::npos) {
        return linea.substr(pos + 1); // lo que está después del |
    }
    return "";
}

void recomendar_por_ultimo_like(
    SearchEngine& engine,
    const string& usuario
) {

    string ultima = obtener_ultimo_like(usuario);

    if (ultima.empty()) {

        cout << "No hay likes previos.\n";
        return;
    }

    string genero = extraer_genero(ultima);

    cout << "\nBasado en tu ultimo like ("
         << genero
         << "):\n";

    vector<int> ids = engine.search(genero);

    if (ids.empty()) {

        cout << "No se encontraron recomendaciones.\n";
        return;
    }

    int limite = min(5, (int)ids.size());

    for (int i = 0; i < limite; i++) {

        cout << "- "
             << engine.getMovie(ids[i]).title
             << endl;
    }
}

void dar_me_gusta(const string & nombre_usuario, const string& linea) {
    string archivo = nombre_usuario + ".txt";
    ofstream file(archivo, ios::app);
    if (file.is_open()) {
        file << linea<< endl;
        file.close();
        cout << "Pelicula guardada en tus likes.\n";
    } else {
        cout << "Error al guardar.\n";
    }
}

void mostrar_ver_despues(const string& nombre_usuario) {
    string archivo = nombre_usuario + ".txt";
    ifstream file(archivo);
    if (!file.is_open()) {
        cout << "No se pudo abrir el archivo.\n";
        return;
    }
    string linea;
    cout << "\n--- PELICULAS PARA VER DESPUES ---\n";
    while (getline(file, linea)) {
        if (linea.find("[ver luego]") != string::npos) {
            cout << linea << endl;
        }
    }
    file.close();
}