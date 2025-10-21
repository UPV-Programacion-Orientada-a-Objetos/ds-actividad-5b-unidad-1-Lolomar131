#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>

template <typename T>
class MatrizBase;

template <typename T>
class MatrizDinamica;

template <typename T, int M, int N>
class MatrizEstatica; 

template <typename T>
MatrizBase<T>* operator+(const MatrizBase<T>& A, const MatrizBase<T>& B);


template <typename T>
class MatrizBase {
protected:
    int _filas;
    int _columnas;

public:
    MatrizBase(int f, int c) : _filas(f), _columnas(c) {}
    virtual ~MatrizBase() = default;

    virtual void cargarValores() = 0;
    virtual MatrizBase<T>* sumar(const MatrizBase<T>& otra) const = 0;
    virtual void imprimir() const = 0;

    template <typename U>
    friend MatrizBase<U>* operator+(const MatrizBase<U>& A, const MatrizBase<U>& B);
    
    int getFilas() const { return _filas; }
    int getColumnas() const { return _columnas; }
};


template <typename T>
class MatrizDinamica : public MatrizBase<T> {
protected:
    T **_datos;

private:
    void deepCopy(const MatrizDinamica<T>& otra) {
        this->_filas = otra._filas;
        this->_columnas = otra._columnas;
        _datos = new T*[this->_filas];

        for (int i = 0; i < this->_filas; ++i) {
            _datos[i] = new T[this->_columnas];
            for (int j = 0; j < this->_columnas; ++j) {
                _datos[i][j] = otra._datos[i][j];
            }
        }
    }
    
    void freeMemory() {
        if (_datos) {
            for (int i = 0; i < this->_filas; ++i) {
                delete[] _datos[i];
            }
            delete[] _datos;
            _datos = nullptr;
        }
    }

public:
    template <typename U, int M, int N>
    friend class MatrizEstatica;
    
    MatrizDinamica(int f, int c) : MatrizBase<T>(f, c), _datos(nullptr) {
        if (f > 0 && c > 0) {
            _datos = new T*[this->_filas];
            for (int i = 0; i < this->_filas; ++i) {
                _datos[i] = new T[this->_columnas]();
            }
        } else {
            throw std::invalid_argument("Dimensiones de matriz deben ser positivas.");
        }
    }

    ~MatrizDinamica() override {
        std::cout << "  Liberando memoria de Matriz Dinámica (" << this->_filas << "x" << this->_columnas << ")..." << std::endl;
        freeMemory();
    }

    MatrizDinamica(const MatrizDinamica<T>& otra) : MatrizBase<T>(otra._filas, otra._columnas), _datos(nullptr) {
        std::cout << "  *Llamando al Constructor de Copia (Deep Copy)..." << std::endl;
        deepCopy(otra);
    }

    MatrizDinamica<T>& operator=(const MatrizDinamica<T>& otra) {
        std::cout << "  *Llamando al Operador de Asignación (Deep Copy)..." << std::endl;
        if (this != &otra) {
            freeMemory();
            deepCopy(otra);
        }
        return *this;
    }
    
   
    MatrizDinamica(MatrizDinamica<T>&& otra) noexcept : MatrizBase<T>(otra._filas, otra._columnas), _datos(otra._datos) {
        otra._datos = nullptr;
        otra._filas = 0;
        otra._columnas = 0;
    }

    
    MatrizDinamica<T>& operator=(MatrizDinamica<T>&& otra) noexcept {
        if (this != &otra) {
            freeMemory();
            this->_filas = otra._filas;
            this->_columnas = otra._columnas;
            _datos = otra._datos;
            otra._datos = nullptr;
            otra._filas = 0;
            otra._columnas = 0;
        }
        return *this;
    }
    
    void cargarValores() override {
        std::cout << "  Cargando valores manuales para Matriz Dinámica " << this->_filas << "x" << this->_columnas << std::endl;
        T valor = static_cast<T>(1.5);
        for (int i = 0; i < this->_filas; ++i) {
            for (int j = 0; j < this->_columnas; ++j) {
                _datos[i][j] = valor + static_cast<T>(j * 0.1);
                valor += static_cast<T>(0.5);
            }
        }
    }

