import glob
import torch.nn
import torch.nn.functional
import torch.optim
import torch.utils.data

# p = 2; t = 1; v = 3; c = 4
d = 4; n = 10; m = 10
# g = 100
cadena_maxima = 14

def mapea_mapa(v):
    if v == " " or v == "E" or v == "S":
        return 1.0
    elif v == "@":
        return 3.0
    elif v == "*":
        return 5.0
    elif v == "#":
        return 7.0

def mapea_cadena(v):
   if v == "l":
       return 1.0
   elif v == "r":
       return 3.0
   elif v == "L":
      return 5.0
   elif v == "R":
      return 7.0
   elif v == "D":
      return 9.0
   elif v == "U":
      return 11.0
   elif v == "d":
      return 13.0
   elif v == "-":
      return 15.0
   elif v == "T":
      return 17.0
   elif v == ">":
      return 19.0
   elif v == "<":
      return 21.0


datos = { }
for tipo in [ "entrenamiento", "validacion" ]:
    entradas, salidas = list( ), list( )
   #  indice = 0
    for ruta in glob.glob(f"./{tipo}/*.txt"):
        with open(ruta) as arch: 
            lineas = [ linea.rstrip("\r\n") for linea in arch ]
            temp = lineas.pop()
            salidas.append(  [ mapea_cadena(c) for c in list("".join(temp))])

            # print(len(temp))
            # if len(temp) % 14 == 0:
            #     print(temp)
            # else:
            #     print("NONE -> ", tipo, ": ",indice, " == ", temp, "[", ruta, "]")
            #     quit()
            # indice += 1

            entradas.append( [ mapea_mapa(c) for c in list("".join(lineas)) ])
    datos[tipo] = torch.utils.data.TensorDataset(torch.tensor(entradas), torch.tensor(salidas))


entrenamiento = torch.utils.data.DataLoader(datos["entrenamiento"], batch_size=16, shuffle=True)
validacion = torch.utils.data.DataLoader(datos["validacion"], batch_size=16, shuffle=True)
modelo = torch.nn.Sequential(
    torch.nn.Linear((n - 2) * (m - 2), cadena_maxima),
    torch.nn.LeakyReLU( ),
    torch.nn.Linear(14, 14),
    torch.nn.LeakyReLU( ),
    torch.nn.Linear(14, 14),
    torch.nn.LeakyReLU( ),
    torch.nn.Linear(14, 14),
   #  torch.nn.Linear((n - 2) * (m - 2), 70),
   #  torch.nn.LeakyReLU( ),
   #  torch.nn.Linear(70, 56),
   #  torch.nn.LeakyReLU( ),
   #  torch.nn.Linear(56, 42),
   #  torch.nn.LeakyReLU( ),
   #  torch.nn.LeakyReLU( ),
   #  torch.nn.LeakyReLU( ),
   #  torch.nn.LeakyReLU( ),
   #  torch.nn.Linear(42, 28),
   #  torch.nn.LeakyReLU( ),
   #  torch.nn.Linear(28, 14)
)

a = 0.001; e = 1000
funcion_error = torch.nn.MSELoss( )
optimizador = torch.optim.Adam(modelo.parameters( ), lr=a)

for i in range(e):
    modelo.train( )
    for x, y in entrenamiento:
        funcion_error(modelo(x), y).backward( )
        optimizador.step( )
        optimizador.zero_grad( )
        
    modelo.eval( )
    with torch.no_grad( ):
        error = 0
        for x, y in validacion:
            error += funcion_error(modelo(x), y).item( )
    print(f"Iteración {i + 1}: error promedio: {error / len(validacion)}")

torch.jit.script(modelo).save("generador_cadena.pt")
