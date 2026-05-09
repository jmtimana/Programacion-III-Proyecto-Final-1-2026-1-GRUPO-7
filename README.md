# Proyecto #1 Programación III - Grupo 7

## Integrantes
- Timana Carmona, Jose Manuel
- Cussianovich Monti, Alejandra
- Kaufman Monzon, Corbin
- Huaman Huamani, Josue Yeremi
- Cunya Villalta, Jairo André

Sistema para la búsqueda y gestión de películas desarrollado en **C++17**, utilizando un **Trie** como estructura de datos principal.  
Permite realizar búsquedas por título, palabras clave, frases o subcadenas sobre un dataset de más de **34,000 películas** provenientes de Wikipedia.

---

# Estructura del Proyecto

```text
Entregable1/
├── CMakeLists.txt
├── include/
│   ├── Movie.h
│   ├── Parser.h
│   ├── SearchEngine.h
│   ├── Trie.h
│   ├── UserManager.h
│   └── Utils.h
├── src/
│   ├── main.cpp
│   ├── Parser.cpp
│   ├── SearchEngine.cpp
│   ├── Trie.cpp
│   ├── UserManager.cpp
│   └── Utils.cpp
└── Data/
    └── wiki_movie_plots_deduped.csv
```

# 1. Pre-procesamiento de Datos

Antes de insertar palabras en el Trie, el texto pasa por un pipeline de normalización que transforma los datos crudos del CSV en palabras listas para ser indexadas.

## Pipeline de Transformación

```text
Texto original (CSV)
        ↓
[Parser] parseCSVLine()
Maneja comillas y comas internas
        ↓
[Utils] normalizeAccents()
ASCII-folding de acentos
        ↓
[Utils] normalize()
Minúsculas + eliminación de caracteres no alfanuméricos
+ colapso de espacios múltiples
        ↓
[Utils] tokenize()
Split por espacios + filtrado de stopwords
+ eliminación de palabras con menos de 3 caracteres
        ↓
Palabras listas para insertar en el Trie
```

---

# Ejemplo de Transformación

| Etapa | Resultado |
|---|---|
| **Original** | `"The Dark Knight" — Action \| A billionaire fights crime in Gotham City. [1]` |
| **normalizeAccents** | `"The Dark Knight" — Action \| A billionaire fights crime in Gotham City. [1]` |
| **normalize** | `the dark knight action a billionaire fights crime in gotham city 1` |
| **tokenize** | `["dark", "knight", "action", "billionaire", "fights", "crime", "gotham", "city"]` |

Las palabras finales (`dark`, `knight`, `action`, etc.) son las que finalmente se insertan en el Trie junto con sus sufijos.

---

# Stopwords Filtradas

```cpp
unordered_set<string> stopwords = {
    // Artículos y determinantes
    "the", "a", "an", "this", "that", "these", "those",

    // Preposiciones (las más comunes)
    "of", "in", "to", "for", "with", "on", "at", "from", "by",
    "about", "into", "through", "during", "before", "after",
    "above", "below", "up", "out", "off", "over", "under",
    "again", "further", "then", "once", "here", "there",

    // Conjunciones
    "and", "but", "or", "yet", "so", "if", "because", "although",
    "though", "while", "where", "when", "than", "then",

    // Pronombres personales y posesivos
    "i", "you", "he", "she", "it", "we", "they", "me", "him",
    "her", "us", "them", "my", "your", "his", "its", "our",
    "their", "mine", "yours", "hers", "ours", "theirs",

    // Pronombres relativos
    "who", "whom", "whose", "which", "what",

    // Verbos auxiliares y "to be"
    "is", "am", "are", "was", "were", "be", "been", "being",
    "have", "has", "had", "do", "does", "did",
    "will", "would", "shall", "should", "may", "might",
    "can", "could", "must",

    // Adverbios comunes sin significado propio
    "not", "no", "so", "just", "only", "also", "too",
    "very", "really", "still", "already", "yet", "even",
    "now", "here", "there", "everywhere", "somewhere",

    // Metadatos del dataset (ruido)
    "en", "org", "https", "http", "wikipedia", "wiki",
    "ref", "cite", "url", "accessed", "retrieved",

    // Números ordinales/cardinales genéricos
    "one", "two", "first", "second"
};
```
# 2. Pseudocódigo de Inserción en la Estructura

