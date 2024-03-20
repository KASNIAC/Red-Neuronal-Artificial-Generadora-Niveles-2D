const anchoTotal = 1000;
const altoTotal = 400;
let ancho, alto, aumentoHorizontal, aumentoVertical, actualHorizontal, actualVertical;

let formulario = document.forms["formulario"];
let cadena, pico_total, trampolin_total, vida_total;
let tablero = [];
let entrada, salida;
let eventos = {
   "ejecutar": null,
   "pausar": null,
   "terminar": false,
};
let retardo_resolver = null;

function ejecuta_retardo(ms) {
   return new Promise(function (resolver) {
      retardo_resolver = resolver;
      setTimeout(() => resolver(), ms);
   });
}

async function carga_datos() {
   let matriz;
   cadena = formulario["cadena"].value; // "Rrrrrrr>rrrrrTrrrrrrrrrr";

   if (formulario["matriz"].value.length === 0) {
      matriz = [
         "##################",
         "#    @@@@@       #",
         "##   @@@@@       #",
         "#                #",
         "#                #",
         "#                #",
         "#                #",
         "#                #",
         "#                #",
         "#E@     *       S#",
         "##################"
      ].join("\n");
      formulario["matriz"].value = matriz;

      formulario["cadena"].value = "Rrrrrrr>rrrrrTrrrrrrrrrr";
      cadena = formulario["cadena"].value;
   } else {
      matriz = formulario["matriz"].value;
   }
   
   // Validacion de formulario
   pico_total = formulario["pico"].value;
   trampolin_total = formulario["trampolin"].value;
   vida_total = formulario["vidas"].value;
   if (pico_total > 10 || pico_total <= 0) {
      pico_total = 10;
      formulario["pico"].value = 10;
   }
   if (trampolin_total > 10 || trampolin_total <= 0) {
      trampolin_total = 10;
      formulario["trampolin"].value = 10;
   }
   if (vida_total > 10 || vida_total <= 0) {
      vida_total = 10;
      formulario["vidas"].value = 10;
   }

   tablero = [];
   for (let linea of matriz.split("\n")) {
      tablero.push(linea.split(""));
   }
   ancho = tablero[0].length, alto = tablero.length;
   aumentoVertical = anchoTotal / ancho, aumentoHorizontal = altoTotal / alto;

   for (let y = 0; y < alto; ++y) {
      for (let x = 0; x < ancho; ++x) {
         if (tablero[y][x] == 'E') {
            entrada = { "first": y, "second": x };
            tablero[y][x] = ' ';
         } else if (tablero[y][x] == 'S') {
            salida = { "first": y, "second": x };
            tablero[y][x] = ' ';
         }
      }
   }

   await dibuja_tablero();
}

async function limpia_tablero() {
   eventos.terminar = true;
   if (eventos.pausar != null) {
      eventos.pausar.resolve();
      eventos.pausar = null;
   }
   if (retardo_resolver != null) {
      retardo_resolver();
      retardo_resolver = null;
   }
   if (eventos.ejecutar != null) {
      await eventos.ejecutar.promise;
      eventos.ejecutar = null;
   }
   eventos.terminar = false;

   document.getElementById("pausar").textContent = "Pausar";
   document.getElementById("correr").disabled = true;
   document.getElementById("pausar").disabled = true;
   document.getElementById("svg-bloques").innerHTML = "";
   document.getElementById("svg-lineas").innerHTML = "";
   document.getElementById("svg-personaje").innerHTML = "";
   document.getElementById("svg-fondo").innerHTML = "";
   document.getElementById("svg-picos").innerHTML = "";
   document.getElementById("svg-trampolines").innerHTML = "";
   document.getElementById("svg-vidas").innerHTML = "";
   document.getElementById("svg-trampolines-usados").innerHTML = "";

}

