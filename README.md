# Proyecto #1 — Programación III · Grupo 7 · Entrega Final

Motor de búsqueda y gestión de películas en **C++17** sobre un dataset de **34,886 películas** (Wikipedia), indexado con un **Trie de sufijos**.

## Integrantes
- Timana Carmona, Jose Manuel
- Cussianovich Monti, Alejandra
- Kaufman Monzon, Corbin
- Huaman Huamani, Josue Yeremi
- Cunya Villalta, Jairo André

---

## Requisitos de la entrega final y su ubicación

| # | Requisito de la entrega | Cumplido en | Sección |
|---|---|---|---|
| 1 | **Implementación del árbol** | `Trie.h` / `Trie.cpp` | Sección 4 |
| 2 | **Programación Paralela** | `SearchEngine::loadCSV` (`std::thread`) + `main` (`std::async`) | Sección 2 |
| 2 | **Programación Genérica** | `template leerLineasUsuario`, `FieldGetter = std::function<...>`, Strategy | Sección 3 |
| 3 | **Patrones de Diseño** | Singleton, Abstract Factory, Strategy, Observer | Sección 1 |
| 4 | **Interfaz con todas las herramientas** | `main.cpp` + `UserManager` + `PlanSession` | Sección 5 |

---

# 1. Patrones de Diseño

Al ver la necesidad de crear perfiles con restricciones según el plan de suscripción elegido, implementamos el patrón *Abstract Factory*. Según Gamma et al. (1994), este patrón es fundamental cuando un sistema debe ser independiente de cómo se crean, componen y representan sus productos, permitiendo intercambiar familias enteras de objetos de manera coherente sin que el cliente interactúe con clases concretas. Asimismo, para alternar entre lógicas de ordenamiento en la interfaz de comandos, aislamos los algoritmos de puntuación en componentes polimórficos; según Gamma et al. (1994), el patrón *Strategy* define una familia de algoritmos, encapsula cada uno y los vuelve intercambiables, permitiendo que la estrategia varíe independientemente de los clientes que la utilizan, lo cual se acopla al diseño de dependencias en nuestra clase `SearchEngine`.

Se aplican cuatro patrones, cada uno resolviendo un problema concreto del sistema.

| Patrón | Tipo | Problema que resuelve | Clases clave | Archivo |
|---|---|---|---|---|
| **Singleton** | Creacional | Una sola instancia global del motor y del bus de eventos | `SearchEngine`, `LikeEventBus` | `SearchEngine.h`, `LikeObserver.h` |
| **Abstract Factory** | Creacional | Crear familias de objetos según el plan de suscripción | `IPlanFactory` a `PlanIndividualFactory`, `PlanFamiliarFactory` | `PlanFactory.h` |
| **Strategy** (x2) | Comportamiento | Intercambiar el algoritmo de **ranking** y de **coincidencia** en tiempo de ejecución | `IRankingStrategy`, `ISearchMatchStrategy` | `RankingStrategy.*`, `SearchMatchStrategy.*` |
| **Observer** | Comportamiento | Reaccionar a cada *like* sin acoplar el emisor a los consumidores | `LikeEventBus` + `ILikeObserver` | `LikeObserver.h` |

---

## 1.1 Singleton

```cpp
// SearchEngine.h — constructor privado + punto de acceso único
private:
    SearchEngine() = default;                         // nadie lo instancia
public:
    static SearchEngine& getInstance() {              // acceso global
        static SearchEngine instance;                 // creada una sola vez
        return instance;
    }
    SearchEngine(const SearchEngine&)            = delete;   // sin copias
    SearchEngine& operator=(const SearchEngine&) = delete;
```

| Regla del patrón | Cómo se garantiza |
|---|---|
| Instancia única | Variable `static` local en `getInstance()` (thread-safe en C++11 en adelante) |
| Sin copias ni asignación | `= delete` en el constructor de copia y en `operator=` |
| Acceso global | `SearchEngine::getInstance()` desde cualquier punto |

`LikeEventBus` aplica el mismo patrón para centralizar la publicación de eventos de *like*.

