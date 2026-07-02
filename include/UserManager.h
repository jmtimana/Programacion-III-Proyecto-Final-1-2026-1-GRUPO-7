#pragma once

#include <string>
#include <fstream>
#include "SearchEngine.h"
using namespace std;

enum class CampoBusqueda { DIRECTOR = 1, GENERO = 2, REPARTO = 3, TITULO = 4 };
enum class MovieAction { VOLVER = 0, DAR_LIKE = 1, VER_DESPUES = 2 };

constexpr int NUEVA_BUSQUEDA = 0;
constexpr int VER_MAS = 9;

constexpr const char* LIKE_TAG = "[like]";
constexpr const char* VER_DESPUES_TAG = "[ver luego]";
constexpr const char* LIKE_PREFIX = "[like] | ";
constexpr const char* VER_DESPUES_PREFIX = "[ver luego] ";

template <typename Func>
void leerLineasUsuario(const string& usuario, const string& prefijo, Func porLinea) {
    ifstream file(usuario + ".txt");
    if (!file.is_open()) return;
    string linea;
    while (getline(file, linea)) {
        if (linea.find(prefijo) != string::npos) {
            porLinea(linea);
        }
    }
}

void dar_me_gusta(
    const string& nombre_usuario,
    const string& linea
);

void mostrar_ver_despues(
    const string& nombre_usuario,int m
);

void buscarYDarLike(
    SearchEngine& engine,
    const string& usuario
);

void buscarPorCategoria(
    SearchEngine& engine,
    const string& usuario
);

void recomendaciones_nuevo(
    SearchEngine& engine
);

string obtener_ultimo_like(
    const string& usuario
);

string extraer_genero(
    const string& linea
);
bool tiene_likes(const string& usuario);
bool tiene_ver_despues(const string& usuario);
void recomendar_por_likes(
    SearchEngine& engine,
    const string& usuario
);

bool existe_usuario(const string& nombre);