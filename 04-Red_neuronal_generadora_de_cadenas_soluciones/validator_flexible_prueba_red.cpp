#include <cctype>
#include <climits>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

struct posicion_estado {
   int8_t i, j;
   int8_t pi, pj;
   bool trampolin;
   char comando;
   int anterior;
};

bool obstaculo_firme(char c) {
   return c == '#' || c == '@';
}

int main(int argc, const char *argv[]) {
   int filas = 10, columnas = 10;
   int correctos = 0, incorrectos = 0;
   int error_comando = 0, error_vida = 0, error_caida = 0, error_longitud = 0;
   bool band_error = false;

   int suma_x = 0, suma_y = 0;

   std::vector<int> exito; // Guarda los indices de los niveles que sí tienen solución
   std::map<int, int> cuenta_errores;

   int indice;
   while (std::cin >> indice) {
      std::getchar();

      std::vector<std::string> tablero;
      std::pair<int8_t, int8_t> entrada, salida;

      std::string res;
      std::string linea;
      for (int i = 0; i < filas; ++i) {
         std::getline(std::cin, linea);
         tablero.push_back(linea);

         for (int8_t j = 0; j < tablero[i].size(); ++j) {
            if (std::isalpha(tablero[i][j])) {
               (tablero[i][j] == 'E' ? entrada : salida) = {i, j};
               tablero[i][j] = ' ';
            }
         }
      }
      std::getline(std::cin, res);

      // DEBUG:
      // std::cout << indice << "\n";
      // for (int i = 0; i < filas; ++i) {
      //    std::cout<<tablero[i]<<"\n";
      // }
      // std::cout<<res<<"\n";

      // return 0;
      // DEBUG

      // Validator:
      bool band_correcto = false;
      int i = entrada.first, j = entrada.second, caidos = 0, pico = 2, trampolin = 1, vida = 3;
      auto mueve = [&](bool cayendo, int dj) {
         i += cayendo;
         if (!obstaculo_firme(tablero[i][j + dj])) {
            j += dj;
         }
      };
      auto rompe = [&](int di, int dj) {
         if (pico != 0) {
            pico -= obstaculo_firme(tablero[i + di][j + dj]);
            if (tablero[i + di][j + dj] == '@') {
               tablero[i + di][j + dj] = ' ';
            }
         }
      };
      auto salta = [&](bool &enemigo) {
         if (trampolin != 0) {
            trampolin -= 1;
            for (int k = 0; k < 4 && !obstaculo_firme(tablero[i - 1][j]); ++k) {
               --i, enemigo |= (tablero[i][j] == '*');
            }
         }
      };
      auto latiga = [&](int dj) {
         if (tablero[i][j + dj] == '*') {
            tablero[i][j + dj] = ' ';
         }
      };

      for (char c : res) {
         bool enemigo = false, cayendo = !obstaculo_firme(tablero[i + 1][j]);
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
            (!cayendo ? salta(enemigo) : mueve(cayendo, 0));
         } else if (c == '<') {
            latiga(-1);
            mueve(cayendo, 0);
         } else if (c == '>') {
            latiga(+1);
            mueve(cayendo, 0);
         } else {
            error_comando++;
            band_error = true;
            break; // throw "comando ilegal";
         }

         vida -= (tablero[i][j] == '*' || enemigo);
         caidos = (cayendo ? caidos + 1 : 0);
         if (vida == 0) {
            error_vida++;
            band_error = true;
            break; // throw "vida ilegal";
         } else if (caidos > 4) {
            error_caida++;
            band_error = true;
            break; // throw "caida ilegal";
         } else if (i == salida.first && j == salida.second) {
            std::cout << "correcto\n";
            ++correctos;
            exito.push_back(indice);
            band_correcto = true;
            break;
            // return 0;
         }
      }

      if (!band_correcto) {
         if (!band_error) {
            error_longitud++;
         } else {
            band_error = false;
         }
         std::cout << "incorrecto " << std::abs(salida.first - i) << " " << std::abs(salida.second - j) << "\n";
         suma_x += std::abs(salida.second - j);
         suma_y += std::abs(salida.first - i);
         int suma = std::abs(salida.first - i) + std::abs(salida.second - j);
         // if(suma == 0){
         //    std::cout<<indice<<"\n";
         //    return 0;
         // }
         cuenta_errores[suma]++;
         ++incorrectos;
      }
   }
   // catch (const char *ex) {
   //    std::cout << ex << "\n";
   // }

   std::cout << "\n== Incorrectos: " << incorrectos << " ==\n"; // << " == " << error_comando + error_vida + error_caida + error_longitud << "\n";
   std::cout << "\tIncorrecto por Comando ilegal: " << error_comando << "\n";
   std::cout << "\tIncorrecto por Vida ilegal: " << error_vida << "\n";
   std::cout << "\tIncorrecto por Caida ilegal: " << error_caida << "\n";
   std::cout << "\tIncorrecto por Nunca llego: " << error_longitud << "\n\n";

   std::cout << "Promedio error x: " << double(suma_x) / double(incorrectos) << "\n";
   std::cout << "Promedio error y: " << double(suma_y) / double(incorrectos) << "\n";
   std::cout << "Promedio error: " << double(suma_x + suma_y) / double(incorrectos) << "\n\n";

   std::cout << "Datos de errores (a cuantos bloques en distancia manhattan se quedo de llegar -> cantidad de niveles):\n";
   for (auto iter = cuenta_errores.begin(); iter != cuenta_errores.end(); ++iter) {
      std::cout << iter->first << "\t->\t" << iter->second << "\n";
   }
   std::cout << "Total:\t\t" << incorrectos << "\n";


   std::cout << "\n\n== Correctos: " << correctos << " ==\n";
   std::cout << "Indices de los correctos:\n";
   int temp = 0;
   for (int e : exito) {
      std::cout << e << "\t";
      ++temp;
      if(temp%10 == 0){
         std::cout<<"\n";
      }
   }
}