---

## 1.2 Abstract Factory

El usuario selecciona un **plan** y este produce una **familia coherente** de objetos (gestión de perfiles y reglas de actividad) sin que `main` conozca las clases concretas.

```text
              IPlanFactory (abstracta)
             /                        \
 PlanIndividualFactory         PlanFamiliarFactory
     |  crea                        |  crea
     +-- PerfilIndividual           +-- PerfilFamiliar        (IGestorPerfiles)
     +-- ActividadIndividual        +-- ActividadFamiliar     (IGestorActividad)
```

| Producto | Plan Individual | Plan Familiar |
|---|---|---|
| Máximo de perfiles | 1 | 5 |
| Máximo de likes | 3 | Ilimitados |
| Watchlist | No disponible | Sí (hasta 5) |

```cpp
// main.cpp — el cliente solo pide una fábrica; no conoce la clase concreta
IPlanFactory* factory = seleccionarPlan();   // Individual o Familiar
PlanSession   planSession(factory);          // arma toda la familia
```

---

## 1.3 Strategy (doble)

Dos ejes de comportamiento intercambiables en tiempo de ejecución.

**A) Ranking de resultados** — `IRankingStrategy`

| Estrategia | Criterio | `nombre()` |
|---|---|---|
| `RelevanceRankingStrategy` | Puntaje ponderado por campo | `"Relevancia"` |
| `AlphabeticalRankingStrategy` | Orden alfabético del título | `"Alfabetico (A-Z)"` |

Puntajes de la estrategia de relevancia:

| Campo | Puntos |
|---|---|
| Título (frase completa) | +20 |
| Título (palabra) | +10 |
| Sinopsis | +3 |
| Género | +2 |
| Director | +2 |
| Reparto | +1 |

**B) Coincidencia de búsqueda** — `ISearchMatchStrategy`

| Estrategia | Lógica | `nombre()` |
|---|---|---|
| `AllWordsMatchStrategy` | Todas las palabras (AND) | `"Todas las palabras (AND)"` |
| `AnyWordMatchStrategy` | Cualquier palabra (OR) | `"Cualquier palabra (OR)"` |
| `ExactPhraseMatchStrategy` | Subcadena exacta | `"Frase exacta"` |

```cpp
// SearchEngine.h — el motor delega en la estrategia inyectada
void setRankingStrategy(IRankingStrategy* s) { delete rankingStrategy; rankingStrategy = s; }
void setMatchStrategy  (ISearchMatchStrategy* s) { delete matchStrategy;   matchStrategy   = s; }
```

---

## 1.4 Observer

Cada *like* dispara un evento que se propaga a N observadores suscritos, sin acoplar `dar_me_gusta()` a ellos.

```text
 dar_me_gusta()  --->  LikeEventBus.notify(LikeEvent)
                            |  (Singleton)
        +-------------------+-------------------+
        v                   v                   v
   LikeLogger         LikeCounter        GenreStatsObserver
 (log en consola)  (conteo x usuario)  (top de géneros)
```

| Observador | Responsabilidad |
|---|---|
| `LikeLogger` | Imprime feedback en tiempo real |
| `LikeCounter` | Cuenta likes por usuario en la sesión |
| `GenreStatsObserver` | Ranking Top-N de géneros más likeados |

```cpp
// main.cpp — la suscripción no obliga a modificar el emisor
LikeEventBus::getInstance().subscribe(&likeLogger);
LikeEventBus::getInstance().subscribe(&likeCounter);
LikeEventBus::getInstance().subscribe(&genreStats);
```

---

# 2. Programación Paralela

Dos técnicas combinadas en la carga de datos.

| Técnica | Dónde | Qué paraleliza |
|---|---|---|
| `std::thread` (chunks) | `SearchEngine::loadCSV` | Parseo y normalización de las 34,886 filas |
| `std::async` | `main.cpp` | Solapa toda la carga con la interacción del usuario |

