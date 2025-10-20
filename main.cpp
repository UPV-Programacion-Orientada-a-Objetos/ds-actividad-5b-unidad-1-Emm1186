#include <iostream>
#include <iomanip> 
// clase base abstrakta
// Esta clase define la interfaz general para cualquier matriz.
// Usa templates para aceptar cualquier tipo numérico (int, float, double, etc.)
// Tiene métodos puros virtuales, lo que la hace una clase abstracta.

template <typename T>
class MatrizBase {
protected:
    int _filas;     // Número de filas
    int _columnas;  // Número de columnas

public:
    // Constructor base
    MatrizBase(int filas = 0, int columnas = 0) : _filas(filas), _columnas(columnas) {}

    // Destructor virtual (importante para herencia)
    virtual ~MatrizBase() {}

    // Getters simples
    int filas() const { return _filas; }
    int columnas() const { return _columnas; }

    // Métodos puros virtuales (las clases hijas deben implementarlos)
    virtual void cargarValores() = 0; // Pide los valores al usuario
    virtual T obtener(int i, int j) const = 0; // Retorna un valor en una posición
    virtual void asignar(int i, int j, T val) = 0; // Asigna un valor en una posición
    virtual MatrizBase<T>* crearNueva(int filas, int columnas) const = 0; // Crea una nueva matriz del mismo tipo
    virtual MatrizBase<T>* sumar(const MatrizBase<T>& otra) const = 0; // Suma con otra matriz (polimórfico)
    virtual void imprimir() const = 0; // Muestra la matriz

    // Sobrecarga del operador + (usa la función virtual "sumar")
    MatrizBase<T>* operator+(const MatrizBase<T>& otra) const {
        return this->sumar(otra);
    }
};


// clase derivada: MatrizDinamica
// Implementa una matriz con memoria dinámica usando punteros dobles (T**).
// Esta clase aplica la "Regla de los 5" (manejo manual de memoria).

template <typename T>
class MatrizDinamica : public MatrizBase<T> {
private:
    T **_datos; // Arreglo bidimensional dinámico (puntero doble)

    // Reserva memoria para la matriz
    void reservar(int filas, int columnas) {
        if (filas <= 0 || columnas <= 0) {
            _datos = nullptr;
            return;
        }
        _datos = new T*[filas];
        for (int i = 0; i < filas; ++i)
            _datos[i] = new T[columnas];
    }

    // Libera la memoria reservada
    void liberar() {
        if (!_datos) return;
        for (int i = 0; i < this->_filas; ++i)
            delete[] _datos[i];
        delete[] _datos;
        _datos = nullptr;
    }

    // Copia profunda desde otra matriz
    void copiarDesde(const MatrizDinamica<T>& otro) {
        this->_filas = otro._filas;
        this->_columnas = otro._columnas;
        if (otro._datos == nullptr) {
            _datos = nullptr;
            return;
        }
        reservar(this->_filas, this->_columnas);
        for (int i = 0; i < this->_filas; ++i)
            for (int j = 0; j < this->_columnas; ++j)
                _datos[i][j] = otro._datos[i][j];
    }

public:
    // Constructor normal
    MatrizDinamica(int filas, int columnas) : MatrizBase<T>(filas, columnas), _datos(nullptr) {
        reservar(filas, columnas);
        // Inicializa todos los valores a cero
        for (int i = 0; i < filas; ++i)
            for (int j = 0; j < columnas; ++j)
                _datos[i][j] = T();
    }

    // Destructor (libera memoria)
    ~MatrizDinamica() override {
        liberar();
        //cout << "Destructor MatrizDinamica\n";
    }

    // Constructor de copia (Regla de los cinco)
    MatrizDinamica(const MatrizDinamica<T>& otro) : MatrizBase<T>(0,0), _datos(nullptr) {
        copiarDesde(otro);
    }

    // Operador de asignación por copia (deep copy)
    MatrizDinamica<T>& operator=(const MatrizDinamica<T>& otro) {
        if (this == &otro) return *this;
        liberar();
        copiarDesde(otro);
        return *this;
    }

