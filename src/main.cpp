#include "../include/SearchEngine.h"
#include "../include/UserManager.h"
#include <functional>
#include <iostream>
#include <limits>
#include <thread>
#include <chrono>
#include <future>
using namespace std;
struct decoratorRecomendar{

    virtual void show(SearchEngine &buscar, string usuario = "") = 0;
    ~decoratorRecomendar(){}
};
struct RecomendcionBasica: public decoratorRecomendar {
  void show(SearchEngine & buscar, string usuario = "") override {
      if (usuario ==  "") {
          recomendaciones_nuevo(buscar);
      }
      else {
          recomendar_por_ultimo_like(
                buscar,
                usuario
            );
      }
  }
};
struct RecomendicionPremiun:public decoratorRecomendar {
    void show(SearchEngine & buscar, string usuario = "") override {
        if (usuario ==  "") {
            recomendaciones_nuevo(buscar);
        }
        else {
            recomendar_por_ultimo_like(
                  buscar,
                  usuario
              );
        }
        cout << "\n Recomendaciones Premium\n";
    }
};
void comencemos( SearchEngine &engine){

    engine.loadCSV(
            "../Data/wiki_movie_plots_deduped.csv"
        );
}
int main() {
    SearchEngine engine;
    system("chcp 65001");
    future<void> res = async(launch::async,
        comencemos,
        std::ref(engine));
    srand(time(NULL));
    decoratorRecomendar * recomendar = new RecomendcionBasica;

    cout << "====================================\n";
    cout << " Cargando base de datos...\n";
    cout << "====================================\n";

    const int segundos = 6;
    const int anchoBarra = 50;

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
            chrono::seconds(5)
        );
    }


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
            recomendar->show(engine, usuario_actual);

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

            recomendar->show(engine);

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
    delete recomendar;
    return 0;
}