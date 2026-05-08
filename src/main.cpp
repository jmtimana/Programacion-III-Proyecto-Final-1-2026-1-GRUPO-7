#include "../include/SearchEngine.h"
#include "../include/UserManager.h"

#include <iostream>
#include <limits>

using namespace std;

int main() {

    system("chcp 65001");

    srand(time(NULL));

    SearchEngine engine;

    cout << "====================================\n";
    cout << " Cargando base de datos...\n";
    cout << "====================================\n";

    engine.loadCSV(
        "../Data/wiki_movie_plots_deduped.csv"
    );

    string usuario_actual;

    int opcion;

    while (true) {

        cout << "\n====================================\n";
        cout << "     STREAMING PLATFORM P3\n";
        cout << "====================================\n";
        cout << "[1] Ingresar usuario\n";
        cout << "[2] Crear usuario\n";
        cout << "[3] Salir\n";
        cout << "====================================\n";
        cout << "Seleccione: ";

        cin >> opcion;

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

        // INGRESAR USUARIO
        if (opcion == 1) {

            cout << "\nIngrese usuario: ";

            getline(cin, usuario_actual);

            if (!existe_usuario(usuario_actual)) {

                cout << "\nEl usuario no existe.\n";

                continue;
            }

            cout << "\n====================================\n";
            cout << " Bienvenido "
                 << usuario_actual
                 << endl;
            cout << "====================================\n";

            mostrar_ver_despues(usuario_actual);

            recomendar_por_ultimo_like(
                engine,
                usuario_actual
            );

            cout << "\n====================================\n";
            cout << " Puedes buscar:\n";
            cout << "- titulos\n";
            cout << "- palabras\n";
            cout << "- frases\n";
            cout << "- subcadenas\n";
            cout << "====================================\n";

            buscarYDarLike(
                engine,
                usuario_actual
            );
        }

        // CREAR USUARIO
        else if (opcion == 2) {

            cout << "\nNuevo usuario: ";

            getline(cin, usuario_actual);

            if (existe_usuario(usuario_actual)) {

                cout << "\nEl usuario ya existe.\n";

                continue;
            }

            crear_usuario(usuario_actual);

            recomendaciones_nuevo(engine);

            cout << "\n====================================\n";
            cout << " Puedes buscar:\n";
            cout << "- titulos\n";
            cout << "- palabras\n";
            cout << "- frases\n";
            cout << "- subcadenas\n";
            cout << "====================================\n";

            buscarYDarLike(
                engine,
                usuario_actual
            );
        }

        else if (opcion == 3) {

            cout << "\nCerrando programa...\n";

            break;
        }

        else {

            cout << "\nOpcion invalida.\n";
        }
    }

    return 0;
}