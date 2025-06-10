#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

#define ENSAYOS 10
#define MAX_HILOS 8

double promedio(double *tiempos, int n) {
    double suma = 0.0;
    for (int i = 0; i < n; i++) suma += tiempos[i];
    return suma / n;
}

void copiar_archivo(const char *origen, const char *destino) {
    FILE *src = fopen(origen, "r");
    FILE *dst = fopen(destino, "w");
    if (!src || !dst) {
        printf("Error copiando %s a %s\n", origen, destino);
        exit(1);
    }
    int c;
    while ((c = fgetc(src)) != EOF)
        fputc(c, dst);
    fclose(src);
    fclose(dst);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s entrada_X.ent\n", argv[0]);
        return 1;
    }

    const char *archivo_entrada = argv[1];
    const char *entrada_oficial = "entrada.ent";

    copiar_archivo(archivo_entrada, entrada_oficial);

    FILE *fmet = fopen("metricas.met", "w");
    if (!fmet) {
        printf("No se pudo abrir metricas.met\n");
        return 1;
    }

    fprintf(fmet, "Ensayo,Hilos,Tiempo_Paralelo,Speedup,Eficiencia\n");

    double tiempos_seq[ENSAYOS];
    double tiempos_par[ENSAYOS];

    // Medir secuencial una vez para referencia
    for (int i = 0; i < ENSAYOS; i++) {
        double start = omp_get_wtime();
        system("./secuencial > /dev/null");
        double end = omp_get_wtime();
        tiempos_seq[i] = end - start;
    }

    double t_prom_seq = promedio(tiempos_seq, ENSAYOS);
    printf("Tiempo promedio secuencial: %.6f segundos\n\n", t_prom_seq);

    for (int hilos = 1; hilos <= MAX_HILOS; hilos *= 2) {
        for (int i = 0; i < ENSAYOS; i++) {
            char cmd[128];
            snprintf(cmd, sizeof(cmd), "OMP_NUM_THREADS=%d ./paralelo > /dev/null", hilos);
            double start = omp_get_wtime();
            system(cmd);
            double end = omp_get_wtime();
            tiempos_par[i] = end - start;

            double speedup = t_prom_seq / tiempos_par[i];
            double eficiencia = speedup / hilos;

            fprintf(fmet, "%d,%d,%.6f,%.6f,%.6f\n", i + 1, hilos, tiempos_par[i], speedup, eficiencia);
        }
    }

    fclose(fmet);
    printf("Resultados guardados en metricas.met\n");
    return 0;
}