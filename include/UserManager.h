#pragma once

#include <string>
#include "SearchEngine.h"
using namespace std;

void crear_usuario(const string& nombreArchivo);

void dar_me_gusta(
    const string& nombre_usuario,
    const string& linea
);

void mostrar_ver_despues(
    const string& nombre_usuario
);

void buscarYDarLike(
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

void recomendar_por_ultimo_like(
    SearchEngine& engine,
    const string& usuario
);

bool existe_usuario(const string& nombre);