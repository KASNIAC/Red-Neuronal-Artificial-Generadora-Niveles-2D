#ifndef SPELUNKY_H
#define SPELUNKY_H

#include<algorithm>
#include<bitset>
#include<cmath>
#include<iostream>
#include<string>
#include<utility>
#include<vector>

template<std::size_t N>
auto operator<=>(const std::bitset<N>& b1, const std::bitset<N>& b2) {
   static_assert(N <= 64);
   return b1.to_ullong( ) <=> b2.to_ullong( );
}

struct estado{
   int hash_destruidos = 0;
   std::bitset<16> asesinados;
   int pico, trampolin, caida = 0, vida;

   estado(int p, int t, int v)
   : pico(p), trampolin(t), vida(v) {
   }

   estado(int h, std::bitset<16> a, int p, int t, int c, int v)
   : hash_destruidos(h), asesinados(a), pico(p), trampolin(t), caida(c), vida(v) {
   }

   auto operator<=>(const estado&) const = default;
};

struct estado_cola : estado{
   int first, second;
   int indice_padre;
   char comando;

   estado_cola(int p, int t, int v)
   : estado(p, t, v) {
   }

   estado_cola(int h, std::bitset<16> a, int p, int t, int c, int v, int f, int s)
   : estado(h, a, p, t, c, v), first(f), second(s) {
   }

   estado_cola(int h, std::bitset<16> a, int p, int t, int c, int v, int f, int s, int i, char d)
   : estado(h, a, p, t, c, v), first(f), second(s), indice_padre(i), comando(d) {
   }
};

bool obstaculo_firme(char c) {
   return c == '#' || c == '@';
}

bool domina(const estado& e1, const estado& e2){
   if(e1.hash_destruidos != e2.hash_destruidos){
      return false;
   }

   bool ax = ((e1.asesinados | e2.asesinados) == e1.asesinados), ay = (ax && e1.asesinados.count() > e2.asesinados.count());
   bool tx = (e1.trampolin >= e2.trampolin), ty = (e1.trampolin > e2.trampolin);
   bool cx = (e1.caida <= e2.caida), cy = (e1.caida < e2.caida);
   bool vx = (e1.vida >= e2.vida), vy = (e1.vida > e2.vida);

   return ax && tx && cx && vx && (ay || ty || cy || vy);
}

bool acepta_estado(std::vector<estado>& actuales, const estado& nuevo) {
   if (std::binary_search(actuales.begin( ), actuales.end( ), nuevo)) {                      // si el nuevo ya estaba, no agregarlo
      return false;
   } else if (std::any_of(actuales.begin( ), actuales.end( ), [&](const estado& actual) {    // si el nuevo es dominado por alguien que ya estaba, no agregarlo
      return domina(actual, nuevo);
   })) {
      return false;
   } else {                                                                                  // sí agregarlo
      actuales.erase(std::remove_if(actuales.begin( ), actuales.end( ), [&](const estado& actual) {      // quitar cualquier viejo que sea dominado por el nuevo
         return domina(nuevo, actual);
      }), actuales.end( ));
      actuales.insert(std::lower_bound(actuales.begin( ), actuales.end( ), nuevo), nuevo);               // insertar el nuevo en orden
      return true;
   }
}

char celda_real(char mapa[50][100], int destruibles[50][100], int enemigos[50][100], const estado_cola& actual, int i, int j, const std::vector<std::vector<int>>& combs) {
   if(destruibles[i][j] != -1){
      int indice = destruibles[i][j];
      return (std::binary_search(combs[actual.hash_destruidos].begin( ), combs[actual.hash_destruidos].end( ), indice) ? ' ' : mapa[i][j]);
   } else if(enemigos[i][j] != -1){
      int indice = enemigos[i][j];
      return (actual.asesinados[indice] == 1 ? ' ' : mapa[i][j]);
   } else {
      return mapa[i][j];
   }
}

void combinaciones(int i, int n, int k, std::vector<int>& actual, std::vector<std::vector<int>>& combs) {
   if (i == n || k == 0) {
      combs.push_back(actual);
   } else {
      combinaciones(i + 1, n, k, actual, combs);
      if (k > 0) {
         actual.push_back(i);
         combinaciones(i + 1, n, k - 1, actual, combs);
         actual.pop_back( );
      }
   }
}

int nuevo_agujero(int hash_destruidos, int indice, const std::vector<std::vector<int>>& combs) {
   auto copia = combs[hash_destruidos];
   copia.push_back(indice);
   std::sort(copia.begin( ), copia.end( ));
   return std::lower_bound(combs.begin( ), combs.end( ), copia) - combs.begin( );
}

