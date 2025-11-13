// SISTEMA DE GESTIÓN DE PROCESOS (LISTA + COLA + PILA)
// Nivel estudiante - solo structs y punteros, C++ clásico

#include <iostream>
#include <cstring>
#include <limits>
#include <cctype>
#include <locale.h>

using namespace std;

const int MAX_NOMBRE  = 40;
const int MAX_DETALLE = 60;

// ------------------ ESTRUCTURAS ------------------

struct Proceso {
    int pid;                       // ID automático
    char nombre[MAX_NOMBRE];       // nombre del proceso / programa
    char detalle[MAX_DETALLE];     // descripción / comentario
    int prioridad;                 // 1 = mayor prioridad, 5 = menor
};

struct NodoLista {
    Proceso data;
    NodoLista *sig;
};

struct NodoCola {
    Proceso *ref;      // apunta al Proceso que está en la lista
    NodoCola *sig;
};

struct NodoPila {
    Proceso *ref;      // proceso cargado en memoria
    NodoPila *ant;
};

// ------------------ VARIABLES GLOBALES ------------------

NodoLista *listaProcesos = NULL;   // lista enlazada (todos los procesos)
NodoCola  *frenteCola    = NULL;   // cola de procesos listos (según prioridad)
NodoPila  *topePila      = NULL;   // pila de memoria
int ultimoPID = 0;

// ------------------ HELPERS GENERALES ------------------

void limpiarBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int generarPID() {
    ultimoPID++;
    return ultimoPID;
}

// Lee entero dentro de un rango
int leerEnteroEnRango(const char *mensaje, int minimo, int maximo) {
    int valor;
    bool ok = false;
    do {
        cout << mensaje;
        if (!(cin >> valor)) {
            cout << "Error: ingrese un número entero.\n";
            limpiarBuffer();
            continue;
        }
        if (valor < minimo || valor > maximo) {
            cout << "Error: el valor debe estar entre "
                 << minimo << " y " << maximo << ".\n";
        } else {
            ok = true;
        }
    } while (!ok);
    limpiarBuffer(); // limpia el salto de línea
    return valor;
}

// Valida que una cadena no esté vacía y tenga solo letras/espacios
bool validarTexto(const char texto[]) {
    int len = strlen(texto);
    if (len == 0) return false;
    for (int i = 0; i < len; i++) {
        if (!isalpha((unsigned char)texto[i]) && texto[i] != ' ')
            return false;
    }
    return true;
}

// ------------------ OPERACIONES SOBRE LISTA ------------------

// Mostrar todos los procesos en la lista
void mostrarListaProcesos() {
    if (!listaProcesos) {
        cout << "No hay procesos registrados en la lista.\n";
        return;
    }
    cout << "\n--- LISTA DE PROCESOS (LISTA ENLAZADA) ---\n";
    NodoLista *aux = listaProcesos;
    while (aux) {
        cout << "Proceso " << aux->data.pid
             << " | Nombre: "   << aux->data.nombre
             << " | Prioridad: " << aux->data.prioridad
             << " | Detalle: "  << aux->data.detalle << endl;
        aux = aux->sig;
    }
}

// Buscar nodo en la lista por PID
NodoLista* buscarNodoPorPID(int pidBuscado) {
    NodoLista *aux = listaProcesos;
    while (aux) {
        if (aux->data.pid == pidBuscado)
            return aux;
        aux = aux->sig;
    }
    return NULL;
}