```text
PROCEDIMIENTO IndexarPelicula(id, textoCompleto)

    textoNormalizado ← NORMALIZAR(textoCompleto)

    palabras ← TOKENIZAR(textoNormalizado)

    palabrasUnicas ← ELIMINAR_DUPLICADOS(palabras)

    PARA CADA palabra EN palabrasUnicas HACER

        SI LONGITUD(palabra) < 3 ENTONCES
            CONTINUAR
        FIN SI

        limite ← MIN(LONGITUD(palabra), 6)

        PARA i DESDE 0 HASTA limite - 1 HACER

            sufijo ← SUBCADENA(palabra, i, FIN)

            SI LONGITUD(sufijo) >= 3 ENTONCES
                INSERTAR_SUFIJO_EN_TRIE(id, sufijo)
            FIN SI

        FIN PARA

    FIN PARA

FIN PROCEDIMIENTO
```

```text
PROCEDIMIENTO INSERTAR_SUFIJO_EN_TRIE(id, sufijo)

    nodo ← raizTrie

    PARA CADA caracter EN sufijo HACER

        SI caracter NO ESTA EN nodo.hijos ENTONCES
            nodo.hijos[caracter] ← NUEVO_NODO()
        FIN SI

        nodo ← nodo.hijos[caracter]

        // Evita duplicados en el mismo nodo para la misma película
        SI nodo.peliculas.ULTIMO <> id ENTONCES
            nodo.peliculas.AGREGAR(id)
        FIN SI

    FIN PARA

FIN PROCEDIMIENTO
```

# 3. Estructura de Datos: Trie con Sufijos

## ¿Por qué un Trie?

El Trie (árbol de prefijos) permite búsquedas por prefijo en `O(m)` donde `m` es la longitud de la consulta, sin importar el tamaño del dataset.

Al insertar sufijos de cada palabra, esta estructura también soporta búsquedas por subcadenas.

---

## Estructura del Nodo

```cpp
struct Node {
    unordered_map<char, Node*> children;  // Aristas por carácter
    vector<int> movieIDs;                 // IDs de películas que pasan por este nodo
};
```

---

## Inserción con Sufijos

Para la palabra `"warcraft"`:

| Sufijo insertado | Longitud | ¿Insertado? |
|---|---|---|
| `"warcraft"` | 8 | ✅ |
| `"arcraft"` | 7 | ✅ |
| `"rcraft"` | 6 | ✅ |
| `"craft"` | 5 | ✅ |
| `"raft"` | 4 | ✅ |
| `"aft"` | 3 | ✅ |
| `"ft"` | 2 | ❌ (< 3) |

---

**Ventaja:**  
Buscar `"war"` encuentra coincidencias como `"warcraft"`, `"star wars"`, `"war"`, etc.

**Límite de 6 sufijos:**  
Se utiliza para balancear cobertura de búsqueda y consumo de memoria.  
Un límite mayor incrementaría considerablemente el tamaño del Trie.

---

## Algoritmo de Búsqueda

```text
FUNCION Buscar(consulta)

    consulta ← NORMALIZAR(consulta)

    palabras ← TOKENIZAR(consulta)

    resultados ← LISTA_VACIA

    PARA CADA palabra EN palabras HACER

        SI LONGITUD(palabra) < 3 ENTONCES
            CONTINUAR
        FIN SI

        nodo ← raizTrie

        existe ← VERDADERO

        PARA CADA caracter EN palabra HACER

            SI caracter NO ESTA EN nodo.hijos ENTONCES
                existe ← FALSO
                ROMPER
            FIN SI

            nodo ← nodo.hijos[caracter]

        FIN PARA

        SI existe ENTONCES
            resultados.AGREGAR_TODOS(nodo.peliculas)
        FIN SI

    FIN PARA

    resultados ← ORDENAR(resultados)

    resultados ← ELIMINAR_DUPLICADOS(resultados)

    RETORNAR resultados

FIN FUNCION
```

