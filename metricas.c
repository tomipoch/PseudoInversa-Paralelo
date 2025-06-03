// Archivo: metricas.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ENSAYOS 10
#define MAX_POTENCIA 4

int potencias[] = {2, 4, 8, 16};

// Función para medir el tiempo de ejecución en segundos
double medirTiempo(const char *comando) {
    clock_t inicio = clock();
    system(comando);
    clock_t fin = clock();
    return ((double)(fin - inicio)) / CLOCKS_PER_SEC;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <archivo_entrada>\n", argv[0]);
        return 1;
    }

    const char *archivoEntrada = argv[1];

    // Compilar los programas una sola vez
    system("gcc secuencial.c -o secuencial -lm");
    system("gcc -fopenmp paralelo.c -o paralelo -lm");

    //para macOS segun tu instalacion de gcc con omp, ya que apple clang no incluye omp
    //system("clang-omp paralelo.c -o paralelo -fopenmp -lm");

    FILE *archivo = fopen("metricas.met", "w");
    if (!archivo) {
        perror("No se pudo abrir metricas.met");
        return 1;
    }

    fprintf(archivo, "Ensayo Hilos Speedup Eficiencia\n");

    // Tiempo secuencial promedio
    double tiempoSecTotal = 0.0;
    for (int i = 0; i < ENSAYOS; i++) {
        tiempoSecTotal += medirTiempo("./secuencial entrada.ent");
    }
    double tiempoSec = tiempoSecTotal / ENSAYOS;

    // Para cada potencia de hilos
    for (int p = 0; p < MAX_POTENCIA; p++) {
        int hilos = potencias[p];
        double tiempoParTotal = 0.0;

        for (int j = 0; j < ENSAYOS; j++) {
            char comando[256];
            sprintf(comando, "OMP_NUM_THREADS=%d ./paralelo entrada.ent", hilos);
            tiempoParTotal += medirTiempo(comando);
        }

        double tiempoPar = tiempoParTotal / ENSAYOS;
        double speedup = tiempoSec / tiempoPar;
        double eficiencia = speedup / hilos;

        fprintf(archivo, "%d %d %.10f %.10f\n", p + 1, hilos, speedup, eficiencia);
    }

    fclose(archivo);
    printf("Archivo metricas.met generado correctamente.\n");
    return 0;
}