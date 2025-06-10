#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define ENSAYOS 10
#define MAX_HILOS 128

double promedio(double *tiempos, int n) {
    double suma = 0.0;
    for (int i = 0; i < n; i++) suma += tiempos[i];
    return suma / n;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s entrada_X.ent\n", argv[0]);
        return 1;
    }

    const char *archivo_entrada = argv[1];

    FILE *fmet = fopen("metricas.met", "w");
    if (!fmet) {
        printf("No se pudo abrir metricas.met\n");
        return 1;
    }

    fprintf(fmet, "Ensayo,Hilos,Speedup,Eficiencia\n");

    double tiempos_seq[ENSAYOS];
    double tiempos_par[ENSAYOS];

    // Secuencial
    for (int i = 0; i < ENSAYOS; i++) {
        char cmd[128];
        snprintf(cmd, sizeof(cmd), "./secuencial %s > /dev/null", archivo_entrada);
        double start = omp_get_wtime();
        system(cmd);
        double end = omp_get_wtime();
        tiempos_seq[i] = end - start;
    }

    double t_prom_seq = promedio(tiempos_seq, ENSAYOS);
    printf("Tiempo promedio secuencial: %.6f segundos\n\n", t_prom_seq);

    // Paralelo
    for (int hilos = 1; hilos <= MAX_HILOS; hilos *= 2) {
        for (int i = 0; i < ENSAYOS; i++) {
            char cmd[128];
            snprintf(cmd, sizeof(cmd), "OMP_NUM_THREADS=%d ./paralelo %s > /dev/null", hilos, archivo_entrada);
            double start = omp_get_wtime();
            system(cmd);
            double end = omp_get_wtime();
            tiempos_par[i] = end - start;

            double speedup = t_prom_seq / tiempos_par[i];
            double eficiencia = speedup / hilos;

            fprintf(fmet, "%d,%d,%.6f,%.6f\n", i + 1, hilos, speedup, eficiencia);
        }
    }

    fclose(fmet);
    printf("Resultados guardados en metricas.met\n");
    return 0;
}