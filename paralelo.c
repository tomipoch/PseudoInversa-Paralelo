#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>  // Biblioteca para paralelismo en OpenMP

#define MAX 100        // Tamaño máximo de la matriz
#define TOL 1e-6       // Tolerancia para detección de pivotes casi nulos

// Lee la matriz A desde archivo y almacena dimensiones m (filas) y n (columnas)
void leer_matriz(FILE *archivo, double A[MAX][MAX], int *m, int *n) {
    fscanf(archivo, "%d %d", m, n);
    for (int i = 0; i < *m; i++)
        for (int j = 0; j < *n; j++)
            fscanf(archivo, "%lf", &A[i][j]);
}

// Calcula la transpuesta de A y guarda el resultado en At
// Uso de paralelismo colapsando dos bucles (mejor aprovechamiento de hilos)
void transpuesta(double A[MAX][MAX], double At[MAX][MAX], int m, int n) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            At[j][i] = A[i][j];
}

// Multiplica dos matrices: A (r×c) y B (c×p), resultado en R (r×p)
// El paralelismo se aplica sobre las filas de R
void multiplicar(double A[MAX][MAX], double B[MAX][MAX], double R[MAX][MAX], int r, int c, int p) {
    #pragma omp parallel for
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < p; j++) {
            double suma = 0.0;
            for (int k = 0; k < c; k++)
                suma += A[i][k] * B[k][j];
            R[i][j] = suma;
        }
    }
}

// Inversión de matriz cuadrada por método de Gauss-Jordan
// No paralelizado por razones de estabilidad y dependencia de datos
int invertir(double A[MAX][MAX], double inv[MAX][MAX], int n) {
    double temp;

    // Inicializa la matriz identidad
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            inv[i][j] = (i == j) ? 1.0 : 0.0;

    // Eliminación hacia adelante y atrás
    for (int i = 0; i < n; i++) {
        if (fabs(A[i][i]) < TOL) return 0;  // Si el pivote es muy pequeño, no se puede invertir

        temp = A[i][i];
        for (int j = 0; j < n; j++) {
            A[i][j] /= temp;
            inv[i][j] /= temp;
        }

        for (int k = 0; k < n; k++) {
            if (k == i) continue;
            temp = A[k][i];
            for (int j = 0; j < n; j++) {
                A[k][j] -= A[i][j] * temp;
                inv[k][j] -= inv[i][j] * temp;
            }
        }
    }
    return 1;
}

// Escribe en archivo el tipo de pseudoinversa ('R' o 'L') y la matriz resultante
void escribir_salida(FILE *archivo, char tipo, double R[MAX][MAX], int rows, int cols) {
    fprintf(archivo, "%c\n", tipo);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(archivo, "%.6lf", R[i][j]);
            if (j < cols - 1) fprintf(archivo, " ");
        }
        fprintf(archivo, "\n");
    }
}

int main(int argc, char *argv[]) {
    const char *archivo_entrada = (argc > 1) ? argv[1] : "entrada.ent";

    FILE *entrada = fopen(archivo_entrada, "r");
    FILE *salida = fopen("salida.sal", "w");  // Se escribe en salida.sal (igual que en secuencial)

    if (!entrada || !salida) {
        fprintf(stderr, "Error abriendo archivos.\n");
        return 1;
    }

    int m, n;
    double A[MAX][MAX], At[MAX][MAX], AA[MAX][MAX], AAt[MAX][MAX];
    double Inv[MAX][MAX], Resultado[MAX][MAX];

    leer_matriz(entrada, A, &m, &n);       // Lectura de la matriz A
    transpuesta(A, At, m, n);              // Cálculo de transpuesta

    if (m <= n) {
        // Cálculo de pseudoinversa derecha: A^+ = A^T * (A * A^T)^-1
        multiplicar(A, At, AAt, m, n, m);
        if (!invertir(AAt, Inv, m)) {
            fprintf(salida, "-1\n");       // No se pudo invertir, pseudoinversa no existe
            fclose(entrada);
            fclose(salida);
            return 0;
        }
        multiplicar(At, Inv, Resultado, n, m, m);
        escribir_salida(salida, 'R', Resultado, n, m);
    } else {
        // Cálculo de pseudoinversa izquierda: A^+ = (A^T * A)^-1 * A^T
        multiplicar(At, A, AA, n, m, n);
        if (!invertir(AA, Inv, n)) {
            fprintf(salida, "-1\n");
            fclose(entrada);
            fclose(salida);
            return 0;
        }
        multiplicar(Inv, At, Resultado, n, n, m);
        escribir_salida(salida, 'L', Resultado, n, m);
    }

    fclose(entrada);
    fclose(salida);
    return 0;
}