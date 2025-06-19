# Instrucciones de Compilación y Ejecución

## Archivos Incluidos

- `secuencial.c` : Código para el cálculo secuencial de la pseudoinversa.
- `paralelo.c` : Código para el cálculo paralelo de la pseudoinversa con OpenMP.
- `metricas.c` : Script para calcular speedup y eficiencia.
- `entrada.ent` : Archivo de entrada de prueba.
- `entrada1.ent` : Archivo de entrada adicional de mayor tamaño.
- `salida.sal` : Resultado generado por el código secuencial y paralelo.
- `metricas.met` : Resultados de las métricas de ejecución.

---

## Requisitos

- Tener instalado `gcc` con soporte para OpenMP.

### En macOS

Apple Clang **no incluye OpenMP por defecto**. Se recomienda instalar `libomp` y `llvm` mediante Homebrew:

```bash
brew install llvm
```

Luego agregar a tu perfil (`.zshrc` o `.bash_profile`):

```bash
export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
export LDFLAGS="-L/opt/homebrew/opt/llvm/lib"
export CPPFLAGS="-I/opt/homebrew/opt/llvm/include"
export CC=/opt/llvm/bin/clang
export CXX=/opt/llvm/bin/clang++
alias clang-omp='clang -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include -L/opt/homebrew/opt/libomp/lib -lomp -isysroot $(xcrun --show-sdk-path) -Wno-unused-command-line-argument'
```

### En Linux o Windows (WSL)

Asegúrese de tener instalado `gcc` con soporte para OpenMP:

```bash
gcc --version
```

---

## Compilación

### En macOS:

1. **Compilar código secuencial**

```bash
gcc secuencial.c -o secuencial -lm
```

2. **Compilar código paralelo**

```bash
clang-omp paralelo.c -o paralelo -fopenmp -lm
```

   O si usas LLVM instalado manualmente:

```bash
clang-omp paralelo.c -o paralelo -fopenmp -lm
```

1. **Compilar script de métricas**

```bash
clang-omp metricas.c -o metricas -fopenmp -lm
```

---

### En Linux o Windows (WSL):

1. **Compilar código secuencial**

```bash
gcc secuencial.c -o secuencial -lm
```

1. **Compilar código paralelo**

```bash
gcc -fopenmp paralelo.c -o paralelo -lm
```

1. **Compilar script de métricas**

```bash
gcc -fopenmp metricas.c -o metricas -lm
```

---

## Ejecución

- **Ejecutar generación de métricas:**

```bash
./metricas entrada.ent
```

- **(Opcional) Ejecutar cálculo secuencial manualmente:**

```bash
./secuencial entrada.ent
```

- **(Opcional) Ejecutar cálculo paralelo con N hilos (ejemplo: 4):**

```bash
OMP_NUM_THREADS=4 ./paralelo entrada.ent
```

---

**Trabajo realizado por: Tomás Poblete y Cristóbal Pérez**