    void imprimir() const override {
        for (int i = 0; i < this->_filas; ++i) {
            std::cout << "| ";
            for (int j = 0; j < this->_columnas; ++j) {
                std::cout << std::fixed << std::setprecision(1) << _datos[i][j] << " |";
            }
            std::cout << std::endl;
        }
    }
    
    MatrizBase<T>* sumar(const MatrizBase<T>& otra) const override;
};



template <typename T, int M, int N>
class MatrizEstatica : public MatrizBase<T> {
protected: 
    T _datos[M][N];

public:
   
    friend class MatrizDinamica<T>;
    
    MatrizEstatica() : MatrizBase<T>(M, N) {}
    ~MatrizEstatica() override {}

    void cargarValores() override {
        std::cout << "  Cargando valores por defecto para Matriz Estática " << M << "x" << N << std::endl;
        T valor = static_cast<T>(1.0);
        for (int i = 0; i < this->_filas; ++i) {
            for (int j = 0; j < this->_columnas; ++j) {
                _datos[i][j] = valor + static_cast<T>(i * 0.5);
                valor += static_cast<T>(0.1);
            }
        }
    }

    void imprimir() const override {
        for (int i = 0; i < this->_filas; ++i) {
            std::cout << "| ";
            for (int j = 0; j < this->_columnas; ++j) {
                std::cout << std::fixed << std::setprecision(1) << _datos[i][j] << " |";
            }
            std::cout << std::endl;
        }
    }
    
    MatrizBase<T>* sumar(const MatrizBase<T>& otra) const override;
};




template <typename T>
MatrizBase<T>* MatrizDinamica<T>::sumar(const MatrizBase<T>& otra) const {
    if (this->_filas != otra.getFilas() || this->_columnas != otra.getColumnas()) {
        throw std::invalid_argument("ERROR: Las matrices deben tener las mismas dimensiones para la suma.");
    }
    
    MatrizDinamica<T>* resultado = new MatrizDinamica<T>(this->_filas, this->_columnas);
    
    const MatrizDinamica<T>* otraDin = dynamic_cast<const MatrizDinamica<T>*>(&otra);
    if (otraDin) {
        for (int i = 0; i < this->_filas; ++i) {
            for (int j = 0; j < this->_columnas; ++j) {
                resultado->_datos[i][j] = this->_datos[i][j] + otraDin->_datos[i][j];
            }
        }
        return resultado;
    } 
    
    
    if (this->_filas == 3 && this->_columnas == 2) {
        const MatrizEstatica<T, 3, 2>* otraEst = dynamic_cast<const MatrizEstatica<T, 3, 2>*>(&otra);
        if (otraEst) {
            for (int i = 0; i < this->_filas; ++i) {
                for (int j = 0; j < this->_columnas; ++j) {
                    resultado->_datos[i][j] = this->_datos[i][j] + otraEst->_datos[i][j];
                }
            }
            return resultado;
        }
    } else if (this->_filas == 2 && this->_columnas == 2) {
        const MatrizEstatica<T, 2, 2>* otraEst = dynamic_cast<const MatrizEstatica<T, 2, 2>*>(&otra);
        if (otraEst) {
            for (int i = 0; i < this->_filas; ++i) {
                for (int j = 0; j < this->_columnas; ++j) {
                    resultado->_datos[i][j] = this->_datos[i][j] + otraEst->_datos[i][j];
                }
            }
            return resultado;
        }
    }

    delete resultado;
    throw std::runtime_error("ERROR: Tipo de matriz incompatible para la suma o dimensiones estáticas desconocidas.");
}

template <typename T, int M, int N>
MatrizBase<T>* MatrizEstatica<T, M, N>::sumar(const MatrizBase<T>& otra) const {
    if (this->_filas != otra.getFilas() || this->_columnas != otra.getColumnas()) {
        throw std::invalid_argument("ERROR: Las matrices deben tener las mismas dimensiones para la suma.");
    }
    
    MatrizDinamica<T>* resultado = new MatrizDinamica<T>(M, N);
    
 
    const MatrizEstatica<T, M, N>* otraEst = dynamic_cast<const MatrizEstatica<T, M, N>*>(&otra);
    if (otraEst) {
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                resultado->_datos[i][j] = this->_datos[i][j] + otraEst->_datos[i][j];
            }
        }
        return resultado;
    }
    
    const MatrizDinamica<T>* otraDin = dynamic_cast<const MatrizDinamica<T>*>(&otra);
    if (otraDin) {
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                resultado->_datos[i][j] = this->_datos[i][j] + otraDin->_datos[i][j];
            }
        }
        return resultado;
    }

    delete resultado;
    throw std::runtime_error("ERROR: Tipo de matriz incompatible para la suma.");
}

