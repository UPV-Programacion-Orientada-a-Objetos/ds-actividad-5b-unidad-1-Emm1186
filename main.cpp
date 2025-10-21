#include <iostream>

// clase base abstracta (inyerfaz polimoorfica)
// Esta clase define el comportamiento comun para cualquier tipo de matriz
// (ya sea dinamica o estatica). Usa un tipo generico T (int, float, etc.)
// y obliga a las clases hijas a implementar los metodos virtuales puros.

template <typename T>
class MatrizBase {
protected:
    int _filas;
    int _columnas;

public:
    // Constructor base
    MatrizBase(int filas, int columnas) : _filas(filas), _columnas(columnas) {}

    // Metodos virtuales puros (deben ser implementados por las clases hijas)
    virtual void cargarValores() = 0;                          // Carga manual de datos
    virtual MatrizBase<T>* sumar(const MatrizBase<T>& otra) const = 0;  // Suma polimorfica
    virtual void imprimir() const = 0;                         // Mostrar matriz

    // Destructor virtual (para permitir destruccion polimorfica)
    virtual ~MatrizBase() {}
};

// CLASE MATRIZ DINAMICA (usa punteros y memoria manual)
// Aqui aplicamos la regla de los cinco y hacemos gestion de memoria manual.
// Esta clase representa matrices con tamano variable.

template <typename T>
class MatrizDinamica : public MatrizBase<T> {
private:
    T **_datos; // Puntero doble que apunta a filas y columnas dinamicas

public:
    // ---------- Constructor ----------
    MatrizDinamica(int filas, int columnas) : MatrizBase<T>(filas, columnas) {
        _datos = new T*[this->_filas];
        for (int i = 0; i < this->_filas; ++i) {
            _datos[i] = new T[this->_columnas];
        }
    }

    // ---------- Destructor ----------
    ~MatrizDinamica() {
        for (int i = 0; i < this->_filas; ++i) {
            delete[] _datos[i];
        }
        delete[] _datos;
        std::cout << "Liberando memoria de Matriz Dinamica..." << std::endl;
    }

    // ---------- Constructor de copia (deep copy) ----------
    MatrizDinamica(const MatrizDinamica& otra) : MatrizBase<T>(otra._filas, otra._columnas) {
        _datos = new T*[this->_filas];
        for (int i = 0; i < this->_filas; ++i) {
            _datos[i] = new T[this->_columnas];
            for (int j = 0; j < this->_columnas; ++j) {
                _datos[i][j] = otra._datos[i][j];
            }
        }
    }

    // ---------- Operador de asignacion (deep copy) ----------
    MatrizDinamica& operator=(const MatrizDinamica& otra) {
        if (this == &otra) return *this; // Evita autoasignacion

        for (int i = 0; i < this->_filas; ++i)
            delete[] _datos[i];
        delete[] _datos;

        this->_filas = otra._filas;
        this->_columnas = otra._columnas;

        _datos = new T*[this->_filas];
        for (int i = 0; i < this->_filas; ++i) {
            _datos[i] = new T[this->_columnas];
            for (int j = 0; j < this->_columnas; ++j) {
                _datos[i][j] = otra._datos[i][j];
            }
        }
        return *this;
    }

    // ---------- Cargar valores manualmente ----------
    void cargarValores() override {
        std::cout << "Cargando valores para una Matriz Dinamica de tamano "
                  << this->_filas << "x" << this->_columnas << std::endl;

        for (int i = 0; i < this->_filas; ++i) {
            for (int j = 0; j < this->_columnas; ++j) {
                std::cout << "Valor [" << i << "][" << j << "]: ";
                std::cin >> _datos[i][j];
            }
        }
    }

    // ---------- Mostrar matriz ----------
    void imprimir() const override {
        for (int i = 0; i < this->_filas; ++i) {
            std::cout << "| ";
            for (int j = 0; j < this->_columnas; ++j) {
                std::cout << _datos[i][j] << " | ";
            }
            std::cout << std::endl;
        }
    }

    // ---------- Sumar otra matriz ----------
    MatrizBase<T>* sumar(const MatrizBase<T>& otra) const override {
        const MatrizDinamica<T>* ptrOtra = dynamic_cast<const MatrizDinamica<T>*>(&otra);
        if (!ptrOtra) {
            std::cout << "Error: no se puede sumar (tipos incompatibles)" << std::endl;
            return nullptr;
        }

        if (this->_filas != ptrOtra->_filas || this->_columnas != ptrOtra->_columnas) {
            std::cout << "Error: dimensiones incompatibles para la suma" << std::endl;
            return nullptr;
        }

        MatrizDinamica<T>* resultado = new MatrizDinamica<T>(this->_filas, this->_columnas);

        for (int i = 0; i < this->_filas; ++i) {
            for (int j = 0; j < this->_columnas; ++j) {
                resultado->_datos[i][j] = this->_datos[i][j] + ptrOtra->_datos[i][j];
            }
        }
        return resultado;
    }

    // ---------- Sobrecarga del operador + ----------
    MatrizDinamica<T> operator+(const MatrizDinamica<T>& otra) const {
        MatrizDinamica<T>* resultado = dynamic_cast<MatrizDinamica<T>*>(this->sumar(otra));
        return *resultado;
    }
};

// clase matriz estatica (usa arreglos fijos)

template <typename T, int M, int N>
class MatrizEstatica : public MatrizBase<T> {
private:
    T _datos[M][N]; // Arreglo fijo

public:
    MatrizEstatica() : MatrizBase<T>(M, N) {}

    void cargarValores() override {
        std::cout << "Cargando valores para una Matriz Estatica de tamano "
                  << M << "x" << N << std::endl;
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                std::cout << "Valor [" << i << "][" << j << "]: ";
                std::cin >> _datos[i][j];
            }
        }
    }

    void imprimir() const override {
        for (int i = 0; i < M; ++i) {
            std::cout << "| ";
            for (int j = 0; j < N; ++j) {
                std::cout << _datos[i][j] << " | ";
            }
            std::cout << std::endl;
        }
    }

    MatrizBase<T>* sumar(const MatrizBase<T>& otra) const override {
        const MatrizEstatica<T, M, N>* ptrOtra = dynamic_cast<const MatrizEstatica<T, M, N>*>(&otra);
        if (!ptrOtra) {
            std::cout << "Error: tipos incompatibles en la suma" << std::endl;
            return nullptr;
        }

        MatrizEstatica<T, M, N>* resultado = new MatrizEstatica<T, M, N>();
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                resultado->_datos[i][j] = this->_datos[i][j] + ptrOtra->_datos[i][j];
            }
        }
        return resultado;
    }
};

// funciom principal

int main() {
    std::cout << "--- Sistema Generico de Algebra Lineal ---" << std::endl;

    // 1. Matrices dinamicas de tipo float
    std::cout << "\n>> Matriz Dinamica A (3x2)" << std::endl;
    MatrizBase<float>* A = new MatrizDinamica<float>(3, 2);
    A->cargarValores();

    std::cout << "\n>> Matriz Dinamica B (3x2)" << std::endl;
    MatrizBase<float>* B = new MatrizDinamica<float>(3, 2);
    B->cargarValores();

    // 2. Suma polimorfica
    std::cout << "\n>> SUMANDO A + B ..." << std::endl;
    MatrizBase<float>* C = A->sumar(*B);

    std::cout << "\n>> Resultado C (3x2):" << std::endl;
    C->imprimir();

    // 3. Liberamos la maquina malandrística (memoria)
    delete A;
    delete B;
    delete C;

    std::cout << "\nSistema cerrado correctamente." << std::endl;
    return 0;
}
