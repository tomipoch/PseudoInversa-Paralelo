Instrucciones de Compilación y Ejecución

Archivos Incluidos
	•	secuencial.c : Código para el cálculo secuencial de la pseudoinversa.
	•	paralelo.c : Código para el cálculo paralelo de la pseudoinversa con OpenMP.
	•	metricas.c : Script para calcular speedup y eficiencia.
	•	entrada.ent : Archivo de entrada de prueba.
	•	entrada1.ent : Archivo de entrada adicional de mayor tamaño.
	•	salida.sal : Resultado generado por el código secuencial.
	•	salida_paralela.sal : Resultado generado por el código paralelo.
	•	metricas.met : Resultados de las métricas de ejecución.

Requisitos
	•	Sistema operativo: macOS
	•	Compiladores: gcc para secuencial, clang-omp o como lo hayas configurad para paralelo

Compilación

1. Compilar código secuencial

gcc secuencial.c -o secuencial -lm

2. Compilar código paralelo

clang-omp paralelo.c -o paralelo -fopenmp -lm

3. Compilar script de métricas

gcc metricas.c -o metricas -lm

Ejecución

1. Ejecutar cálculo secuencial

./secuencial entrada.ent

2. Ejecutar cálculo paralelo con N hilos (ejemplo: 4)

OMP_NUM_THREADS=4 ./paralelo entrada.ent

3. Ejecutar generación de métricas

./metricas entrada.ent

Formato de entrega
	1.	Comprimir todos los archivos en un solo archivo .zip o .tar:

zip TOMAS_PEREZ_HERNANDEZ.zip secuencial.c paralelo.c metricas.c entrada.ent entrada1.ent salida.sal salida_paralela.sal metricas.met README.txt

	2.	Subir el archivo comprimido a UCM Virtual antes del 10 de junio a las 23:59.

Observación
	•	Asegúrese de que los archivos generados (salida.sal, metricas.met, etc.) tengan el formato exacto exigido.
	•	Para evitar penalizaciones, verifique que el rango de la matriz sea completo antes de enviar.

⸻

Trabajo realizado por: Tomas Poblete y Cristobal Perez