    // Constructor de movimiento
    MatrizDinamica(MatrizDinamica<T>&& otro) noexcept
        : MatrizBase<T>(otro._filas, otro._columnas), _datos(otro._datos) {
        // El "otro" pierde su memoria
        otro._datos = nullptr;
        otro._filas = 0;
        otro._columnas = 0;
    }

    // Operador de asignación por movimiento
    MatrizDinamica<T>& operator=(MatrizDinamica<T>&& otro) noexcept {
        if (this == &otro) return *this;
        liberar();
        this->_filas = otro._filas;
        this->_columnas = otro._columnas;
        _datos = otro._datos;
        otro._datos = nullptr;
        otro._filas = 0;
        otro._columnas = 0;
        return *this;
    }

    // Obtiene un valor
    T obtener(int i, int j) const override {
        return _datos[i][j];
    }

    // Asigna un valor
    void asignar(int i, int j, T val) override {
        _datos[i][j] = val;
    }

    // Crea una nueva matriz dinámica del mismo tipo
    MatrizBase<T>* crearNueva(int filas, int columnas) const override {
        return new MatrizDinamica<T>(filas, columnas);
    }

    // Implementa la suma polimórfica
    MatrizBase<T>* sumar(const MatrizBase<T>& otra) const override {
        // Verifica dimensiones
        if (this->_filas != otra.filas() || this->_columnas != otra.columnas()) {
            std::cerr << "Error: dimensiones incompatibles para suma.\n";
            return nullptr;
        }
        // Crea una nueva matriz del mismo tipo que *this
        MatrizBase<T>* resultado = this->crearNueva(this->_filas, this->_columnas);
        // Realiza la suma elemento a elemento
        for (int i = 0; i < this->_filas; ++i)
            for (int j = 0; j < this->_columnas; ++j)
                resultado->asignar(i, j, this->obtener(i, j) + otra.obtener(i, j));

        return resultado;
    }

    // Pide los valores por consola
    void cargarValores() override {
        std::cout << "Cargando valores para MatrizDinamica (" << this->_filas << "x" << this->_columnas << ")\n";
        for (int i = 0; i < this->_filas; ++i)
            for (int j = 0; j < this->_columnas; ++j) {
                T val;
                std::cout << "valor[" << i << "][" << j << "] = ";
                std::cin >> val;
                asignar(i, j, val);
            }
    }

    // Muestra la matriz en consola
    void imprimir() const override {
        std::cout << "MatrizDinamica (" << this->_filas << "x" << this->_columnas << ")\n";
        for (int i = 0; i < this->_filas; ++i) {
            std::cout << "| ";
            for (int j = 0; j < this->_columnas; ++j) {
                std::cout << std::fixed << std::setprecision(2) << this->obtener(i,j);
                if (j < this->_columnas - 1) std::cout << " | ";
            }
            std::cout << " |\n";
        }
    }
};


// clase derivada: MatrizEstatica
// Usa arreglos nativos fijos (T _datos[M][N]).
// No requiere liberar memoria, el tamaño se conoce en compilación.

template <typename T, int M, int N>
class MatrizEstatica : public MatrizBase<T> {
private:
    T _datos[M][N]; // Arreglo fijo en memoria

public:
    // Constructor
    MatrizEstatica() : MatrizBase<T>(M, N) {
        // Inicializa todo a cero
        for (int i = 0; i < M; ++i)
            for (int j = 0; j < N; ++j)
                _datos[i][j] = T();
    }

    // Obtiene un valor
    T obtener(int i, int j) const override {
        return _datos[i][j];
    }

    // Asigna un valor
    void asignar(int i, int j, T val) override {
        _datos[i][j] = val;
    }

    // Crea una nueva matriz del mismo tamaño (M,N)
    MatrizBase<T>* crearNueva(int filas, int columnas) const override {
        if (filas != M || columnas != N) {
            std::cerr << "Error: dimensiones incorrectas para MatrizEstatica.\n";
            return nullptr;
        }
        return new MatrizEstatica<T,M,N>();
    }

    // Implementa la suma
    MatrizBase<T>* sumar(const MatrizBase<T>& otra) const override {
        if (this->_filas != otra.filas() || this->_columnas != otra.columnas()) {
            std::cerr << "Error: dimensiones incompatibles para suma.\n";
            return nullptr;
        }
        MatrizBase<T>* resultado = this->crearNueva(this->_filas, this->_columnas);
        if (!resultado) return nullptr;
        for (int i = 0; i < this->_filas; ++i)
            for (int j = 0; j < this->_columnas; ++j)
                resultado->asignar(i,j, this->obtener(i,j) + otra.obtener(i,j));
        return resultado;
    }