async function dibuja_tablero() {
   await limpia_tablero();

   let dibuja_ventajas = function (padre, x, y, alto, ancho, nombre, opacidad, tipo, cuantos) {
      for (let i = 0; i < cuantos; ++i) {
         let disponibles = document.getElementById(padre);

         let nuevaVentaja = document.createElementNS("http://www.w3.org/2000/svg", "use");
         nuevaVentaja.setAttribute("href", `#${nombre}-disponibles`);
         if (tipo == 1) {
            nuevaVentaja.setAttribute("id", `${nombre}_${i}`);
         } else if (tipo == 2) {
            nuevaVentaja.setAttribute("id", `${nombre}_equipado`);
            nuevaVentaja.setAttribute("opacity", opacidad);
         }

         nuevaVentaja.setAttribute("x", x - 30 * i);
         nuevaVentaja.setAttribute("y", y);
         nuevaVentaja.setAttribute("width", ancho);
         nuevaVentaja.setAttribute("height", alto);

         disponibles.appendChild(nuevaVentaja);
      }
   };

   dibuja_ventajas("svg-picos", 950, 3, 20, 20, "picos", 1, 1, pico_total);
   dibuja_ventajas("svg-trampolines", 950, 27, 20, 20, "trampolines", 1, 1, trampolin_total);
   dibuja_ventajas("svg-vidas", 950, 53, 20, 20, "vidas", 1, 1, vida_total);

   let fondo = document.createElementNS("http://www.w3.org/2000/svg", "use");
   fondo.setAttribute("href", "#fondo");
   fondo.setAttribute("x", 0);
   fondo.setAttribute("y", 0);
   fondo.setAttribute("width", anchoTotal);
   fondo.setAttribute("height", altoTotal);
   fondo.setAttribute("fill", "blue");
   document.getElementById("svg-fondo").appendChild(fondo);
   for (let y = 0; y < alto; ++y) {
      for (let x = 0; x < ancho; ++x) {
         let nuevoBloque = document.createElementNS("http://www.w3.org/2000/svg", "use");

         if (y == entrada.first && x == entrada.second) {
            nuevoBloque.setAttribute("href", "#bloque-entrada");
            let svg_personaje = document.getElementById("svg-personaje");
            svg_personaje.setAttribute("transform", "translate(" + x * aumentoVertical + ", " + y * aumentoHorizontal + ")");

            let kasniac = document.createElementNS("http://www.w3.org/2000/svg", "use");
            svg_personaje.appendChild(kasniac);

            let easter = Math.random();
            let rango = easter * 100;
            let egg = Math.round(rango);

            if (egg == 53) {
               kasniac.setAttribute("href", "#personaje-oso");
            } else {
               kasniac.setAttribute("href", "#personaje-kasniac");
            }

            kasniac.setAttribute("id", `kasniac`);
            kasniac.setAttribute("x", 0);
            kasniac.setAttribute("y", 0);
            kasniac.setAttribute("width", aumentoVertical);
            kasniac.setAttribute("height", aumentoHorizontal);

            dibuja_ventajas("svg-personaje", aumentoVertical / 2, aumentoHorizontal / 4, aumentoHorizontal, aumentoVertical, "picos", 0, 2, 1);
            dibuja_ventajas("svg-personaje", aumentoVertical / 2, aumentoHorizontal / 4, aumentoHorizontal, aumentoVertical, "latigo", 0, 2, 1);

         } else if (y == salida.first && x == salida.second) {
            nuevoBloque.setAttribute("href", "#bloque-salida");
         } else if (tablero[y][x] == '#') {
            nuevoBloque.setAttribute("href", "#bloque-irrompible");
         } else if (tablero[y][x] == '@') {
            nuevoBloque.setAttribute("href", "#bloque-rompible");
         } else if (tablero[y][x] == '*') {
            nuevoBloque.setAttribute("href", "#bloque-enemigo");
         } else if (tablero[y][x] == ' ') {
            // nuevoBloque.setAttribute("fill", "pink");
         }

         nuevoBloque.setAttribute("id", `${x}_${y}`);
         nuevoBloque.setAttribute("x", x * aumentoVertical);
         nuevoBloque.setAttribute("y", y * aumentoHorizontal);
         nuevoBloque.setAttribute("width", aumentoVertical);
         nuevoBloque.setAttribute("height", aumentoHorizontal);
         document.getElementById("svg-bloques").appendChild(nuevoBloque);
      }
   }

   document.getElementById("correr").disabled = false;
}

