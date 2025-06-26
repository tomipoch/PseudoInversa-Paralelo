/*
 * Autores: Tomas Poblete Chamorro, Cristobal Perez Ramirez
 * Carrera: Ingeniería Civil Informática
 * Fecha: 10 de Junio de 2025
 * Propósito: Cálculo Paralelo
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define MAX 100
#define TOL 1e-9

void leer_matriz(FILE *archivo, double A[MAX][MAX], int *m, int *n) {
    fscanf(archivo, "%d %d", m, n);
    for (int i = 0; i < *m; i++)
        for (int j = 0; j < *n; j++)
            fscanf(archivo, "%lf", &A[i][j]);
}

// Transpuesta paralela
void transpuesta(double A[MAX][MAX], double At[MAX][MAX], int m, int n) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            At[j][i] = A[i][j];
}

// Multiplicación paralela
void multiplicar(double A[MAX][MAX], double B[MAX][MAX], double R[MAX][MAX], int r, int c, int p) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < r; i++)
        for (int j = 0; j < p; j++) {
            double suma = 0.0;
            #pragma omp simd reduction(+:suma)
            for (int k = 0; k < c; k++)
                suma += A[i][k] * B[k][j];
            R[i][j] = suma;
        }
}

// Resolver Ax = b por Gauss simple y sustitución atrás (no paralelizado, depende de datos)
int resolver_sistema(double A[MAX][MAX], double b[MAX], double x[MAX], int n) {
    for (int k = 0; k < n - 1; k++) {
        if (fabs(A[k][k]) < TOL) return 0;
        for (int i = k + 1; i < n; i++) {
            double m = A[i][k] / A[k][k];
            for (int j = k; j < n; j++)
                A[i][j] -= m * A[k][j];
            b[i] -= m * b[k];
        }
    }
    for (int i = n - 1; i >= 0; i--) {
        if (fabs(A[i][i]) < TOL) return 0;
        x[i] = b[i];
        for (int j = i + 1; j < n; j++)
            x[i] -= A[i][j] * x[j];
        x[i] /= A[i][i];
    }
    return 1;
}

// Inversión paralela: Cada columna de la inversa es un sistema independiente
int invertir(double A[MAX][MAX], double inv[MAX][MAX], int n) {
    int exito = 1;
    #pragma omp parallel for
    for (int col = 0; col < n; col++) {
        double tmpA[MAX][MAX], b[MAX], x[MAX];
        // Copiar A a tmpA (cada hilo su copia)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                tmpA[i][j] = A[i][j];
        for (int i = 0; i < n; i++)
            b[i] = (i == col) ? 1.0 : 0.0;
        // Resolver el sistema
        if (!resolver_sistema(tmpA, b, x, n)) {
            exito = 0;
        }
        // Guardar columna si tuvo éxito
        for (int i = 0; i < n; i++)
            inv[i][col] = x[i];
    }
    return exito;
}

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
    FILE *salida = fopen("salida.sal", "w");
    if (!entrada || !salida) {
        fprintf(stderr, "Error abriendo archivos.\n");
        return 1;
    }

    int m, n;
    double A[MAX][MAX], At[MAX][MAX], AA[MAX][MAX], AAt[MAX][MAX];
    double Inv[MAX][MAX], Resultado[MAX][MAX];

    leer_matriz(entrada, A, &m, &n);
    transpuesta(A, At, m, n);

    if (m <= n) {
        // Pseudoinversa derecha: R = A^T * (A * A^T)^-1
        multiplicar(A, At, AAt, m, n, m);
        if (!invertir(AAt, Inv, m)) {
            fprintf(salida, "-1\n");
            fclose(entrada); fclose(salida);
            return 0;
        }
        multiplicar(At, Inv, Resultado, n, m, m);
        escribir_salida(salida, 'R', Resultado, n, m);
    } else {
        // Pseudoinversa izquierda: L = (A^T * A)^-1 * A^T
        multiplicar(At, A, AA, n, m, n);
        if (!invertir(AA, Inv, n)) {
            fprintf(salida, "-1\n");
            fclose(entrada); fclose(salida);
            return 0;
        }
        multiplicar(Inv, At, Resultado, n, n, m);
        escribir_salida(salida, 'L', Resultado, n, m);
    }

    fclose(entrada);
    fclose(salida);
    return 0;
}