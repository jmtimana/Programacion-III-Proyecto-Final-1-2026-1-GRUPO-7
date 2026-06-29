//
// Created by jhynh on 29/06/2026.
//

#ifndef ENTREGABLE1_LIKEOBSERVER_H
#define ENTREGABLE1_LIKEOBSERVER_H

#endif //ENTREGABLE1_LIKEOBSERVER_H
#pragma  once
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iostream>

using namespace std;

//  EVENTO: datos que viajan con cada notificacion

struct LikeEvent {
    string usuario;
    string titulo;
    string genero;
};

//  INTERFAZ OBSERVER

class ILikeObserver {
public:
    virtual ~ILikeObserver() = default;

    // Se llama automaticamente cada vez que alguien da un like
    virtual void onLike(const LikeEvent& evento) = 0;
};

//  Funcion: imprime en consola cada like en tiempo real
//  Util para depuracion y para que el usuario vea feedback.
class LikeLogger : public ILikeObserver {
public:
    void onLike(const LikeEvent& e) override {
        cout << "  [Observer] Like registrado — "
             << "Usuario: " << e.usuario
             << " | Genero: " << e.genero << "\n";
    }
};


// cuenta cuantos likes ha dado cada usuario durante la sesion actual (en memoria).

class LikeCounter : public ILikeObserver {
private:
    unordered_map<string, int> conteo;  // usuario -> cantidad de likes

public:
    void onLike(const LikeEvent& e) override {
        conteo[e.usuario]++;
    }

    int getLikes(const string& usuario) const {
        auto it = conteo.find(usuario);
        return (it != conteo.end()) ? it->second : 0;
    }

    // Muestra un resumen de likes por usuario al cerrar sesion
    void mostrarResumen() const {
        if (conteo.empty()) return;
        cout << "\n--- Likes dados en esta sesion ---\n";
        for (const auto& [u, c] : conteo)
            cout << "  " << u << ": " << c << " like(s)\n";
        cout << "----------------------------------\n";
    }
};
//Las peliculas más linkeadas por el usuario
class GenreStatsObserver : public ILikeObserver {
private:
    unordered_map<string, int> generoConteo;  // genero -> total de likes

public:
    void onLike(const LikeEvent& e) override {
        if (!e.genero.empty())
            generoConteo[e.genero]++;
    }

    // Devuelve el genero mas likeado en la sesion
    string generoMasPopular() const {
        if (generoConteo.empty()) return "";
        return max_element(
            generoConteo.begin(), generoConteo.end(),
            [](const auto& a, const auto& b){ return a.second < b.second; }
        )->first;
    }

    // Muestra ranking Top-N de generos
    void mostrarTop(int n = 5) const {
        if (generoConteo.empty()) {
            cout << "  (sin likes en esta sesion)\n";
            return;
        }
        // Ordenar de mayor a menor
        vector<pair<string,int>> lista(generoConteo.begin(), generoConteo.end());
        sort(lista.begin(), lista.end(),
             [](const auto& a, const auto& b){ return a.second > b.second; });

        cout << "\n--- Top generos likeados (sesion) ---\n";
        int mostrar = min(n, (int)lista.size());
        for (int i = 0; i < mostrar; i++)
            cout << "  " << (i+1) << ". " << lista[i].first
                 << " (" << lista[i].second << " like(s))\n";
        cout << "-------------------------------------\n";
    }

    const unordered_map<string,int>& getConteo() const { return generoConteo; }
};


//  se disparan notificaciones.

class LikeEventBus {
private:
    vector<ILikeObserver*> observers;

    LikeEventBus() = default;

public:
    // Singleton: acceso global
    static LikeEventBus& getInstance() {
        static LikeEventBus instance;
        return instance;
    }

    // Deshabilitar copia
    LikeEventBus(const LikeEventBus&)            = delete;
    LikeEventBus& operator=(const LikeEventBus&) = delete;

    // Suscribir un observador
    void subscribe(ILikeObserver* obs) {
        observers.push_back(obs);
    }

    // Desuscribir
    void unsubscribe(ILikeObserver* obs) {
        observers.erase(
            remove(observers.begin(), observers.end(), obs),
            observers.end()
        );
    }

    // Notificar a todos los observadores — se llama desde dar_me_gusta()
    void notify(const string& usuario, const string& genero,
                const string& titulo = "") {
        LikeEvent e{ usuario, titulo, genero };
        for (ILikeObserver* obs : observers)
            obs->onLike(e);
    }
};