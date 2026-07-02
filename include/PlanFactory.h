#pragma once
#ifndef ENTREGABLE1_PLANFACTORY_H
#define ENTREGABLE1_PLANFACTORY_H

#endif //ENTREGABLE1_PLANFACTORY_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdexcept>

using namespace std;

class IGestorPerfiles {
public:
    virtual ~IGestorPerfiles() = default;

    virtual bool agregarUsuario(const string& nombre) = 0;

    virtual bool eliminarUsuario(const string& nombre) = 0;

    virtual int maxPerfiles() const = 0;

    virtual vector<string> listarUsuarios() const = 0;

    virtual string nombrePlan() const = 0;
};


class IGestorActividad {
public:
    virtual ~IGestorActividad() = default;

    virtual bool puedeDarLike(const string& usuario) const = 0;

    virtual bool tieneWatchlist() const = 0;

    virtual int maxLikes() const = 0;

    virtual void mostrarBeneficios() const = 0;
};

class PerfilIndividual : public IGestorPerfiles {
public:
    int maxPerfiles() const override { return 1; }
    string nombrePlan() const override { return "Individual"; }

    bool agregarUsuario(const string& nombre) override {

        if (ifstream(nombre + ".txt").good()) {
            cout << "  El usuario '" << nombre << "' ya existe.\n";
            return false;
        }
        ofstream f(nombre + ".txt");
        if (!f.is_open()) {
            cout << "  Error al crear el usuario.\n";
            return false;
        }
        f.close();
        cout << "  [Plan Individual] Usuario '" << nombre << "' creado.\n";
        return true;
    }

    bool eliminarUsuario(const string& nombre) override {
        if (!ifstream(nombre + ".txt").good()) {
            cout << "  El usuario '" << nombre << "' no existe.\n";
            return false;
        }
        if (remove((nombre + ".txt").c_str()) == 0) {
            cout << "  [Plan Individual] Usuario '" << nombre << "' eliminado.\n";
            return true;
        }
        cout << "  Error al eliminar el usuario.\n";
        return false;
    }

    vector<string> listarUsuarios() const override {
        vector<string> lista;
        ifstream reg("planes_individual.txt");
        string linea;
        while (getline(reg, linea))
            if (!linea.empty()) lista.push_back(linea);
        return lista;
    }
};

class PerfilFamiliar : public IGestorPerfiles {
public:
    int maxPerfiles() const override { return 5; }
    string nombrePlan() const override { return "Familiar"; }

    bool agregarUsuario(const string& nombre) override {
        if (ifstream(nombre + ".txt").good()) {
            cout << "  El usuario '" << nombre << "' ya existe.\n";
            return false;
        }
        ofstream f(nombre + ".txt");
        if (!f.is_open()) {
            cout << "  Error al crear el usuario.\n";
            return false;
        }
        f.close();
        cout << "  [Plan Familiar] Perfil '" << nombre << "' creado.\n";
        return true;
    }

    bool eliminarUsuario(const string& nombre) override {
        if (!ifstream(nombre + ".txt").good()) {
            cout << "  El usuario '" << nombre << "' no existe.\n";
            return false;
        }
        if (remove((nombre + ".txt").c_str()) == 0) {
            cout << "  [Plan Familiar] Perfil '" << nombre << "' eliminado.\n";
            return true;
        }
        cout << "  Error al eliminar el perfil.\n";
        return false;
    }

    vector<string> listarUsuarios() const override {
        vector<string> lista;
        ifstream reg("planes_familiar.txt");
        string linea;
        while (getline(reg, linea))
            if (!linea.empty()) lista.push_back(linea);
        return lista;
    }
};


class ActividadIndividual : public IGestorActividad {
public:
    int  maxLikes()      const override { return 3; }
    bool tieneWatchlist() const override { return false; }

    bool puedeDarLike(const string& usuario) const override {
        // Cuenta cuántas líneas "[like]" tiene el archivo
        ifstream f(usuario + ".txt");
        int count = 0;
        string linea;
        while (getline(f, linea))
            if (linea.find("[like]") != string::npos) count++;
        return count < maxLikes();
    }

    void mostrarBeneficios() const override {
        cout << "  Plan Individual:\n";
        cout << "    - 1 perfil de usuario\n";
        cout << "    - Hasta " << maxLikes() << " likes\n";
        cout << "    - Sin watchlist (solo podras seleccionar una pelicula para ver despues)\n";
    }
};

class ActividadFamiliar : public IGestorActividad {
public:
    int  maxLikes()       const override { return 0; }   // 0 = ilimitado
    bool tieneWatchlist() const override { return true; }

    bool puedeDarLike(const string& ) const override {
        return true;
    }

    void mostrarBeneficios() const override {
        cout << "  Plan Familiar:\n";
        cout << "    - Hasta 5 perfiles\n";
        cout << "    - Likes ilimitados\n";
        cout << "    - Watchlist compartida (hasta 5 peliculas para ver despues)\n";
    }
};

class IPlanFactory {
public:
    virtual ~IPlanFactory() = default;

    virtual IGestorPerfiles*  crearGestorPerfiles()  const = 0;
    virtual IGestorActividad* crearGestorActividad() const = 0;
    virtual string            nombrePlan()            const = 0;
};

class PlanIndividualFactory : public IPlanFactory {
public:
    IGestorPerfiles*  crearGestorPerfiles()  const override { return new PerfilIndividual(); }
    IGestorActividad* crearGestorActividad() const override { return new ActividadIndividual(); }
    string            nombrePlan()           const override { return "Individual"; }
};

class PlanFamiliarFactory : public IPlanFactory {
public:
    IGestorPerfiles*  crearGestorPerfiles()  const override { return new PerfilFamiliar(); }
    IGestorActividad* crearGestorActividad() const override { return new ActividadFamiliar(); }
    string            nombrePlan()           const override { return "Familiar"; }
};