// Registrar nuevo proceso (INSERTAR)
void registrarProceso() {
    NodoLista *nuevo = new NodoLista;
    nuevo->sig = NULL;

    nuevo->data.pid = generarPID();
    cout << "\n--- REGISTRO DE NUEVO PROCESO ---\n";
    cout << "PID asignado automáticamente: " << nuevo->data.pid << endl;

    // Nombre
    do {
        cout << "Ingrese nombre del proceso: ";
        cin.getline(nuevo->data.nombre, MAX_NOMBRE);
        if (!validarTexto(nuevo->data.nombre))
            cout << "Error: el nombre no debe estar vacío y solo puede tener letras y espacios.\n";
    } while (!validarTexto(nuevo->data.nombre));

    // Detalle
    do {
        cout << "Ingrese breve descripción del proceso: ";
        cin.getline(nuevo->data.detalle, MAX_DETALLE);
        if (strlen(nuevo->data.detalle) == 0)
            cout << "Error: la descripción no debe estar vacía.\n";
    } while (strlen(nuevo->data.detalle) == 0);

    // Prioridad
    nuevo->data.prioridad = leerEnteroEnRango("Ingrese prioridad (1 = mayor, 5 = menor): ", 1, 5);

    // Insertar al final de la lista
    if (!listaProcesos) {
        listaProcesos = nuevo;
    } else {
        NodoLista *aux = listaProcesos;
        while (aux->sig) aux = aux->sig;
        aux->sig = nuevo;
    }

    cout << "\n[OK] Proceso registrado correctamente.\n";
    cout << "Proceso " << nuevo->data.pid
         << " | Nombre: "   << nuevo->data.nombre
         << " | Prioridad: " << nuevo->data.prioridad << endl;
}

// Modificar proceso existente
void modificarProceso() {
    if (!listaProcesos) {
        cout << "No hay procesos para modificar.\n";
        return;
    }

    int pid = leerEnteroEnRango("Ingrese el PID del proceso a modificar: ", 1, 1000000);
    NodoLista *nodo = buscarNodoPorPID(pid);
    if (!nodo) {
        cout << "No existe un proceso con PID = " << pid << ".\n";
        return;
    }

    cout << "\n--- PROCESO ENCONTRADO ---\n";
    cout << "Proceso " << nodo->data.pid
         << " | Nombre: "   << nodo->data.nombre
         << " | Prioridad: " << nodo->data.prioridad
         << " | Detalle: "  << nodo->data.detalle << endl;

    // Nuevos datos
    cout << "\nIngrese nuevos datos (deje vacíos si no desea cambiar texto)\n";

    char buffer[MAX_DETALLE];

    // Nombre
    cout << "Nuevo nombre (enter para mantener): ";
    cin.getline(buffer, MAX_NOMBRE);
    if (strlen(buffer) > 0 && validarTexto(buffer)) {
        strcpy(nodo->data.nombre, buffer);
    }

    // Detalle
    cout << "Nueva descripción (enter para mantener): ";
    cin.getline(buffer, MAX_DETALLE);
    if (strlen(buffer) > 0) {
        strcpy(nodo->data.detalle, buffer);
    }

    // Prioridad
    cout << "¿Desea cambiar la prioridad? (1=Si, 2=No): ";
    int op;
    if (!(cin >> op)) {
        limpiarBuffer();
        op = 2;
    }
    limpiarBuffer();
    if (op == 1) {
        nodo->data.prioridad = leerEnteroEnRango("Nueva prioridad (1-5): ", 1, 5);
    }

    cout << "\n[OK] Proceso actualizado:\n";
    cout << "Proceso " << nodo->data.pid
         << " | Nombre: "   << nodo->data.nombre
         << " | Prioridad: " << nodo->data.prioridad
         << " | Detalle: "  << nodo->data.detalle << endl;
}

// Eliminar proceso de la lista por PID (solo de la lista)
void eliminarProceso() {
    if (!listaProcesos) {
        cout << "No hay procesos para eliminar.\n";
        return;
    }

    int pid = leerEnteroEnRango("Ingrese el PID del proceso a eliminar: ", 1, 1000000);

    NodoLista *actual = listaProcesos;
    NodoLista *anterior = NULL;
    bool encontrado = false;

    while (actual) {
        if (actual->data.pid == pid) {
            encontrado = true;
            break;
        }
        anterior = actual;
        actual = actual->sig;
    }

    if (!encontrado) {
        cout << "No se encontró un proceso con PID = " << pid << ".\n";
        return;
    }

    cout << "\nProcesando eliminación del Proceso " << actual->data.pid
         << " | Nombre: " << actual->data.nombre << endl;

    if (!anterior) {
        // era el primero
        listaProcesos = actual->sig;
    } else {
        anterior->sig = actual->sig;
    }

    delete actual;
    cout << "[OK] Proceso eliminado de la lista.\n";
}

