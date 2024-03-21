#include <cctype>
#include <climits>
#include <fstream>
#include <iostream>
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

void imprime(const std::vector<std::string>& tablero, int ki, int kj, int caidos, int pico, int trampolin, int vida, auto entrada, auto salida) {
   system("cls");
   for (int i = 0; i < tablero.size( ); ++i) {
      for (int j = 0; j < tablero[i].size( ); ++j) {
         if (i == ki && j == kj) {
            std::cout << 'K';
         } else if (i == entrada.first && j == entrada.second) {
            std::cout << 'E';
         } else if (i == salida.first && j == salida.second) {
            std::cout << 'S';
         } else {
            std::cout << tablero[i][j];
         }
      }
      std::cout << "\n";
   }
   std::cout << "C=" << caidos << ", P=" << pico << ", T=" << trampolin << ", V=" << vida << "\n";
   std::this_thread::sleep_for(std::chrono::milliseconds(333));
}

int main(int argc, const char* argv[]) {
   if (argc < 3) {
      std::cout << "Uso: " << argv[0] << " archivo_entrada archivo_cadena\n";
      return 0;
   }

   std::ifstream archivo_entrada(argv[1]);
   std::ifstream archivo_cadena(argv[2]);

   std::vector<std::string> tablero; std::string s;
   std::pair<int8_t, int8_t> entrada, salida;
   for (int8_t i = 0; std::getline(archivo_entrada >> std::ws, s); ++i) {
      tablero.push_back(s);
      for (int8_t j = 0; j < tablero[i].size( ); ++j) {
         if (std::isalpha(tablero[i][j])) {
            (tablero[i][j] == 'E' ? entrada : salida) = { i, j };
            tablero[i][j] = ' ';
         }
      }
   }

   std::string res;
   archivo_cadena >> res;

   try {
      int i = entrada.first, j = entrada.second, caidos = 0, pico = 1, trampolin = 1, vida = 5;
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
      auto salta = [&](bool& enemigo) {
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
         imprime(tablero, i, j, caidos, pico, trampolin, vida, entrada, salida);

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
            throw "comando ilegal";
         }

         vida -= (tablero[i][j] == '*' || enemigo);
         caidos = (cayendo ? caidos + 1 : 0);
         if (vida == 0) {
            throw "vida ilegal";
         } else if (caidos > 4) {
            throw "caida ilegal";
         }
      }

      imprime(tablero, i, j, caidos, pico, trampolin, vida, entrada, salida);
   } catch (const char* ex) {
      std::cout << ex << "\n";
   }
}