std::string resuelve(const char mapa_original[50][100], int filas, int columnas, int total_pico, int total_trampolin, int total_vidas, int total_caida) {
   estado_cola entrada(total_pico, total_trampolin, total_vidas), salida(-1, -1, -1);
   char mapa[50][100];        std::copy_n(&mapa_original[0][0], 50 * 100, &mapa[0][0]);
   int destruibles[50][100];  std::fill_n(&destruibles[0][0], 50 * 100, -1);
   int enemigos[50][100];     std::fill_n(&enemigos[0][0], 50 * 100, -1);
   int cuentaDestruibles = 0, cuentaEnemigos = 0;
   for (int f = 0; f < filas; ++f) {
      for(int c = 0; c < columnas; ++c){
         if(mapa[f][c] == 'E'){
            entrada.first = f, entrada.second = c;
            mapa[f][c] = ' ';
         } else if(mapa[f][c] == 'S'){
            salida.first = f, salida.second = c;
            mapa[f][c] = ' ';
         } else if(mapa[f][c] == '*'){
            enemigos[f][c] = cuentaEnemigos++;
         } else if(mapa[f][c] == '@'){
            destruibles[f][c] = cuentaDestruibles++;
         }
      }
   }

   std::vector<std::vector<int>> combs; std::vector<int> trabajo;    // combinaciones de posibles agujeros rotos
   combinaciones(0, cuentaDestruibles, total_pico, trabajo, combs);
   std::sort(combs.begin( ), combs.end( ));

   std::vector<estado> visitadas[50][100]; // Estados no dominados
   std::vector<estado_cola> cola = { entrada };
   visitadas[entrada.first][entrada.second].push_back(entrada);
   std::vector<std::pair<estado, std::string>> soluciones;

   for (int i = 0, t = 0; i < cola.size() && soluciones.empty(); ++t) {
      for(int tope = cola.size(); i < tope; ++i){
         estado_cola actual = cola[i];
         if(actual.first == salida.first && actual.second == salida.second){
            std::string res;
            do{
               res += cola[i].comando;
               i = cola[i].indice_padre;
            } while(i != 0);
            std::reverse(res.begin(), res.end());
            return res;
         }

         actual.vida -= (celda_real(mapa, destruibles, enemigos, actual, actual.first, actual.second, combs) == '*');
         if (actual.vida == 0) {
            continue;
         }

         if(obstaculo_firme(celda_real(mapa, destruibles, enemigos, actual, actual.first + 1, actual.second, combs))){
            // Se mueve a la izquierda o derecha
            std::vector<std::pair<int, int>> posiciones = {{actual.first, actual.second - 1}, {actual.first, actual.second + 1}};
            int j = 0;
            for(const auto& p : posiciones){
               if(celda_real(mapa, destruibles, enemigos, actual, p.first, p.second, combs) == ' ' || celda_real(mapa, destruibles, enemigos, actual, p.first, p.second, combs) == '*'){
                  auto nuevo = actual;
                  nuevo.first = p.first, nuevo.second = p.second;
                  nuevo.caida = 0;
                  if(acepta_estado(visitadas[nuevo.first][nuevo.second], nuevo)){
                     char com = (j == 0 ? 'l' : 'r');
                     cola.push_back(estado_cola{nuevo.hash_destruidos, nuevo.asesinados, nuevo.pico, nuevo.trampolin, nuevo.caida, nuevo.vida, nuevo.first, nuevo.second, i, com});
                  }

                  if(celda_real(mapa, destruibles, enemigos, actual, p.first, p.second, combs) == '*'){
                     int indice = enemigos[p.first][p.second];
                     auto nuevo = actual;
                     nuevo.asesinados[indice] = 1;
                     if(acepta_estado(visitadas[nuevo.first][nuevo.second], nuevo)){
                        char com = (j == 0 ? '<' : '>');
                        cola.push_back(estado_cola{nuevo.hash_destruidos, nuevo.asesinados, nuevo.pico, nuevo.trampolin, nuevo.caida, nuevo.vida, nuevo.first, nuevo.second, i, com});
                     }
                  }
               }
               ++j;
            }

            // Rompe bloque
            posiciones.push_back({actual.first - 1, actual.second});
            posiciones.push_back({actual.first + 1, actual.second});
            j = 0;
            for(const auto& p : posiciones){
               if(celda_real(mapa, destruibles, enemigos, actual, p.first, p.second, combs) == '@' && actual.pico > 0){
                  int indice = destruibles[p.first][p.second];
                  auto nuevo = actual;
                  nuevo.hash_destruidos = nuevo_agujero(nuevo.hash_destruidos, indice, combs);
                  nuevo.pico -= 1;
                  nuevo.caida = 0;
                  if(acepta_estado(visitadas[nuevo.first][nuevo.second], nuevo)){
                     char com = (j == 0 ? 'L' : j == 1 ? 'R' : j == 2 ? 'U' : 'D');
                     cola.push_back(estado_cola{nuevo.hash_destruidos, nuevo.asesinados, nuevo.pico, nuevo.trampolin, nuevo.caida, nuevo.vida, nuevo.first, nuevo.second, i, com});
                  }
               }
               ++j;
            }

            // Usa trampolin
            bool danio = false;
            if(actual.trampolin > 0){
               int nuevaFila;
               for(nuevaFila = actual.first - 1; nuevaFila >= actual.first - total_caida; --nuevaFila){
                  if(celda_real(mapa, destruibles, enemigos, actual, nuevaFila, actual.second, combs) == '#'  || celda_real(mapa, destruibles, enemigos, actual, nuevaFila, actual.second, combs) == '@'){
                     break;
                  }
                  if(celda_real(mapa, destruibles, enemigos, actual, nuevaFila, actual.second, combs) == '*'){
                     danio = true;
                  }
               }
               nuevaFila++;

               auto nuevo = actual;
               nuevo.first = nuevaFila;
               nuevo.trampolin -= 1;
               nuevo.caida = 0;
               nuevo.vida -= danio;
               if(acepta_estado(visitadas[nuevo.first][nuevo.second], nuevo)){
                  char com = 'T';
                  cola.push_back(estado_cola{nuevo.hash_destruidos, nuevo.asesinados, nuevo.pico, nuevo.trampolin, nuevo.caida, nuevo.vida, nuevo.first, nuevo.second, i, com});
               }
            }
         } else {
            std::vector<std::pair<int, int>> posiciones = {{actual.first + 1, actual.second}, {actual.first + 1, actual.second + 1}, {actual.first + 1, actual.second - 1}};
            int j = 0;
            for(const auto& p : posiciones){
               char celda = celda_real(mapa, destruibles, enemigos, actual, p.first, p.second, combs);
               if(actual.caida < total_caida && (celda == ' ' || celda == '*')){
                  auto nuevo = actual; //  actual.vida - (caracter_abajo == '*')
                  nuevo.first = p.first, nuevo.second = p.second;
                  nuevo.caida += 1;
                  // nuevo.vida -= bool(celda == '*');
                  if(acepta_estado(visitadas[nuevo.first][nuevo.second], nuevo)){
                     char com = (j == 0 ? 'd' : j == 1 ? 'r' : 'l');
                     cola.push_back(estado_cola{nuevo.hash_destruidos, nuevo.asesinados, nuevo.pico, nuevo.trampolin, nuevo.caida, nuevo.vida, nuevo.first, nuevo.second, i, com});
                  }
               }
               ++j;
            }
         }
      }
   }

   return "";
}

