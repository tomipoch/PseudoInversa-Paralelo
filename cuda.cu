/*
 * Autores: Tomas Poblete Chamorro, Cristobal Perez Ramirez
 * Carrera: Ingeniería Civil Informática
 * Fecha: 10 de Junio de 2025
 * Propósito: Cálculo Paralelo con Cuda C
 */

#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#define MAX 100
#define TOL 1e-6

// Kernel CUDA para transponer una matriz
__global__ void kernel_transpose(double* A, double* At, int m, int n) {
    int i = blockIdx.y * blockDim.y + threadIdx.y; // fila de A
    int j = blockIdx.x * blockDim.x + threadIdx.x; // columna de A
    if (i < m && j < n)
        At[j * m + i] = A[i * n + j];
}

// Kernel CUDA para multiplicar matrices
__global__ void kernel_mult(double* A, double* B, double* R, int r, int c, int p) {
    int i = blockIdx.y * blockDim.y + threadIdx.y; // fila de A
    int j = blockIdx.x * blockDim.x + threadIdx.x; // columna de B
    if (i < r && j < p) {
        double suma = 0.0;
        for (int k = 0; k < c; k++)
            suma += A[i * c + k] * B[k * p + j];
        R[i * p + j] = suma;
    }
}

// Función CPU: invertir matriz cuadrada por sistemas lineales (puedes mejorarla con LAPACK)
int invertir(double A[MAX][MAX], double inv[MAX][MAX], int n) {
    double temp;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            inv[i][j] = (i == j) ? 1.0 : 0.0;

    for (int i = 0; i < n; i++) {
        if (fabs(A[i][i]) < TOL) return 0;
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

void leer_matriz(FILE* archivo, double A[MAX][MAX], int* m, int* n) {
    fscanf(archivo, "%d %d", m, n);
    for (int i = 0; i < *m; i++)
        for (int j = 0; j < *n; j++)
            fscanf(archivo, "%lf", &A[i][j]);
}

void escribir_salida(FILE* archivo, char tipo, double* R, int rows, int cols) {
    fprintf(archivo, "%c\n", tipo);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(archivo, "%.10lf", R[i * cols + j]);
            if (j < cols - 1) fprintf(archivo, " ");
        }
        fprintf(archivo, "\n");
    }
}

int main(int argc, char* argv[]) {
    // Configuración bloques e hilos por bloque, puedes parametrizarlo
    int hilosBloque = 16; // ejemplo, puedes variar por línea de comandos
    int bloques, bloquesY, bloquesX;
    double A[MAX][MAX], At[MAX][MAX], AA[MAX][MAX], AAt[MAX][MAX];
    double Inv[MAX][MAX], Resultado[MAX][MAX];
    int m, n;
    char archivo_entrada[128] = "entrada.ent";
    if (argc > 1) strcpy(archivo_entrada, argv[1]);
    FILE* entrada = fopen(archivo_entrada, "r");
    FILE* salida = fopen("salida.sal", "w");
    if (!entrada || !salida) {
        printf("Error abriendo archivos.\n");
        return 1;
    }

    leer_matriz(entrada, A, &m, &n);
    fclose(entrada);

    // Reserva y copia memoria a device
    double *d_A, *d_At, *d_AA, *d_AAt, *d_Resultado;
    size_t sz_A = m * n * sizeof(double);
    size_t sz_At = n * m * sizeof(double);
    size_t sz_AA = n * n * sizeof(double);
    size_t sz_AAt = m * m * sizeof(double);

    cudaMalloc(&d_A, sz_A);
    cudaMalloc(&d_At, sz_At);
    cudaMemcpy(d_A, A, sz_A, cudaMemcpyHostToDevice);

    // --- Transponer
    dim3 threadsTrans(hilosBloque, hilosBloque);
    dim3 gridTrans((n + hilosBloque - 1)/hilosBloque, (m + hilosBloque - 1)/hilosBloque);
    kernel_transpose<<<gridTrans, threadsTrans>>>(d_A, d_At, m, n);
    cudaMemcpy(At, d_At, sz_At, cudaMemcpyDeviceToHost);

    if (m <= n) {
        // Pseudoinversa derecha: R = A^T * (A * A^T)^-1
        cudaMalloc(&d_AAt, sz_AAt);
        cudaMalloc(&d_Resultado, sz_At);
        // Multiplica A * At
        dim3 gridMul((m + hilosBloque - 1)/hilosBloque, (m + hilosBloque - 1)/hilosBloque);
        kernel_mult<<<gridMul, threadsTrans>>>(d_A, d_At, d_AAt, m, n, m);
        cudaMemcpy(AAt, d_AAt, sz_AAt, cudaMemcpyDeviceToHost);
        if (!invertir(AAt, Inv, m)) {
            fprintf(salida, "-1\n");
            return 0;
        }
        // Multiplica At * Inv
        cudaMemcpy(d_At, At, sz_At, cudaMemcpyHostToDevice);
        cudaMemcpy(d_AAt, Inv, sz_AAt, cudaMemcpyHostToDevice);
        kernel_mult<<<gridTrans, threadsTrans>>>(d_At, d_AAt, d_Resultado, n, m, m);
        double resultado[MAX*MAX];
        cudaMemcpy(resultado, d_Resultado, sz_At, cudaMemcpyDeviceToHost);
        escribir_salida(salida, 'R', resultado, n, m);
    } else {
        // Pseudoinversa izquierda: L = (A^T * A)^-1 * A^T
        cudaMalloc(&d_AA, sz_AA);
        cudaMalloc(&d_Resultado, sz_At);
        // Multiplica At * A
        dim3 gridMul((n + hilosBloque - 1)/hilosBloque, (n + hilosBloque - 1)/hilosBloque);
        kernel_mult<<<gridMul, threadsTrans>>>(d_At, d_A, d_AA, n, m, n);
        cudaMemcpy(AA, d_AA, sz_AA, cudaMemcpyDeviceToHost);
        if (!invertir(AA, Inv, n)) {
            fprintf(salida, "-1\n");
            return 0;
        }
        cudaMemcpy(d_AA, Inv, sz_AA, cudaMemcpyHostToDevice);
        // Multiplica Inv * At
        kernel_mult<<<gridMul, threadsTrans>>>(d_AA, d_At, d_Resultado, n, n, m);
        double resultado[MAX*MAX];
        cudaMemcpy(resultado, d_Resultado, sz_At, cudaMemcpyDeviceToHost);
        escribir_salida(salida, 'L', resultado, n, m);
    }

    // Libera memoria
    cudaFree(d_A); cudaFree(d_At);
    cudaFree(d_AA); cudaFree(d_AAt); cudaFree(d_Resultado);
    fclose(salida);
    return 0;
}