import glob # Para usar los casos de la carpeta /pruebas
import torch.nn
import torch.nn.functional
import torch.optim

p = 2; t = 1; v = 3; c = 4
d = 4; n = 10; m = 10
g = 100

generador = torch.jit.load("generador_cadena.pt")
generador.eval( )

def mapea_mapa(v):
    if v == " " or v == "E" or v == "S":
        return 1.0
    elif v == "@":
        return 3.0
    elif v == "*":
        return 5.0
    elif v == "#":
        return 7.0

def recupera_cadena(v):
   if v <= 2:
      return "l"
   elif v <= 4:
      return "r"
   elif v <= 6:
      return "L"
   elif v <= 8:
      return "R"
   elif v <= 10:
      return "D"
   elif v <= 12:
      return "U"
   elif v <= 14:
      return "d"
   elif v <= 16:
      return "-"
   elif v <= 18:
      return "T"
   elif v <= 20:
      return ">"
   elif v <= 22:
      return "<"

indice = 0
# ruta = glob.glob(f"./pruebas/ejemplo0.txt") # glob.glob(f"./pruebas/*.txt")
for tipo in [ "validacion", "pruebas" ]:
   for ruta in glob.glob(f"./{tipo}/*.txt"):
      with open(ruta) as arch: 
         lineas = [ linea.rstrip("\r\n") for linea in arch ]
         lineas.pop()

         entrada = [ mapea_mapa(c) for c in list("".join(lineas)) ]

         # Imprimo un indice
         print(indice)
         indice += 1

         # Imprimo el mapa
         for i in range(m):
            print('#', end="")
         print()
         
         for flat in lineas:
            print("#" + flat + "#")

         for i in range(m):
            print('#', end="")
         print()

         # La red genera la salida
         salida = generador(torch.tensor(entrada))

         cadena_solucion = ""
         for valor in salida:
            cadena_solucion += recupera_cadena(valor)

         # Imprimo la cadena
         print(cadena_solucion)
         print()
         
   
    
