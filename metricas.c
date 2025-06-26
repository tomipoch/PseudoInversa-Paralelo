/*
 * Autores: Tomas Poblete Chamorro, Cristobal Perez Ramirez
 * Carrera: Ingeniería Civil Informática
 * Fecha: 10 de Junio de 2025
 * Propósito: Cálculo de Metricas
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define ENSAYOS 10         // Cantidad de repeticiones por configuración
#define MAX_HILOS 128      // Máximo número de hilos a probar (potencias de 2 hasta 128)

// Calcula el promedio de un arreglo de tiempos
double promedio(double *tiempos, int n) {
    double suma = 0.0;
    for (int i = 0; i < n; i++) suma += tiempos[i];
    return suma / n;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        // Verifica uso correcto del programa
        printf("Uso: %s entrada_X.ent\n", argv[0]);
        return 1;
    }

    const char *archivo_entrada = argv[1];  // Ruta del archivo de entrada

    // Abre archivo de salida para guardar las métricas
    FILE *fmet = fopen("metricas.met", "w");
    if (!fmet) {
        printf("No se pudo abrir metricas.met\n");
        return 1;
    }

    // Escribe encabezado del CSV
    fprintf(fmet, "Ensayo,Hilos,Speedup,Eficiencia\n");

    double tiempos_seq[ENSAYOS];  // Arreglo para almacenar tiempos secuenciales
    double tiempos_par[ENSAYOS];  // Arreglo para tiempos paralelos

    // Ejecuta el código secuencial múltiples veces
    for (int i = 0; i < ENSAYOS; i++) {
        char cmd[128];
        snprintf(cmd, sizeof(cmd), "./secuencial %s > /dev/null", archivo_entrada);  // Redirige salida a null
        double start = omp_get_wtime();  // Tiempo de inicio
        system(cmd);                     // Ejecuta el binario secuencial
        double end = omp_get_wtime();    // Tiempo de fin
        tiempos_seq[i] = end - start;    // Guarda duración del ensayo
    }

    // Calcula el tiempo promedio de los ensayos secuenciales
    double t_prom_seq = promedio(tiempos_seq, ENSAYOS);
    printf("Tiempo promedio secuencial: %.6f segundos\n\n", t_prom_seq);

    // Pruebas paralelas con número creciente de hilos (potencias de 2)
    for (int hilos = 1; hilos <= MAX_HILOS; hilos *= 2) {
        for (int i = 0; i < ENSAYOS; i++) {
            char cmd[128];
            // Establece variable de entorno para controlar hilos, ejecuta el programa
            snprintf(cmd, sizeof(cmd), "OMP_NUM_THREADS=%d ./paralelo %s > /dev/null", hilos, archivo_entrada);
            double start = omp_get_wtime();
            system(cmd);
            double end = omp_get_wtime();
            tiempos_par[i] = end - start;

            // Calcula Speedup y Eficiencia
            double speedup = t_prom_seq / tiempos_par[i];
            double eficiencia = speedup / hilos;

            // Escribe resultados por ensayo
            fprintf(fmet, "%d,%d,%.6f,%.6f\n", i + 1, hilos, speedup, eficiencia);
        }
    }

    fclose(fmet);  // Cierra archivo de métricas
    printf("Resultados guardados en metricas.met\n");
    return 0;
}