int evalua(const char mapa_original[50][100], int filas, int columnas, int total_pico, int total_trampolin, int total_vidas, int total_caida) {
   char mapa[50][100];

   std::copy_n(&mapa_original[0][0], 50 * 100, &mapa[0][0]);
   std::replace_if(&mapa[0][0], &mapa[50][0], [](char c) { return c == '@' || c == '*'; }, ' ');
   auto s1 = resuelve(mapa, filas, columnas, 0, filas * columnas, 1, filas * columnas);         // poder resolverse quitando @* usando trampolines infinitos y caida infinita
   auto s2 = resuelve(mapa, filas, columnas, 0, filas * columnas, 1, total_caida);              // poder resolverse quitando @* usando trampolines infinitos y caida normal
   auto s3 = resuelve(mapa, filas, columnas, 0, total_trampolin, 1, total_caida);               // poder resolverse quitando @* usando caida normal

   std::copy_n(&mapa_original[0][0], 50 * 100, &mapa[0][0]);
   std::replace_if(&mapa[0][0], &mapa[50][0], [](char c) { return c == '@'; }, ' ');            // poder resolverse quitando @
   auto s4 = resuelve(mapa, filas, columnas, 0, total_trampolin, 1, total_caida);

   std::copy_n(&mapa_original[0][0], 50 * 100, &mapa[0][0]);
   auto s5 = resuelve(mapa, filas, columnas, total_pico, total_trampolin, total_vidas, total_caida);                                                           // poder resolverse
   auto s6 = resuelve(mapa, filas, columnas, std::max(total_pico - 1, 0), std::max(total_trampolin - 1, 0), std::max(total_vidas - 1, 1), total_caida);        // poder resolverse con menos recursos
   auto s7 = resuelve(mapa, filas, columnas, 0, 0, 1, total_caida);                                                                                            // poder resolverse sin recursos

   std::pair<int, int> entrada, salida;
   for (int f = 0; f < filas; ++f) {
      for(int c = 0; c < columnas; ++c){
         if(mapa[f][c] == 'E'){
            entrada.first = f, entrada.second = c;
         } else if(mapa[f][c] == 'S'){
            salida.first = f, salida.second = c;
         }
      }
   }

   auto hamming = std::abs(salida.first - entrada.first) + std::abs(salida.second - entrada.second);
   auto longitud_opt = s5.size( );

   return (!s1.empty( ) << 8) | (!s2.empty( ) << 7) | (!s3.empty( ) << 6) | (!s4.empty( ) << 5) | (!s5.empty( ) << 4) | (!s6.empty( ) << 3) | (!s7.empty( ) << 2) | std::max(0b11, int((double(longitud_opt) / hamming - 1) / 0.25));
}