Al paralelizar el parseo y la normalización de las filas mediante el uso distribuido de hilos concurrentes, maximizamos la ocupación de los núcleos de la CPU. Según Williams (2019), la clave para lograr una concurrencia de alto rendimiento radica en dividir las tareas de forma que se minimice el uso de mecanismos de sincronización bloqueantes y se maximice el aislamiento de los datos durante la ejecución. En nuestro proyecto, este principio se garantiza asignando rangos indexados independientes a cada hilo, evitando colisiones de memoria en el vector global. 

## 2.1 Reparto en threads

```cpp
// SearchEngine.cpp
unsigned h = thread::hardware_concurrency();        // núcleos disponibles
for (unsigned t = 0; t < h; t++) {
    int begin = 1 + t * totalRows / h;              // rango [begin, end)
    int end   = 1 + (t + 1) * totalRows / h;
    threads.emplace_back(&SearchEngine::processChunk, this, cref(rows), begin, end);
}
for (auto& th : threads) th.join();                 // barrera de sincronización
```

Cada thread escribe en su propio rango de índices de `movies`, por lo que no hay condiciones de carrera (no comparten posiciones).

## 2.2 Carga asíncrona (solape con la interfaz)

```cpp
// main.cpp — la base de datos carga en segundo plano mientras el usuario
// elige plan y gestiona perfiles
future<void> res = async(launch::async, comencemos, std::ref(engine));
...
seleccionarPlan();                   // el usuario decide su plan
planSession.menuGestionUsuarios();   // agrega o elimina perfiles
res.get();                           // recién aquí se espera al fin de la carga
```

## 2.3 Decisión de diseño: el Trie no se paraleliza

El `Trie` usa punteros crudos compartidos y no es thread-safe. Insertar en paralelo produciría condiciones de carrera, por lo que la construcción del árbol se mantiene secuencial de forma deliberada. Esta decisión explica los tiempos de la sección 6.

---

# 3. Programación Genérica

| Mecanismo | Dónde | Qué generaliza |
|---|---|---|
| **Template de función** | `UserManager.h` | Recorrer líneas del archivo con cualquier callback |
| **`std::function`** | `SearchEngine::searchByField` | Elegir el campo a buscar como parámetro |
| **Polimorfismo de interfaz** | Strategy, Observer, Factory | Algoritmos intercambiables sin tocar el cliente |

```cpp
// UserManager.h — funciona con lambdas, functores o funciones libres
template <typename Func>
void leerLineasUsuario(const string& usuario, const string& prefijo, Func porLinea) {
    ifstream file(usuario + ".txt");
    string linea;
    while (getline(file, linea))
        if (linea.find(prefijo) != string::npos)
            porLinea(linea);            // Func es genérico
}
```
Implementamos componentes parametrizados en tiempo de compilación. La plantilla función `leerLineasUsuario` fue diseñada para procesar el historial de interacciones. Según Stroustrup (2018), el paradigma de la programación genérica consiste en centrarse en la formulación de algoritmos de forma abstracta, de modo que funcionen con una amplia gama de tipos de datos sin sacrificar la eficiencia de la máquina ni generar sobrecarga en tiempo de ejecución.

```cpp
// SearchEngine.h — el getter del campo es un parámetro genérico
using FieldGetter = function<string(const Movie&)>;
Resultados searchByField(const string& query, FieldGetter fieldGetter);

// uso: buscar solo por director, género, reparto o título con la misma función
searchByField(q, [](const Movie& m){ return m.normalizedDirector; });
```

Al filtrar por categorías como director, género o reparto se estructuró la lectura con (`FieldGetter`). Según Stroustrup (2018), el uso de abstracciones funcionales robustas eleva la cohesión del diseño de software, permitiendo vincular limpiamente las estructuras lógicas del motor con los datos físicos subyacentes de la entidad `Movie`.

---

# 4. Árbol: Trie de Sufijos

Estructura principal de indexación. Permite búsqueda por prefijo y por subcadena en `O(m)`, de forma independiente del tamaño del dataset.

