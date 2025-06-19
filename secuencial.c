#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX 100          // Tamaño máximo permitido para filas y columnas
#define TOL 1e-6         // Tolerancia para detección de pivotes nulos (evita divisiones por cero)

// Función que lee una matriz desde un archivo dado
void leer_matriz(FILE *archivo, double A[MAX][MAX], int *m, int *n) {
    fscanf(archivo, "%d %d", m, n);  // Lectura de dimensiones m (filas), n (columnas)
    for (int i = 0; i < *m; i++)
        for (int j = 0; j < *n; j++)
            fscanf(archivo, "%lf", &A[i][j]);  // Lectura de elementos
}

// Calcula la transpuesta de la matriz A y la almacena en At
void transpuesta(double A[MAX][MAX], double At[MAX][MAX], int m, int n) {
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            At[j][i] = A[i][j];  // Intercambio de filas por columnas
}

// Multiplica dos matrices A (r×c) y B (c×p), guarda el resultado en R (r×p)
void multiplicar(double A[MAX][MAX], double B[MAX][MAX], double R[MAX][MAX], int r, int c, int p) {
    for (int i = 0; i < r; i++)
        for (int j = 0; j < p; j++) {
            R[i][j] = 0.0;
            for (int k = 0; k < c; k++)
                R[i][j] += A[i][k] * B[k][j];
        }
}

// Realiza la inversión de una matriz cuadrada A usando eliminación Gauss-Jordan
int invertir(double A[MAX][MAX], double inv[MAX][MAX], int n) {
    double temp;

    // Inicializa matriz identidad en 'inv'
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            inv[i][j] = (i == j) ? 1.0 : 0.0;

    // Proceso de Gauss-Jordan para obtener la inversa
    for (int i = 0; i < n; i++) {
        if (fabs(A[i][i]) < TOL) return 0;  // Falla si el pivote es cercano a cero

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
    return 1;  // Éxito
}

// Escribe la matriz resultado en el archivo de salida con tipo 'R' o 'L'
void escribir_salida(FILE *archivo, char tipo, double R[MAX][MAX], int rows, int cols) {
    fprintf(archivo, "%c\n", tipo);  // Encabezado con tipo de pseudoinversa
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(archivo, "%.6lf", R[i][j]);  // Seis decimales según requerimiento
            if (j < cols - 1) fprintf(archivo, " ");
        }
        fprintf(archivo, "\n");
    }
}

int main(int argc, char *argv[]) {
    const char *archivo_entrada = (argc > 1) ? argv[1] : "entrada.ent";  // Usa argumento o archivo por defecto

    FILE *entrada = fopen(archivo_entrada, "r");
    FILE *salida = fopen("salida.sal", "w");
    if (!entrada || !salida) {
        printf("Error abriendo archivos.\n");
        return 1;
    }

    int m, n;
    double A[MAX][MAX], At[MAX][MAX], AA[MAX][MAX], AAt[MAX][MAX];
    double Inv[MAX][MAX], Resultado[MAX][MAX];

    leer_matriz(entrada, A, &m, &n);     // Carga matriz A desde archivo
    transpuesta(A, At, m, n);           // Calcula A^T

    if (m <= n) {
        // Pseudoinversa derecha: A^+ = A^T * (A * A^T)^-1
        multiplicar(A, At, AAt, m, n, m);
        if (!invertir(AAt, Inv, m)) {
            fprintf(salida, "-1\n");    // Matriz no invertible
            return 0;
        }
        multiplicar(At, Inv, Resultado, n, m, m);
        escribir_salida(salida, 'R', Resultado, n, m);
    } else {
        // Pseudoinversa izquierda: A^+ = (A^T * A)^-1 * A^T
        multiplicar(At, A, AA, n, m, n);
        if (!invertir(AA, Inv, n)) {
            fprintf(salida, "-1\n");    // Matriz no invertible
            return 0;
        }
        multiplicar(Inv, At, Resultado, n, n, m);
        escribir_salida(salida, 'L', Resultado, n, m);
    }

    fclose(entrada);
    fclose(salida);
    return 0;
}