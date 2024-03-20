import torch.nn
import torch.nn.functional
import torch.optim
import torch.utils.data

p = 2; t = 1; v = 3; c = 4
d = 4; n = 10; m = 10
g = 100

datos = { }
for tipo in [ "entrenamiento_generador", "validacion_generador" ]:
    entradas, salidas = list( ), list( )
    for i in range(0, g):
        for j in range(0, pow(2, d)):
            entrada = (8 * torch.rand(d + (n - 2) * (m - 2))).tolist( )
            for b in range(0, d):
                entrada[b] = (j & (1 << b)) != 0
            entradas.append(entrada)
            salidas.append(float(0b111110000 | j))
    datos[tipo] = torch.utils.data.TensorDataset(torch.tensor(entradas), torch.tensor(salidas))
    
entrenamiento = torch.utils.data.DataLoader(datos["entrenamiento_generador"], batch_size=64, shuffle=True)
validacion = torch.utils.data.DataLoader(datos["validacion_generador"], batch_size=64, shuffle=True)
modelo = torch.nn.Sequential(
    torch.nn.Linear(d + (n - 2) * (m - 2), (n - 2) * (m - 2)),
    torch.nn.LeakyReLU( ),
    torch.nn.LeakyReLU( ),
    torch.nn.LeakyReLU( )
)
discriminador = torch.jit.load("discriminador.pt")
discriminador.eval( )

a = 0.001; e = 1000
funcion_error = torch.nn.MSELoss( )
optimizador = torch.optim.Adam(modelo.parameters( ), lr=a)

for i in range(e):
    modelo.train( )
    for x, y in entrenamiento:
        funcion_error(discriminador(modelo(x)), y.unsqueeze(1)).backward( )
        optimizador.step( )
        optimizador.zero_grad( )
        
    modelo.eval( )
    with torch.no_grad( ):
        error = 0
        for x, y in validacion:
            error += funcion_error(discriminador(modelo(x)), y.unsqueeze(1)).item( )
    print(f"Iteración {i + 1}: error promedio: {error / len(validacion)}")

torch.jit.script(modelo).save("generador.pt")