#pragma once

#include <string>
#include "SearchEngine.h"
using namespace std;

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