/*????? evalua(const char mapa_original[50][100], int filas, int columnas, int total_pico, int total_trampolin, int total_vidas, const std::string& cadena) {
   estado_cola entrada(total_trampolin, total_vidas), salida(-1, -1);
   char mapa[50][100];        std::copy_n(&mapa_original[0][0], 50 * 100, &mapa[0][0]);
   int destruibles[50][100];  std::fill_n(&destruibles[0][0], 50 * 100, -1);
   int enemigos[50][100];     std::fill_n(&enemigos[0][0], 50 * 100, -1);
   int cuentaDestruibles = 0, cuentaEnemigos = 0;
   for (int f = 0; f < filas; ++f) {
      for(int c = 0; c < columnas; ++c){
         if(mapa[f][c] == 'E'){
            entrada.first = f, entrada.second = c;
            mapa[f][c] = ' ';
         } else if(mapa[f][c] == 'S'){
            salida.first = f, salida.second = c;
            mapa[f][c] = ' ';
         } else if(mapa[f][c] == '*'){
            enemigos[f][c] = cuentaEnemigos++;
         } else if(mapa[f][c] == '@'){
            destruibles[f][c] = cuentaDestruibles++;
         }
      }
   }

   try {
      auto kasniac = entrada;
      auto& i = kasniac.first;
      auto& j = kasniac.second;
      auto& pico = kasniac.pico;
      auto& trampolin = kasniac.trampolin;
      auto& vida = kasniac.vida;
      auto& caida = kasniac.caida;

      auto mueve = [&](bool cayendo, int dj) {
         i += cayendo;
         if (!obstaculo_firme(mapa[i][j + dj])) {
            j += dj;
         }
      };
      auto rompe = [&](int di, int dj) {
         if (pico != 0) {
            pico -= obstaculo_firme(mapa[i + di][j + dj]);
            if (mapa[i + di][j + dj] == '@') {
               mapa[i + di][j + dj] = ' ';
            }
         }
      };
      auto salta = [&](bool& enemigo) {
         if (trampolin != 0) {
            trampolin -= 1;
            for (int k = 0; k < 4 && !obstaculo_firme(mapa[i - 1][j]); ++k) {
               --i, enemigo |= (mapa[i][j] == '*');
            }
         }
      };
      auto latiga = [&](int dj) {
         if (mapa[i][j + dj] == '*') {
            mapa[i][j + dj] = ' ';
         }
      };

      for (char c : res) {
         bool enemigo = false, cayendo = !obstaculo_firme(mapa[i + 1][j]);
         if (c == 'l') {
            mueve(cayendo, -1);
         } else if (c == 'r') {
            mueve(cayendo, +1);
         } else if (c == 'd') {
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

         vida -= (mapa[i][j] == '*' || enemigo);
         caida = (cayendo ? caida + 1 : 0);
         if (vida == 0) {
            throw "vida ilegal";
         } else if (caida > 4) {
            throw "caida ilegal";
         }
      }
   } catch (const char* ex) {
      std::cout << ex << "\n";
   }
}*/

#endif
