//
// Created by jhynh on 27/06/2026.
//
#pragma once
#ifndef ENTREGABLE1_PLANSESSION_H
#define ENTREGABLE1_PLANSESSION_H

#endif //ENTREGABLE1_PLANSESSION_H

#include "PlanFactory.h"
#include <fstream>
#include <limits>
#include <set>

class PlanSession {
private:
    IPlanFactory*     factory    = nullptr;
    IGestorPerfiles*  perfiles   = nullptr;
    IGestorActividad* actividad  = nullptr;
    string planFile;
    vector<string> usuariosActivos;

    void cargarRegistro() {
        usuariosActivos.clear();
        ifstream f(planFile);
        string linea;
        while (getline(f, linea))
            if (!linea.empty()) usuariosActivos.push_back(linea);
    }

    void guardarRegistro() const {
        ofstream f(planFile);
        for (const auto& u : usuariosActivos)
            f << u << "\n";
    }

    bool enLista(const string& nombre) const {
        return find(usuariosActivos.begin(), usuariosActivos.end(), nombre)
               != usuariosActivos.end();
    }

public:
    explicit PlanSession(IPlanFactory* f)
        : factory(f),
          perfiles(f->crearGestorPerfiles()),
          actividad(f->crearGestorActividad()),
          planFile("plan_" + f->nombrePlan() + ".txt")
    {
        cargarRegistro();
    }

    ~PlanSession() {
        delete perfiles;
        delete actividad;
    }

    IGestorActividad* getActividad() const { return actividad; }
    IGestorPerfiles*  getPerfiles()  const { return perfiles;  }
    const vector<string>& getUsuariosActivos() const { return usuariosActivos; }

    void menuGestionUsuarios() {
        int op;
        while (true) {
            cout << "\n========================================\n";
            cout << "  GESTION DE USUARIOS — " << factory->nombrePlan() << "\n";
            cout << "========================================\n";

            if (usuariosActivos.empty()) {
                cout << "  (sin usuarios registrados en este plan)\n";
            } else {
                cout << "  Perfiles (" << usuariosActivos.size()
                     << "/" << perfiles->maxPerfiles() << "):\n";
                for (size_t i = 0; i < usuariosActivos.size(); ++i)
                    cout << "    " << (i+1) << ". " << usuariosActivos[i] << "\n";
            }

            actividad->mostrarBeneficios();

            cout << "----------------------------------------\n";
            cout << "[1] Agregar usuario\n";
            cout << "[2] Eliminar usuario\n";
            cout << "[0] Continuar al menu principal\n";
            cout << "----------------------------------------\n";
            cout << "Seleccione: ";

            cin >> op;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "\nOpcion invalida.\n";
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (op == 0) break;

            // ── AGREGAR ──────────────────────────────────────
            if (op == 1) {
                // Verificar límite del plan
                if ((int)usuariosActivos.size() >= perfiles->maxPerfiles()) {
                    cout << "\n  Limite de " << perfiles->maxPerfiles()
                         << " perfiles alcanzado para el Plan "
                         << factory->nombrePlan() << ".\n";
                    continue;
                }

                cout << "\nNombre del nuevo usuario: ";
                string nombre;
                getline(cin, nombre);
                if (nombre.empty()) { cout << "  Nombre vacio.\n"; continue; }

                if (enLista(nombre)) {
                    cout << "  '" << nombre << "' ya esta en este plan.\n";
                    continue;
                }

                if (perfiles->agregarUsuario(nombre)) {
                    usuariosActivos.push_back(nombre);
                    guardarRegistro();
                }
                continue;
            }

            if (op == 2) {
                if (usuariosActivos.empty()) {
                    cout << "\n  No hay usuarios para eliminar.\n";
                    continue;
                }

                cout << "\nNombre del usuario a eliminar: ";
                string nombre;
                getline(cin, nombre);
                if (nombre.empty()) { cout << "  Nombre vacio.\n"; continue; }

                if (!enLista(nombre)) {
                    cout << "  '" << nombre << "' no esta en este plan.\n";
                    continue;
                }

                if (perfiles->eliminarUsuario(nombre)) {
                    usuariosActivos.erase(
                        find(usuariosActivos.begin(), usuariosActivos.end(), nombre)
                    );
                    guardarRegistro();
                }
                continue;
            }

            cout << "\nOpcion invalida.\n";
        }
    }
    void mostrar_perfiles() {

        cout << "========================================\n";

        if (usuariosActivos.empty()) {
            cout << "  (sin usuarios registrados en este plan)\n";
        } else {
            cout << "  Perfiles (" << usuariosActivos.size()
                 << "/" << perfiles->maxPerfiles() << "):\n";
            for (size_t i = 0; i < usuariosActivos.size(); ++i)
                cout << "    " << (i+1) << ". " << usuariosActivos[i] << "\n";
        }
        cout << "\n========================================\n";
    }
};
