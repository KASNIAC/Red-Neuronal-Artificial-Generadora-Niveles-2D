import torch.nn
import torch.nn.functional
import torch.optim

p = 2; t = 1; v = 3; c = 4
d = 4; n = 10; m = 10
g = 200 # Generará 16 * 200 niveles

generador = torch.jit.load("generador.pt")
generador.eval( )

def recupera(v):
    if v <= 2:
        return " "
    elif v <= 4:
        return "@"
    elif v <= 6:
        return "*"
    else:
        return "#"

temp = 0
for i in range(0, g):
    for j in range(0, pow(2, d)):
        entrada = torch.rand(d + (n - 2) * (m - 2)).tolist( )
        for b in range(0, d):
            entrada[b] = (j & (1 << b)) != 0
        salida = generador(torch.tensor(entrada))
        
        indice, cadena = 0, ""
        for x in range(0, n):
            for y in range(0, m):
                if x == 0 or x == n - 1 or y == 0 or y == m - 1:
                    cadena += "#"
                elif x == 1 and y == m // 2:
                    cadena += "E"
                elif x == 2 and y == m // 2:
                    cadena += "#"
                elif x == n - 2 and y == m // 2:
                    cadena += "S"
                else:
                    cadena += recupera(salida[indice])
                    indice += 1
            cadena += "\n"
        print(j)
        print(cadena)
        temp += 1

print(temp)