template <typename T>
MatrizBase<T>* operator+(const MatrizBase<T>& A, const MatrizBase<T>& B) {
    return A.sumar(B);
}



void demostracionFloat() {
    std::cout << "\n>> Demostración de Genericidad (Tipo FLOAT) <<\n" << std::endl;

    std::cout << "// 1. Creación de Matriz Dinámica (a través del puntero base)" << std::endl;
    MatrizBase<float>* A = nullptr;
    try {
        A = new MatrizDinamica<float>(3, 2);
        A->cargarValores();
        std::cout << "Creando Matriz Dinámica A (3x2)..." << std::endl;
        std::cout << "A =" << std::endl;
        A->imprimir();
    } catch (const std::exception& e) {
        std::cerr << "Error al crear Matriz Dinámica A: " << e.what() << std::endl;
    }

    std::cout << "\n// 2. Creación de Matriz Estática (a través del puntero base)" << std::endl;
    MatrizBase<float>* B = nullptr;
    try {
        B = new MatrizEstatica<float, 3, 2>(); 
        B->cargarValores();
        std::cout << "Creando Matriz Estática B (3x2)..." << std::endl;
        std::cout << "B =" << std::endl;
        B->imprimir();
    } catch (const std::exception& e) {
        std::cerr << "Error al crear Matriz Estática B: " << e.what() << std::endl;
    }

    std::cout << "\n// 3. Operación Polimórfica (Suma con Operador +)" << std::endl;
    MatrizBase<float>* C = nullptr;
    if (A && B) {
        try {
            std::cout << "SUMANDO: Matriz C = A + B ..." << std::endl;
            C = *A + *B; 
            
            std::cout << "Matriz Resultado C (" << C->getFilas() << "x" << C->getColumnas() << ", Tipo FLOAT):" << std::endl;
            C->imprimir();
        } catch (const std::exception& e) {
            std::cerr << "Error en la suma: " << e.what() << std::endl;
        }
    }

    std::cout << "\n>> Demostración de Limpieza de Memoria (Destructor Virtual) <<" << std::endl;
    
    if (C) {
        std::cout << "Llamando al destructor de C..." << std::endl;
        delete C;
    }
    if (B) {
        std::cout << "Llamando al destructor de B..." << std::endl;
        delete B;
    }
    if (A) {
        std::cout << "Llamando al destructor de A..." << std::endl;
        delete A;
    }
}

void demostracionInt() {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "\n>> Demostración de Genericidad (Tipo INT) <<\n" << std::endl;

    MatrizBase<int>* A_int = new MatrizDinamica<int>(2, 2);
    MatrizBase<int>* B_int = new MatrizEstatica<int, 2, 2>();
    
    A_int->cargarValores();
    B_int->cargarValores();
    
    std::cout << "Matriz A (INT):" << std::endl;
    A_int->imprimir();
    std::cout << "Matriz B (INT):" << std::endl;
    B_int->imprimir();

    MatrizBase<int>* C_int = nullptr;
    try {
        C_int = *A_int + *B_int;
        std::cout << "\nResultado C = A + B (INT):" << std::endl;
        C_int->imprimir();
    } catch (const std::exception& e) {
        std::cerr << "Error en suma INT: " << e.what() << std::endl;
    }
    
    std::cout << "\n>> Limpieza de Memoria para INT <<" << std::endl;
    if (C_int) delete C_int;
    delete B_int;
    delete A_int;
}

int main() {
    std::cout << "--- Sistema genérico de Álgebra Lineal ---\n";
    
    demostracionFloat();
    demostracionInt();

    std::cout << "\nSistema cerrado." << std::endl;
    return 0;
}