# 4. Complejidad Algorítmica

## 4.1 Pre-procesamiento de Texto

| Función | Complejidad Temporal | Complejidad Espacial | Justificación |
|---|---|---|---|
| `normalizeAccents(s)` | O(n × k) | O(n) | n = longitud del string. Para cada grupo de acentos (k grupos), busca y reemplaza en el string. |
| `normalize(s)` | O(n) | O(n) | Un solo recorrido para minúsculas + filtrado + colapso de espacios. |
| `tokenize(text)` | O(n) | O(w) | n = longitud del texto, w = cantidad de palabras válidas. Un solo recorrido con `stringstream`. |

**Complejidad total del pre-procesamiento por película:**  
O(n × k)

Donde:
- `n` = longitud total del texto de la película
- `k` = cantidad de grupos de acentos (constante ≈ 14)

---

## 4.2 Inserción en el Trie

| Función | Complejidad Temporal | Complejidad Espacial | Justificación |
|---|---|---|---|
| `insertSingleWord(id, word)` | O(L) | O(L) nuevos nodos | L = longitud de la palabra. Recorrido carácter por carácter. |
| `insertSuffixes(id, word)` | O(6 × L) = O(L) | O(6 × L) | Genera hasta 6 sufijos, cada uno de longitud decreciente. |
| `insert(id, text)` | O(P × 6 × L̄) | O(P × 6 × L̄) | P = palabras únicas después de tokenizar, L̄ = longitud promedio de palabra. |

### Dataset completo

Sea:
- `N = 34,886` películas
- `P ≈ 150` palabras únicas por película
- `L̄ ≈ 8` caracteres por palabra

### Complejidad total de indexación

```text
O(N × P × 6 × L̄) ≈ O(N × P × L̄)
```

Con los valores del dataset:

```text
34,886 × 150 × 6 × 8 ≈ 251 millones
```

aproximadamente **251 millones de operaciones de inserción de caracteres**.

### Espacio total del Trie

```text
O(N × P × 6 × L̄)
```

nodos en el peor caso teórico.

Gracias a la compartición de prefijos, el espacio real es menor.

---

## 4.3 Búsqueda en el Trie

| Función | Complejidad Temporal | Justificación |
|---|---|---|
| `search(query)` | O(K × M + R log R) | K = palabras en el query, M = longitud promedio de palabra, R = resultados acumulados. |

### Desglose

- `O(K × M)` → recorrido del Trie
- `O(R)` → acumulación de resultados
- `O(R log R)` → `sort()`
- `O(R)` → `unique()` y `erase()`

### Casos

**Peor caso:**  
Queries muy comunes (`"love"`, `"war"`).

**Mejor caso:**  
Queries raros donde domina `O(K × M)`.

---

## 4.4 Ranking de Resultados

| Función | Complejidad Temporal | Justificación |
|---|---|---|
| `rankResults(ids, query)` | O(R × (Q + L̄) + R log R) | R = resultados, Q = palabras del query, L̄ = longitud promedio de campos comparados. |

### Desglose

Para cada resultado:

- Tokenización: `O(Q)`
- Búsquedas `find()`: `O(L̄)`
- Comparación completa: `O(L̄)`

### Ordenamiento final

```text
sort() → O(R log R)
```

### Complejidad total

```text
O(R × (Q + L̄) + R log R)
```

---

## 4.5 Resumen de Complejidades

| Operación | Complejidad Temporal | Complejidad Espacial |
|---|---|---|
| Carga e indexación completa | O(N × P × L̄) | O(T) |
| Búsqueda simple | O(M + R log R) | O(R) |
| Búsqueda compuesta | O(K × M + R log R) | O(R) |
| Ranking | O(R × (Q + L̄) + R log R) | O(R) |
| Recomendación por género | O(G + R log R) | O(R) |
| Crear usuario | O(1) | O(1) |
| Dar like / Ver después | O(1) amortizado | O(1) |

