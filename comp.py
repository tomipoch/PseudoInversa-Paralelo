# comparar_salidas.py
import sys
import math

TOLERANCIA = 1e-6

def comparar_lineas(l1, l2):
    v1 = l1.strip().split()
    v2 = l2.strip().split()
    if len(v1) != len(v2):
        return False
    for a, b in zip(v1, v2):
        try:
            if abs(float(a) - float(b)) > TOLERANCIA:
                return False
        except ValueError:
            if a != b:
                return False
    return True

def comparar_archivos(f1, f2):
    with open(f1) as file1, open(f2) as file2:
        for i, (l1, l2) in enumerate(zip(file1, file2), 1):
            if not comparar_lineas(l1, l2):
                print(f"Diferencia en línea {i}:")
                print(f"  {f1}: {l1.strip()}")
                print(f"  {f2}: {l2.strip()}")
                return
        print("Los archivos son equivalentes dentro de la tolerancia.")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Uso: python3 comparar_salidas.py salida.sal salida_paralela.sal")
        sys.exit(1)
    comparar_archivos(sys.argv[1], sys.argv[2])