// ------------------ OPERACIONES SOBRE COLA ------------------

// Verifica si un PID ya está en la cola
bool estaEnCola(int pid) {
    NodoCola *aux = frenteCola;
    while (aux) {
        if (aux->ref->pid == pid)
            return true;
        aux = aux->sig;
    }
    return false;
}

// Encolar manteniendo el orden por prioridad (menor número = más prioridad)
void encolarPorPrioridad(Proceso *p) {
    NodoCola *nuevo = new NodoCola;
    nuevo->ref = p;
    nuevo->sig = NULL;

    if (!frenteCola || p->prioridad < frenteCola->ref->prioridad) {
        nuevo->sig = frenteCola;
        frenteCola = nuevo;
        return;
    }

    NodoCola *aux = frenteCola;
    while (aux->sig && aux->sig->ref->prioridad <= p->prioridad) {
        aux = aux->sig;
    }
    nuevo->sig = aux->sig;
    aux->sig = nuevo;
}

// Mostrar la cola completa
void mostrarCola() {
    if (!frenteCola) {
        cout << "Cola vacía, no hay procesos para mostrar.\n";
        return;
    }
    cout << "\n--- COLA DE PROCESOS LISTOS (POR PRIORIDAD) ---\n";
    NodoCola *aux = frenteCola;
    while (aux) {
        cout << "Proceso " << aux->ref->pid
             << " | Nombre: "   << aux->ref->nombre
             << " | Prioridad: " << aux->ref->prioridad << endl;
        aux = aux->sig;
    }
}

// Desencolar: devuelve puntero al Proceso
bool desencolar(Proceso *&p) {
    if (!frenteCola) return false;

    NodoCola *tmp = frenteCola;
    p = tmp->ref;
    frenteCola = tmp->sig;
    delete tmp;
    return true;
}

// Transferir todos los procesos de la lista a la cola (si no están ya)
void transferirListaACola() {
    if (!listaProcesos) {
        cout << "No hay procesos en la lista para encolar.\n";
        return;
    }

    cout << "\nPrimero se mostrará la lista de procesos:\n";
    mostrarListaProcesos();

    NodoLista *aux = listaProcesos;
    while (aux) {
        if (!estaEnCola(aux->data.pid)) {
            encolarPorPrioridad(&aux->data);
        }
        aux = aux->sig;
    }
    cout << "\n[OK] Procesos transferidos a la cola según prioridad.\n";
}

// ------------------ OPERACIONES SOBRE PILA (MEMORIA) ------------------

// Push en pila (cargar proceso en memoria)
void pushMemoria(Proceso *p) {
    NodoPila *nuevo = new NodoPila;
    nuevo->ref = p;
    nuevo->ant = topePila;
    topePila = nuevo;
}

// Pop en pila (liberar memoria)
bool popMemoria(Proceso *&p) {
    if (!topePila) return false;
    NodoPila *tmp = topePila;
    p = tmp->ref;
    topePila = tmp->ant;
    delete tmp;
    return true;
}

// Mostrar pila de memoria
void mostrarPila() {
    if (!topePila) {
        cout << "Pila vacía, no hay procesos en memoria.\n";
        return;
    }
    cout << "\n--- PILA DE MEMORIA (TOPE = PROCESO ACTUAL) ---\n";
    NodoPila *aux = topePila;
    while (aux) {
        cout << "Proceso " << aux->ref->pid
             << " | Nombre: "   << aux->ref->nombre
             << " | Prioridad: " << aux->ref->prioridad << endl;
        aux = aux->ant;
    }
}

// ------------------ MENÚS ------------------