    // Carga de valores (desde consola)
    void cargarValores() override {
        std::cout << "Cargando valores para MatrizEstatica (" << this->_filas << "x" << this->_columnas << ")\n";
        for (int i = 0; i < this->_filas; ++i)
            for (int j = 0; j < this->_columnas; ++j) {
                T val;
                std::cout << "valor[" << i << "][" << j << "] = ";
                std::cin >> val;
                asignar(i, j, val);
            }
    }

    // Imprime los valores
    void imprimir() const override {
        std::cout << "MatrizEstatica (" << this->_filas << "x" << this->_columnas << ")\n";
        for (int i = 0; i < this->_filas; ++i) {
            std::cout << "| ";
            for (int j = 0; j < this->_columnas; ++j) {
                std::cout << std::fixed << std::setprecision(2) << this->obtener(i,j);
                if (j < this->_columnas - 1) std::cout << " | ";
            }
            std::cout << " |\n";
        }
    }
};


// el mein)
// Demuestra el uso de las clases con polimorfismo y genericidad.
// Se crean matrices dinámicas y estáticas, se suman y se imprime el resultado.

int main() {
    std::cout << "--- Sistema genérico de Álgebra Lineal (AVANCE) ---\n\n";

    // ---------------- DEMOSTRACIÓN CON FLOAT ----------------
    std::cout << ">> Demostración (tipo float) <<\n\n";

    // Crea una matriz dinámica A (3x2)
    MatrizBase<float>* A = new MatrizDinamica<float>(3,2);
    // Asigna valores manualmente (podrías usar cargarValores())
    A->asignar(0,0,1.5f); A->asignar(0,1,2.0f);
    A->asignar(1,0,0.0f); A->asignar(1,1,1.0f);
    A->asignar(2,0,4.5f); A->asignar(2,1,3.0f);
    std::cout << "A = \n"; A->imprimir(); std::cout << "\n";

    // Crea una matriz estática B (3x2)
    MatrizBase<float>* B = new MatrizEstatica<float,3,2>();
    B->asignar(0,0,0.5f); B->asignar(0,1,1.0f);
    B->asignar(1,0,2.0f); B->asignar(1,1,3.0f);
    B->asignar(2,0,1.0f); B->asignar(2,1,1.0f);
    std::cout << "B = \n"; B->imprimir(); std::cout << "\n";

    // Suma polimórfica (usa el método virtual)
    std::cout << "SUMANDO: C = A + B ...\n";
    MatrizBase<float>* C = (*A) + (*B);
    if (C) {
        std::cout << "Resultado C = \n";
        C->imprimir();
        std::cout << "\n";
    }

    // Libera la memoria de los objetos dinámicos
    delete C;
    std::cout << "Destructor de C llamado.\n";
    delete A;
    std::cout << "Destructor de A llamado.\n";
    delete B;
    std::cout << "Destructor de B llamado.\n\n";


    // DEMOSTRACIÓN CON INT 
    std::cout << ">> Demostración (tipo int) <<\n\n";

    MatrizBase<int>* D = new MatrizDinamica<int>(2,2);
    MatrizBase<int>* E = new MatrizDinamica<int>(2,2);

    // Asignamos valores a D y E
    D->asignar(0,0,1); D->asignar(0,1,2);
    D->asignar(1,0,3); D->asignar(1,1,4);

    E->asignar(0,0,10); E->asignar(0,1,20);
    E->asignar(1,0,30); E->asignar(1,1,40);

    std::cout << "D = \n"; D->imprimir(); std::cout << "\n";
    std::cout << "E = \n"; E->imprimir(); std::cout << "\n";

    // Realiza la suma (D + E)
    MatrizBase<int>* F = (*D) + (*E);
    if (F) {
        std::cout << "F = D + E = \n"; F->imprimir(); std::cout << "\n";
    }

    // Libera memoria
    delete F; delete D; delete E;

    std::cout << "Sistema cerrado correctamente.\n";
    return 0;
}
