#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX 100
#define EPSILON 1e-8

int m, n;
double A[MAX][MAX];
double At[MAX][MAX];

// Lee la matriz desde el archivo de entrada
void leerEntrada(const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "r");
    if (!archivo) {
        fprintf(stderr, "Error: No se pudo abrir el archivo de entrada\n");
        exit(EXIT_FAILURE);
    }
    fscanf(archivo, "%d %d", &m, &n);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            fscanf(archivo, "%lf", &A[i][j]);
    fclose(archivo);
}

void transponer(double origen[MAX][MAX], double destino[MAX][MAX], int filas, int columnas) {
    for (int i = 0; i < filas; i++)
        for (int j = 0; j < columnas; j++)
            destino[j][i] = origen[i][j];
}

void multiplicar(double A[MAX][MAX], int filasA, int columnasA,
                 double B[MAX][MAX], int columnasB,
                 double resultado[MAX][MAX]) {
    for (int i = 0; i < filasA; i++) {
        for (int j = 0; j < columnasB; j++) {
            resultado[i][j] = 0;
            for (int k = 0; k < columnasA; k++)
                resultado[i][j] += A[i][k] * B[k][j];
        }
    }
}

void copiarMatriz(double origen[MAX][MAX], double destino[MAX][MAX], int tam) {
    for (int i = 0; i < tam; i++)
        for (int j = 0; j < tam; j++)
            destino[i][j] = origen[i][j];
}

int invertir(double matriz[MAX][MAX], double inversa[MAX][MAX], int tam) {
    for (int i = 0; i < tam; i++)
        for (int j = 0; j < tam; j++)
            inversa[i][j] = (i == j) ? 1.0 : 0.0;

    for (int i = 0; i < tam; i++) {
        double pivote = matriz[i][i];
        if (fabs(pivote) < EPSILON)
            return 0;

        for (int j = 0; j < tam; j++) {
            matriz[i][j] /= pivote;
            inversa[i][j] /= pivote;
        }

        for (int k = 0; k < tam; k++) {
            if (k == i) continue;
            double factor = matriz[k][i];
            for (int j = 0; j < tam; j++) {
                matriz[k][j] -= factor * matriz[i][j];
                inversa[k][j] -= factor * inversa[i][j];
            }
        }
    }
    return 1;
}

void guardarSalidaInvalida() {
    FILE *salida = fopen("salida.sal", "w");
    fprintf(salida, "-1\n");
    fclose(salida);
}

void guardarSalida(char tipo, double matriz[MAX][MAX], int filas, int columnas) {
    FILE *salida = fopen("salida.sal", "w");
    if (!salida) {
        fprintf(stderr, "Error: No se pudo escribir salida.sal\n");
        exit(EXIT_FAILURE);
    }
    fprintf(salida, "%c\n", tipo);
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++)
            fprintf(salida, "%.10f ", matriz[i][j]);
        fprintf(salida, "\n");
    }
    fclose(salida);
}

int calcularRango(double matriz[MAX][MAX], int filas, int columnas) {
    double temp[MAX][MAX];
    for (int i = 0; i < filas; i++)
        for (int j = 0; j < columnas; j++)
            temp[i][j] = matriz[i][j];

    int rango = 0;
    for (int col = 0; col < columnas; col++) {
        int filaPivote = -1;
        for (int fila = rango; fila < filas; fila++) {
            if (fabs(temp[fila][col]) > EPSILON) {
                filaPivote = fila;
                break;
            }
        }
        if (filaPivote == -1) continue;

        if (filaPivote != rango) {
            for (int k = 0; k < columnas; k++) {
                double tmp = temp[rango][k];
                temp[rango][k] = temp[filaPivote][k];
                temp[filaPivote][k] = tmp;
            }
        }

        for (int fila = rango + 1; fila < filas; fila++) {
            double factor = temp[fila][col] / temp[rango][col];
            for (int k = col; k < columnas; k++)
                temp[fila][k] -= factor * temp[rango][k];
        }
        rango++;
    }
    return rango;
}

void calcularPseudoinversa() {
    int rango = calcularRango(A, m, n);
    if (rango < ((m < n) ? m : n)) {
        guardarSalidaInvalida();
        return;
    }

    transponer(A, At, m, n);

    if (m >= n) {
        double AtA[MAX][MAX], invAtA[MAX][MAX], resultado[MAX][MAX];
        multiplicar(At, n, m, A, n, AtA);
        double copia[MAX][MAX];
        copiarMatriz(AtA, copia, n);

        if (!invertir(copia, invAtA, n)) {
            guardarSalidaInvalida();
            return;
        }
        multiplicar(invAtA, n, n, At, m, resultado);
        guardarSalida('L', resultado, n, m);

    } else {
        double AAt[MAX][MAX], invAAt[MAX][MAX], resultado[MAX][MAX];
        multiplicar(A, m, n, At, m, AAt);
        double copia[MAX][MAX];
        copiarMatriz(AAt, copia, m);

        if (!invertir(copia, invAAt, m)) {
            guardarSalidaInvalida();
            return;
        }
        multiplicar(At, n, m, invAAt, m, resultado);
        guardarSalida('R', resultado, n, m);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <archivo>\n", argv[0]);
        return 1;
    }
    leerEntrada(argv[1]);
    calcularPseudoinversa();
    return 0;
}