void menuLista() {
    int op;
    do {
        cout << "\n===== MENU LISTA DE PROCESOS =====\n";
        cout << "1. Registrar nuevo proceso\n";
        cout << "2. Modificar proceso por PID\n";
        cout << "3. Eliminar proceso por PID\n";
        cout << "4. Mostrar todos los procesos\n";
        cout << "5. Volver al menu principal\n";
        cout << "Seleccione una opcion: ";
        if (!(cin >> op)) {
            limpiarBuffer();
            op = 0;
        }
        limpiarBuffer();

        switch (op) {
        case 1:
            registrarProceso();
            break;
        case 2:
            modificarProceso();
            break;
        case 3:
            eliminarProceso();
            break;
        case 4:
            mostrarListaProcesos();
            break;
        case 5:
            // volver
            break;
        default:
            cout << "Opción inválida.\n";
        }
    } while (op != 5);
}

void menuCola() {
    int op;
    do {
        cout << "\n===== MENU PLANIFICADOR (COLA) =====\n";
        cout << "1. Transferir procesos de lista a cola (por prioridad)\n";
        cout << "2. Mostrar cola de procesos\n";
        cout << "3. Desencolar y ejecutar un proceso\n";
        cout << "4. Volver al menu principal\n";
        cout << "Seleccione una opcion: ";
        if (!(cin >> op)) {
            limpiarBuffer();
            op = 0;
        }
        limpiarBuffer();

        switch (op) {
        case 1:
            transferirListaACola();
            break;
        case 2:
            mostrarCola();
            break;
        case 3: {
            Proceso *p = NULL;
            if (!desencolar(p)) {
                cout << "Cola vacía, no hay procesos para desencolar.\n";
            } else {
                cout << "\n[CPU] Proceso " << p->pid
                     << " | Nombre: " << p->nombre
                     << " | Prioridad: " << p->prioridad << " --> ejecutando...\n";

                // Cargar en memoria
                pushMemoria(p);

                // Aquí se podría simular tiempo de ejecución, etc.
                // Por ahora, asumimos que termina de inmediato.
                Proceso *salio = NULL;
                if (popMemoria(salio)) {
                    cout << "[MEMORIA] Proceso " << salio->pid
                         << " liberado de memoria.\n";
                    cout << "[OK] Proceso ejecutado correctamente.\n";
                } else {
                    cout << "Error: la pila estaba vacía al intentar liberar memoria.\n";
                }
            }
        } break;
        case 4:
            // volver
            break;
        default:
            cout << "Opción inválida.\n";
        }
    } while (op != 4);
}

void menuPila() {
    int op;
    do {
        cout << "\n===== MENU MEMORIA (PILA) =====\n";
        cout << "1. Mostrar pila de memoria\n";
        cout << "2. Volver al menu principal\n";
        cout << "Seleccione una opcion: ";
        if (!(cin >> op)) {
            limpiarBuffer();
            op = 0;
        }
        limpiarBuffer();

        switch (op) {
        case 1:
            mostrarPila();
            break;
        case 2:
            // volver
            break;
        default:
            cout << "Opción inválida.\n";
        }
    } while (op != 2);
}

// ------------------ MAIN ------------------

int main() {
    setlocale(LC_ALL, "Spanish");

    int opcion;
    do {
        cout << "\n===== SISTEMA DE GESTION DE PROCESOS =====\n";
        cout << "1. Operaciones con procesos (LISTA)\n";
        cout << "2. Planificador de CPU (COLA)\n";
        cout << "3. Gestion de memoria (PILA)\n";
        cout << "4. Salir\n";
        cout << "Seleccione una opcion: ";
        if (!(cin >> opcion)) {
            limpiarBuffer();
            opcion = 0;
        }
        limpiarBuffer();

        switch (opcion) {
        case 1:
            menuLista();
            break;
        case 2:
            menuCola();
            break;
        case 3:
            menuPila();
            break;
        case 4:
            cout << "Programa finalizado.\n";
            break;
        default:
            cout << "Opción inválida, intente de nuevo.\n";
        }

    } while (opcion != 4);

    return 0;
}