function alterna_pausa() {
   if (eventos.pausar == null) {
      eventos.pausar = Promise.withResolvers();
      document.getElementById("pausar").textContent = "Continuar";
   } else {
      eventos.pausar.resolve();
      eventos.pausar = null;
      document.getElementById("pausar").textContent = "Pausar";
   }
}

async function ejecuta_solucion() {
   await dibuja_tablero(), await ejecuta_retardo(150); // await dibuja_tablero() es necesario por si quiero volver a correr la solución una vez que acabe

   eventos.ejecutar = Promise.withResolvers();
   document.getElementById("svg-tablero").classList.add("animado");
   document.getElementById("svg-datos").classList.add("animado");
   document.getElementById("correr").disabled = true;
   document.getElementById("pausar").disabled = false;

   function obstaculo_firme(c) {
      return c == '#' || c == '@';
   }

   try {
      let tablero_propio = JSON.parse(JSON.stringify(tablero));
      let i = entrada.first, j = entrada.second, pico = pico_total, trampolin = trampolin_total, vida = vida_total, caidos = 0;
      let reglas = document.styleSheets[0].cssRules;

      let uso_trampolin = false;
      let trampolin_x = -1, trampolin_y = -1;
      let mueve = function (cayendo, dj) {
         i += cayendo;
         if (!obstaculo_firme(tablero_propio[i][j + dj])) {
            j += dj;
         }
      };
      let rompe = function (di, dj) {
         if (pico != 0) {
            // Dibuja pico
            document.getElementById("picos_equipado").setAttribute("opacity", "1");

            // Quito pico 
            if (obstaculo_firme(tablero_propio[i + di][j + dj])) {
               document.getElementById(`picos_${pico - 1}`).setAttribute("opacity", "0.2");
            }

            pico -= obstaculo_firme(tablero_propio[i + di][j + dj]);

            if (tablero_propio[i + di][j + dj] == '@') {
               tablero_propio[i + di][j + dj] = ' ';
               document.getElementById(`${j + dj}_${i + di}`).setAttribute("opacity", "0");
            }
         }
      };
      let salta = function () {
         let enemigo = false;
         if (trampolin != 0) {
            // Dibuja trampolin
            let elimina_trampolin = document.getElementById(`trampolin_${trampolin_x}_${trampolin_y}`);
            if (elimina_trampolin !== null) {
               elimina_trampolin.parentNode.removeChild(elimina_trampolin);
            }

            uso_trampolin = true;
            trampolin_x = j;
            trampolin_y = i;

            let nuevoTrampolin = document.createElementNS("http://www.w3.org/2000/svg", "use");
            nuevoTrampolin.setAttribute("href", "#trampolines-disponibles");
            let trampolin_usado = document.getElementById("svg-trampolines-usados");
            nuevoTrampolin.setAttribute("id", `trampolin_${j}_${i}`);
            nuevoTrampolin.setAttribute("x", (j * aumentoVertical) + aumentoVertical / 4);
            nuevoTrampolin.setAttribute("y", (i * aumentoHorizontal) + aumentoVertical / 2);
            nuevoTrampolin.setAttribute("width", aumentoVertical / 2);
            nuevoTrampolin.setAttribute("height", aumentoHorizontal / 2);
            nuevoTrampolin.setAttribute("opacity", "1");
            trampolin_usado.appendChild(nuevoTrampolin);

            // Quito trampolin
            document.getElementById(`trampolines_${trampolin - 1}`).setAttribute("opacity", "0.2");
            trampolin -= 1;

            for (let k = 0; k < 4 && !obstaculo_firme(tablero_propio[i - 1][j]); ++k) {
               --i, enemigo |= (tablero_propio[i][j] == '*');
            }
         }
         return enemigo;
      };
      let latiga = function (dj) {
         document.getElementById("latigo_equipado").setAttribute("opacity", "1");
         if (tablero_propio[i][j + dj] == '*') {
            tablero_propio[i][j + dj] = ' ';
            document.getElementById(`${j + dj}_${i}`).setAttribute("opacity", "0");
         }
      };

      let pasos = 0;
      if (cadena.length == 0) {
         throw ("Perdiste: la cadena vacía no te llevará a ningún lado.");
      }
      for (const c of cadena) {
         document.getElementById("picos_equipado").setAttribute("opacity", "0");
         document.getElementById("latigo_equipado").setAttribute("opacity", "0");
         document.getElementById("kasniac").setAttribute("opacity", "1");

         if (eventos.pausar != null) {
            await eventos.pausar.promise;
         }
         if (eventos.terminar) {
            break;
         }

         let retardo = formulario["retardo"].value;
         reglas[reglas.length - 1].style.transitionDuration = `${retardo}ms`;

         let enemigo = false, cayendo = !obstaculo_firme(tablero_propio[i + 1][j]);
         if (c == 'l') {
            mueve(cayendo, -1);
         } else if (c == 'r') {
            mueve(cayendo, +1);
         } else if (c == 'd' || c == '-') {
            mueve(cayendo, 0);
         } else if (c == 'L') {
            (!cayendo ? rompe(0, -1) : mueve(cayendo, 0));
         } else if (c == 'R') {
            (!cayendo ? rompe(0, +1) : mueve(cayendo, 0));
         } else if (c == 'D') {
            (!cayendo ? rompe(+1, 0) : mueve(cayendo, 0));
         } else if (c == 'U') {
            (!cayendo ? rompe(-1, 0) : mueve(cayendo, 0));
         } else if (c == 'T') {
            (!cayendo ? enemigo = salta(enemigo) : mueve(cayendo, 0));
         } else if (c == '<') {
            latiga(-1);
            mueve(cayendo, 0);
         } else if (c == '>') {
            latiga(+1);
            mueve(cayendo, 0);
         } else {
            //throw "comando ilegal";
         }

         if (tablero_propio[i][j] == '*' || enemigo) {
            document.getElementById("kasniac").setAttribute("opacity", "0.5");
            // Quito vida
            document.getElementById(`vidas_${vida - 1}`).setAttribute("opacity", "0.2");
         }

         if (uso_trampolin && obstaculo_firme(tablero_propio[i + 1][j])) {
            document.getElementById(`trampolin_${trampolin_x}_${trampolin_y}`).setAttribute("opacity", "0");
            uso_trampolin = false;
         }

         let svg_personaje = document.getElementById("svg-personaje");
         svg_personaje.setAttribute("transform", "translate(" + j * aumentoVertical + ", " + i * aumentoHorizontal + ")");
         await ejecuta_retardo(retardo);

         vida -= (tablero_propio[i][j] == '*' || enemigo);
         caidos = (cayendo ? caidos + 1 : 0);
         if (vida == 0) {
            throw "Perdiste: los perros locos te han devorado"; // throw "vida ilegal";
         } else if (caidos > 4 && obstaculo_firme(tablero_propio[i + 1][j])) {
            throw "Perdiste: caíste mas de 4 bloques"; // throw "caida ilegal";
         } else if (i == salida.first && j == salida.second) {
            throw "¡Felicidades, has ganado!";
         } else if (pasos == cadena.length - 1) {
            throw ("Perdiste: la cadena terminó y jamás llegaste a la salida");
         }
         ++pasos;
      }
      document.getElementById("kasniac").setAttribute("opacity", "1");
   } catch (ex) {
      alert(ex);
   }

   document.getElementById("svg-tablero").classList.remove("animado");
   document.getElementById("svg-datos").classList.remove("animado");
   document.getElementById("correr").disabled = false;
   document.getElementById("pausar").disabled = true;
   eventos.ejecutar.resolve();
}

const modal = document.getElementById("mostrar-instrucciones");
function mostrar_instrucciones(estado) {
   if (estado) {
      modal.showModal();
   } else {
      modal.close();
   }
}