### Variables

- `N` = películas del dataset
- `P` = palabras únicas por película
- `L̄` = longitud promedio de palabra
- `K` = palabras del query
- `M` = longitud promedio de palabra del query
- `R` = resultados encontrados
- `Q` = palabras tokenizadas
- `G` = longitud del género
- `T` = nodos totales del Trie

---

## 4.6 Comparación con Alternativas

| Estructura | Búsqueda | Inserción | Espacio | Subcadenas |
|---|---|---|---|---|
| Trie con sufijos | O(K × M) | O(N × P × L̄) | Alto | ✅ Sí |
| Hash Table simple | O(1) | O(N × P) | Medio | ❌ No |
| Árbol B/B+ | O(log N) | O(log N) | Medio | ❌ No |
| Inverted Index | O(K) | O(N × P) | Medio | ❌ No |
| Fuerza bruta | O(N × L) | O(1) | Bajo | ✅ Sí |

## Trade-off del Trie

El Trie consume más memoria, pero permite:

- búsquedas extremadamente rápidas
- búsquedas por prefijo
- búsquedas por subcadena
- rendimiento independiente del tamaño del dataset

Ideal para motores de búsqueda y autocompletado.

# 5. Módulos del Sistema

## Movie.h

Struct que representa una película con datos originales y normalizados.

| Campo | Descripción |
|---|---|
| `id` | Identificador único secuencial |
| `title`, `genre`, `cast`, `director`, `plot` | Datos originales del CSV |
| `normalizedTitle/Genre/Cast/Director/Plot` | Versiones procesadas para búsqueda |
| `normalizedText` | Concatenación de todos los campos normalizados para indexación |

---

## Utils

Funciones de pre-procesamiento de texto.

| Función | Descripción |
|---|---|
| `normalizeAccents(s)` | Reemplaza caracteres acentuados y especiales por ASCII equivalente (soporta español, turco, alemán, etc.) |
| `normalize(s)` | Aplica `normalizeAccents` + minúsculas + elimina caracteres no alfanuméricos + colapsa espacios múltiples |
| `tokenize(text)` | Divide en palabras, filtra stopwords en inglés y palabras menores a 3 caracteres |

---

## Trie

Estructura principal de indexación.

| Método | Descripción | Complejidad Temporal |
|---|---|---|
| `insertSingleWord(id, word)` | Inserta palabra carácter a carácter en el Trie | `O(L)` |
| `insertSuffixes(id, word)` | Genera hasta 6 sufijos de la palabra y los inserta | `O(6L) = O(L)` |
| `insert(id, text)` | Normaliza, tokeniza e inserta todos los sufijos únicos | `O(P × 6L̄)` |
| `search(query)` | Retorna IDs de películas que coinciden con el query | `O(K × M + R log R)` |

---

## Parser

Lectura robusta del CSV con campos multilínea.

| Función | Descripción |
|---|---|
| `parseCSVLine(line)` | Parsea línea respetando campos entre comillas con comas internas |
| `readCSVRows(file)` | Lee CSV completo manejando sinopsis multilínea (campos entre comillas que contienen saltos de línea) |

---

## SearchEngine

Orquesta la carga de datos, indexación y ranking.

| Método | Descripción |
|---|---|
| `loadCSV(filename)` | Carga el CSV, normaliza e indexa cada película en el Trie |
| `search(query)` | Delega búsqueda al Trie |
| `rankResults(ids, query)` | Ordena resultados por relevancia usando sistema de puntaje |
| `getMovie(id)` | Retorna la película por ID |

### Sistema de puntaje en `rankResults`

| Campo | Puntos |
|---|---|
| Título (query completo exacto) | `+20` |
| Título (palabra individual) | `+10` |
| Sinopsis | `+3` |
| Género | `+2` |
| Director | `+2` |
| Cast | `+1` |

---

## UserManager