```cpp
// Trie.h
struct Node {
    unordered_map<char, Node*> children;   // aristas por carácter
    vector<int> movieIDs;                  // películas que pasan por el nodo
};
```

**Inserción con sufijos** (`Trie.cpp`): por cada palabra se generan hasta **6 sufijos**, cada uno con longitud **mayor o igual a 3**.

| Palabra `"warcraft"` a sufijo | Longitud | ¿Insertado? |
|---|---|---|
| `warcraft` | 8 | Sí |
| `arcraft` | 7 | Sí |
| `rcraft` | 6 | Sí |
| `craft` | 5 | Sí |
| `raft` | 4 | Sí |
| `aft` | 3 | Sí |
| `ft` | 2 | No (menor a 3) |

Buscar `"war"` encuentra coincidencias como `warcraft`, `star wars` o `war`. El límite de 6 sufijos balancea la cobertura de búsqueda frente al consumo de memoria.

La recuperación de películas mediante subcadenas (como buscar "bar" y recuperar coincidencias exactas como "warcraft") requería una estructura indexada superior. Para resolver esto de manera óptima, desarrollamos un Trie modificado con la generación e inserción de hasta 6 sufijos decrecientes por token. Según Cormen et al. (2009), los algoritmos de correspondencia de patrones por fuerza bruta sufren de una ineficiencia crítica al depender linealmente del tamaño total del corpus analizado, mientras que las estructuras de búsqueda digital o árboles de prefijos permiten realizar búsquedas cuyo costo computacional depende estrictamente de la longitud de la clave consultada. En nuestra implementación, la complejidad temporal se reduce a un $O(m)$, donde $m$ representa la longitud de la consulta.

## Complejidad

| Operación | Tiempo | Espacio |
|---|---|---|
| Indexación completa | O(N · P · L̄) | O(T) |
| Búsqueda simple | O(M + R log R) | O(R) |
| Búsqueda compuesta | O(K · M + R log R) | O(R) |
| Ranking | O(R · (Q + L̄) + R log R) | O(R) |
| Like / Ver después | O(1) amortizado | O(1) |

Donde `N` = películas, `P` = palabras únicas por película, `L̄` = longitud media de palabra, `K` = palabras del query, `M` = longitud media del query, `R` = resultados, `T` = nodos del Trie.

---

# 5. Interfaz y Herramientas

La aplicación es una interfaz de línea de comandos (CLI) basada en menús. Flujo completo con todas las herramientas:

```text
main
 |- async: loadCSV() ------------> (carga en segundo plano)
 |- [Abstract Factory] seleccionarPlan()  -> Individual / Familiar
 |- [Observer] suscribir LikeLogger, LikeCounter, GenreStatsObserver
 |- PlanSession.menuGestionUsuarios()      -> agregar / eliminar perfiles
 |- res.get()  <---- espera fin de carga
 |- Menú principal
     |- [1] Ingresar usuario
     |     |- recomendaciones (por último like / aleatorias)
     |     |- mostrar "Ver después"
     |     |- búsqueda
     |          |- [1] general       -> buscarYDarLike()
     |          |- [2] por categoría -> buscarPorCategoria()  (Strategy de match)
     |- [2] Gestionar usuario
     |- [3] Salir -> LikeCounter.resumen() + GenreStats.top(5)  (Observer)
```

| Herramienta | Acción | Patrón o técnica que la soporta |
|---|---|---|
| Selección de plan | Individual o Familiar | Abstract Factory |
| Gestión de perfiles | Crear o eliminar según el límite del plan | Abstract Factory |
| Búsqueda general | Título, palabra, frase, subcadena | Trie + Strategy (ranking) |
| Búsqueda por categoría | Director, Género, Reparto, Título | `searchByField` (genérico) + Strategy (match) |
| Like / Ver después | Guardar en `usuario.txt` | Observer (eventos) |
| Recomendaciones | Por último like o aleatorias | — |
| Resumen de sesión | Top de géneros y conteo de likes | Observer |

---

# 6. Comparación de Tiempos: Entrega Anterior vs Entrega Final

