import glob
import torch.nn
import torch.nn.functional
import torch.optim
import torch.utils.data

p = 2; t = 1; v = 3; c = 4
d = 4; n = 10; m = 10

def mapea(v):
    if v == " ":
        return 1.0
    elif v == "@":
        return 3.0
    elif v == "*":
        return 5.0
    elif v == "#":
        return 7.0

datos = { }
for tipo in [ "entrenamiento_discriminador", "validacion_discriminador" ]:
    entradas, salidas = list( ), list( )
    for ruta in glob.glob(f"./{tipo}/*.data"):
        with open(ruta) as arch: 
            lineas = [ linea.rstrip("\r\n") for linea in arch ]
            salidas.append(float(lineas.pop( )))
            entradas.append([ mapea(c) for c in list("".join(lineas)) ])
    datos[tipo] = torch.utils.data.TensorDataset(torch.tensor(entradas), torch.tensor(salidas))

entrenamiento = torch.utils.data.DataLoader(datos["entrenamiento_discriminador"], batch_size=64, shuffle=True)
validacion = torch.utils.data.DataLoader(datos["validacion_discriminador"], batch_size=64, shuffle=True)
modelo = torch.nn.Sequential(
    torch.nn.Linear((n - 2) * (m - 2), 40),
    torch.nn.LeakyReLU( ),
    torch.nn.Linear(40, 20),
    torch.nn.LeakyReLU( ),
    torch.nn.Linear(20, 10),
    torch.nn.LeakyReLU( ),
    torch.nn.Linear(10, 5),
    torch.nn.LeakyReLU( ),
    torch.nn.Linear(5, 1)
)

a = 0.001; e = 250
funcion_error = torch.nn.MSELoss( )
optimizador = torch.optim.Adam(modelo.parameters( ), lr=a)

for i in range(e):
    modelo.train( )
    for x, y in entrenamiento:
        funcion_error(modelo(x), y.unsqueeze(1)).backward( )
        optimizador.step( )
        optimizador.zero_grad( )
        
    modelo.eval( )
    with torch.no_grad( ):
        error = 0
        for x, y in validacion:
            error += funcion_error(modelo(x), y.unsqueeze(1)).item( )
    print(f"Iteración {i + 1}: error promedio: {error / len(validacion)}")

torch.jit.script(modelo).save("discriminador.pt")
