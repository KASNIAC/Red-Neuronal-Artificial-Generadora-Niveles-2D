# Red Neuronal Artificial Generadora Niveles 2D

Este proyecto consiste en un juego de plataformas 2D para navegadores web, en donde los niveles pueden ser creados por una red neuronal artificial de manera local y esos mismos niveles pueden ser jugados por otra red neuronal artificial diferente.

Durante el desarrollo del presente proyecto fue necesario resolver un problema de busqueda en amplitud (BFS) cuya descripción se encuentra en el juez en línea [OmegaUp](https://omegaup.com/arena/problem/Spelunky). Se invita a los espectadores a intentar resolverlo.

Posteriormente se hicieron una serie de modificaciones al código para permitir mas elementos como los enemigos. A continuación, los dejo con un recorrido a través del proyecto, en caso de que deseen probarlo ☝🤓.

## Tabla de Contenidos

- [01-Validador](#01-Validador)
- [02-Interfaz_web](#02-Interfaz_web)
- [03-Red_neuronal_generadora_mapas](#03-Red_neuronal_generadora_mapas)
- [04-Red_neuronal_generador_cadenas_soluciones](#04-Red_neuronal_generador_cadenas_soluciones)

<br>

  ## 01-Validador
En este directorio encontrarás el programa **validador-grafico-flexible.cpp**, el programa recibe 2 argumentos desde la consola: una nivel y una cadena. Este programa intenará ejecutar cada una de las acciones descritas por la cadena sobre el nivel. El modo de uso se encuentra en los archivos de compilacion **compilar_y_usar.bat** para windows o **compilar_y_usar.sh para linux**.

  ## 02-Interfaz web
En este directorio se encuentra la interfaz gráfica web en donde se puede realizar la visualización de una manera mas amigable. El núcleo se encuentra en la carpeta js en donde el validador descrito en el apartado anterior ha sido traducido al lenguaje JavsScript para cumplir con el mismo objetivo, solo que ahora recibiendo las entradas desde un formulario HTML y no como argumentos de consola.

  ## 03-Red_neuronal_generadora_mapas
1) generador_aleatorio.cpp genera todos los mapas de las carpetas "entrenamiento_discriminador" y "validacion_discriminador", los cuales serán usados para entrenar a la red "discriminadora" de mapas, que recibe como entrada un nivel y produce como salida un valor que indica que tan bueno es el nivel, en el sentido de que sí es posible resolverlo (variando la cantidad de vidas, picos, trampolines, etc)

2) El programa "entrenamiendo_discriminador.py" recibe como entrenamiento los casos generados por generador_aleatorio.cpp (los de las carpetas "entrenamiento_discriminador" y "validacion_discriminador"), de esta manera se tendrá la primera red neuronal que será capaz de emitir una calificación determinada para un nivel dado.

3) El programa "entrenamiento_generador.py" recibe como entreamiento "ruido" y adicionalmente 4 valores que indican la dificultad para el nivel que queremos obtener. Esta segunda red neuronal es la que deberá poder generar los niveles en forma de caracteres que se pasarán a la primera red para poder ser evaluados. En este mismo programa se ejecuta la EVALUACION de "entrenamiento_discriminador.py"

4) El programa "evaluador_generador.py" se encarga de la EVALUACION de "entrenamiento_generador.py", es decir, es el programa que se usa para poder usar la red neuronal "generador.pt"

5) El programa "evaluador_generados.cpp" se encarga de verificar que los mapas generador por la red neuronal (a través del programa "evaluador_generador.py") sean en verdad factibles.

  ## 04-Red_neuronal_generador_cadenas_soluciones
  1) "evaluador_generados.cpp" es el programa generador de casos de entrenamiento para la red. Primero se necesita un conjunto de niveles, los cuales son generados por la red de la etapa anterior, a través del programa "evaluador_generador.py"

2) el programa "evaluador_generador.py" crea el archivo "ejemplo_niveles.txt" (redirigiendo la entrada con >). Con muy poca probabilidad estos niveles generados por la red pueden estar repetidos o NO tener solucion.

3) "evaluador_generados.cpp" lee los datos del archivo "ejemplo_niveles.txt" y resuelve cada uno de estos niveles, mandadolos a /entrenamiento y /validacion. El programa se asegura de que todos los niveles tengan solución y que además NO exista ningún nivel repetido. Estos casos se usaran para entrenar a la red que resuelve niveles.

4) "entrenamiento_gen_sol.py" es el programa encargado de entrenar a la red neuronal que generará las soluciones para cada uno de estos niveles, a partir de /entrenamiento y /validacion. El entrenamiento se quedá en un error aproximado de 5.0

5) Una vez terminado el entrenamiento se genera "generador_cadena.pt" que se evalua desde el programa "evaluador_gen_sol.py". Es desde este programa donde ya podemos observar los resultados de la red entrenada.

6) El programa "evaluador_gen_sol.py" se adaptó para que evaluará los niveles de /entrenamiento y /pruebas, de esta manera la red genera soluciones para todos estos niveles y los almacena en "niveles_soluciones.txt" (redireccionando la entrada >)

7) El programa "validator_flexible_prueba_red.cpp" lee el archivo "niveles_soluciones.txt" y evalua todos los niveles con sus respectivas soluciones, de esta manera se obtiene el numero de soluciones que la red genero de manera correcta e incorrecta.