La entrega anterior cargaba los datos de forma **secuencial** (un solo bucle que parseaba, normalizaba e insertaba en el Trie, bloqueante y sin medición de tiempos). La entrega final incorpora parseo **paralelo**, Trie secuencial, instrumentación con `chrono` y carga **asíncrona**.

## 6.1 Medición sobre el dataset completo (34,886 películas)

Medición tomada con optimización `-O2`, promedio de tres corridas, sobre un entorno de un núcleo. La entrega anterior no instrumentaba el tiempo por fase (todo ocurría en un único bucle), por lo que de ella solo se dispone del total. La entrega final sí mide cada fase con `chrono`.

**Comparación del tiempo total de carga**

| Versión | Modelo de carga | Total |
|---|---|---|
| Anterior | Secuencial, un solo bucle, sin instrumentación | 24.51 s |
| Final | Parseo en threads + Trie secuencial + carga asíncrona | 22.26 s |

**Desglose por fase de la entrega final** (instrumentado con `chrono`)

| Fase | Tiempo | ¿Paralelizable? |
|---|---|---|
| Lectura del CSV | 0.11 s | No (operación de E/S) |
| Parseo y normalización | 1.89 s | Sí (mediante threads) |
| Construcción del Trie | 20.25 s | No (Trie no thread-safe) |
| **Total** | **22.26 s** | |

El desglose permite identificar dónde se concentra el tiempo; la escalabilidad en varios núcleos se proyecta en la sección 6.2.

## 6.2 Proyección con la Ley de Amdahl

Solo el parseo (1.89 s en un núcleo) escala con la cantidad de núcleos. El resto (lectura más construcción del Trie, aproximadamente 20.36 s) es serial.

$$T(p) = T_{serial} + \frac{T_{par}}{p} = 20.36 + \frac{1.89}{p}\ \ [\text{s}]$$

$$S(p) = \frac{1}{(1-f) + \dfrac{f}{p}}, \qquad f = \frac{1.89}{22.26} \approx 0.085$$

| Núcleos (p) | Tiempo de carga proyectado | Speedup del parseo | Speedup total |
|---|---|---|---|
| 1 | 22.26 s | 1.00x | 1.00x |
| 2 | 21.31 s | 2.00x | 1.04x |
| 4 | 20.83 s | 4.00x | 1.07x |
| 8 | 20.60 s | 8.00x | 1.08x |
| Infinito | 20.36 s | ∞ | 1.09x (techo) |

La paralelización del parseo aporta poco al tiempo total porque la construcción del Trie domina (cerca del 91 por ciento) y es serial. El techo teórico de la paralelización, según Amdahl, es aproximadamente 1.09x.

## 6.3 Ganancia principal: solape asíncrono

El uso de `std::async` oculta la carga detrás de la interacción del usuario (selección de plan y gestión de perfiles):

$$T_{percibido} = \max\bigl(0,\ T_{carga} - T_{interaccion\ usuario}\bigr)$$

Si el usuario tarda más de 22 segundos aproximadamente eligiendo su plan y sus perfiles, la espera percibida tiende a cero. Esta es la mejora de experiencia frente a la entrega anterior, que bloqueaba la ejecución hasta terminar de cargar.

---

# 7. Compilación

```bash
cmake -B build && cmake --build build
./build/Entregable1        # requiere Data/wiki_movie_plots_deduped.csv
```

Requisitos: C++17, CMake 3.30 o superior. El paralelismo emplea `<thread>` y `<future>` de la biblioteca estándar, sin dependencias externas.

# 8. Referencias Bibliográficas

* Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2009). *Introduction to Algorithms* (3rd ed.). MIT Press.
* Gamma, E., Helm, R., Johnson, R., & Vlissides, J. (1994). *Design Patterns: Elements of Reusable Object-Oriented Software*. Addison-Wesley.
* Stroustrup, B. (2018). *A Tour of C++* (2nd ed.). Addison-Wesley.
* Williams, A. (2019). *C++ Concurrency in Action* (2nd ed.). Manning Publications.
