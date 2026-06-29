#include "../include/SearchEngine.h"
#include "../include/UserManager.h"
#include "../include/PlanFactory.h"
#include "../include/PlanSession.h"
#include "../include/LikeObserver.h"
#include <functional>
#include <iostream>
#include <limits>
#include <thread>
#include <chrono>
#include <future>
using namespace std;

void comencemos( SearchEngine &engine){

    engine.loadCSV(
            "../Data/wiki_movie_plots_deduped.csv"
        );
}

IPlanFactory* seleccionarPlan() {
    int op;
    while (true) {
        cout << "\n========================================\n";
        cout << "   STREAMING PLATFORM P3\n";
        cout << "   Seleccione su plan de suscripcion:\n";
        cout << "========================================\n";
        cout << "[1] Plan Individual\n";
        cout << "    - 1 perfil, hasta 3 likes, sin watchlist\n";
        cout << "[2] Plan Familiar\n";
        cout << "    - Hasta 5 perfiles, likes ilimitados, watchlist\n";
        cout << "========================================\n";
        cout << "Seleccione: ";

        cin >> op;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nOpcion invalida.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (op == 1) return new PlanIndividualFactory();
        if (op == 2) return new PlanFamiliarFactory();
        cout << "\nOpcion invalida.\n";
    }
}

int main() {
    //carga de archivos asincrona
    SearchEngine engine;
    system("chcp 65001");
    future<void> res = async(launch::async,
        comencemos,
        std::ref(engine));
    srand(time(NULL));
    // ── OBSERVER: crear observadores y suscribirlos al bus ──
    LikeLogger likeLogger;
    LikeCounter likeCounter;
    GenreStatsObserver genreStats;

    LikeEventBus::getInstance().subscribe(&likeLogger);
    LikeEventBus::getInstance().subscribe(&likeCounter);
    LikeEventBus::getInstance().subscribe(&genreStats);
    // ── 1. ABSTRACT FACTORY: elegir plan
    IPlanFactory* factory = seleccionarPlan();
    PlanSession   planSession(factory);

    // ── 2. MENU DE GESTION DE USUARIOS (antes de cargar BD) ─
    //    Permite agregar/eliminar perfiles según las reglas del plan.
    planSession.menuGestionUsuarios();

    cout << "====================================\n";
    cout << " Cargando base de datos...\n";
    cout << "====================================\n";

    const int segundos = 10;
    const int anchoBarra = 100;

    for(int i = 0; i <= segundos; i++)
    {
        int progreso = i * anchoBarra / segundos;

        cout << "\r[";

        for(int j = 0; j < anchoBarra; j++)
        {
            if(j < progreso)
                cout << "=";
            else
                cout << " ";
        }

        cout << "] "
             << (i * 100 / segundos)
             << "%";

        cout.flush();

        this_thread::sleep_for(
            chrono::seconds(1)
        );
    }


    string usuario_actual;
    int opcion;
    while (true) {

        cout << "\n====================================\n";
        cout << "     STREAMING PLATFORM P3\n";
        cout << "====================================\n";
        cout << "[1] Ingresar usuario\n";
        cout << "[2] Gestionar usuario\n";
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
            planSession.mostrar_perfiles();
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
            if (tiene_likes(usuario_actual)) {
                recomendar_por_likes(engine, usuario_actual);
            }
            else {
                recomendaciones_nuevo(engine);
            }

            if (tiene_ver_despues(usuario_actual)) {
                if (factory->nombrePlan()=="Individual") {
                    mostrar_ver_despues(usuario_actual, 1);
                }
                else {
                    mostrar_ver_despues(usuario_actual, 5);
                }
            }
            cout << "\n====================================\n";
            cout << " Puedes buscar:\n";
            cout << "- titulos\n";
            cout << "- palabras\n";
            cout << "- frases\n";
            cout << "- subcadenas\n";
            cout << "====================================\n";

            while (true) {

                cout << "\n====================================\n";
                cout << "[1] Busqueda general\n";
                cout << "[2] Busqueda por categoria\n";
                cout << "[0] Cerrar sesion\n";
                cout << "====================================\n";
                cout << "Seleccione: ";

                int modo;
                cin >> modo;

                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "\nOpcion invalida.\n";
                    continue;
                }

                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (modo == 0) break;

                if (modo == 1) {
                    buscarYDarLike(engine, usuario_actual);
                    break;
                }

                if (modo == 2) {
                    buscarPorCategoria(engine, usuario_actual);
                    break;
                }

                cout << "\nOpcion invalida.\n";
            }
        }

        // Gestion de usarios
        else if (opcion == 2) {
            planSession.menuGestionUsuarios();

        }

        else if (opcion == 3) {
            likeCounter.mostrarResumen();
            genreStats.mostrarTop(5);

            cout << "\nCerrando programa...\n";
            std::terminate();
        }

        else {

            cout << "\nOpcion invalida.\n";
        }
    }
    return 0;
}