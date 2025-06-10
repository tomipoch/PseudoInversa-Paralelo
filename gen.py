import numpy as np

def generar_matriz_rango_completo(filas, columnas, archivo):
    while True:
        A = np.random.randn(filas, columnas)
        if np.linalg.matrix_rank(A) == min(filas, columnas):
            break
    with open(archivo, 'w') as f:
        f.write(f"{filas} {columnas}\n")
        for fila in A:
            f.write(" ".join(f"{x:.2f}" for x in fila) + "\n")

generar_matriz_rango_completo(700, 600, "entrada_valida.ent")