Gestión de usuarios mediante archivos `.txt` (uno por usuario).

| Función | Descripción |
|---|---|
| `crear_usuario(nombre)` | Crea archivo `NombreUsuario.txt` |
| `existe_usuario(nombre)` | Verifica existencia del archivo |
| `dar_me_gusta(usuario, linea)` | Guarda like o "ver luego" en el archivo |
| `mostrar_ver_despues(usuario)` | Muestra entradas con etiqueta `[ver luego]` |
| `recomendar_por_ultimo_like(engine, usuario)` | Busca películas del género del último like |
| `recomendaciones_nuevo(engine)` | Muestra 5 películas aleatorias para usuarios nuevos |
| `buscarYDarLike(engine, usuario)` | Loop interactivo de búsqueda con paginación (5 resultados por página) |

---

## Formato del Archivo de Usuario

```text
[like] | Action, Adventure
[ver luego] The Dark Knight
[like] | Drama
```

# 6. Interfaz del Programa

La aplicación utiliza una interfaz de línea de comandos (**CLI**) basada en menús interactivos para facilitar la navegación y búsqueda de películas.

---

## 6.1 Pantalla de Inicio

```text
====================================
 Cargando base de datos...
====================================
Movies loaded: 34886

====================================
     STREAMING PLATFORM P3
====================================
[1] Ingresar usuario
[2] Crear usuario
[3] Salir
====================================
Seleccione:
```

---

## 6.2 Flujo para Usuario Existente

Al ingresar con un usuario existente, el sistema:

1. Muestra mensaje de bienvenida
2. Enseña la lista de películas guardadas en **"Ver después"**
3. Genera recomendaciones basadas en el último like:
    - `[like] | Género`
4. Inicia el loop de búsqueda interactiva

### Resultados de Búsqueda

```text
============= RESULTADOS =============
1. The Dark Knight
2. Batman Begins
3. Inception
4. Interstellar
5. The Prestige
======================================
[1-5] Ver pelicula
[9]   Ver mas
[0]   Nueva busqueda
======================================
Seleccione:
```

---

## 6.3 Detalle de Película

```text
====================================
The Dark Knight
====================================

SINOPSIS:
[billionaire fights crime in gotham city...]

====================================
[1] Dar like
[2] Ver despues
[0] Volver
====================================
Seleccione:
```

---

## 6.4 Flujo para Nuevo Usuario

Cuando se crea un nuevo usuario, el sistema:

1. Solicita un nombre de usuario
2. Muestra 5 películas aleatorias como recomendaciones iniciales
3. Permite acceso inmediato al loop de búsqueda

---

## 6.5 Diagrama de Flujo

```text
Inicio
  └── Cargar CSV → Indexar en Trie (34,886 películas)
        └── Menú Principal
              ├── [1] Ingresar usuario
              │     ├── Mostrar "Ver después"
              │     ├── Recomendar por último like
              │     └── Loop de búsqueda con paginación
              ├── [2] Crear usuario
              │     ├── 5 películas aleatorias
              │     └── Loop de búsqueda con paginación
              └── [3] Salir
```

---

# 7. Decisiones Técnicas

| Decisión | Justificación |
|---|---|
| Trie con sufijos (hasta 6) | Permite búsqueda de subcadenas sin realizar full-text scan. Limitar a 6 balancea cobertura y memoria. |
| Normalización extensiva de acentos | El dataset contiene películas en múltiples idiomas con caracteres especiales. |
| `movies.reserve(rows.size())` | Evita realocaciones del vector durante la carga masiva del CSV. |
| `allResults.reserve(2000)` | Reduce realocaciones frecuentes durante búsquedas grandes. |
| Stopwords en tokenizer | Filtra palabras comunes (`the`, `and`, `of`) que generan ruido y aumentan innecesariamente el Trie. |
| Persistencia en `.txt` | Permite guardar estado sin dependencias externas como SQLite o JSON. |
| Ranking por puntaje | Prioriza coincidencias en títulos sobre sinopsis, mejorando la relevancia de resultados. |
