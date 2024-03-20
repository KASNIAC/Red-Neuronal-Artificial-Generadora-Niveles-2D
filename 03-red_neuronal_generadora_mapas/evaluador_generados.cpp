#include"spelunky.h"
#include<algorithm>
#include<iostream>    // -Wl,--stack,1073741824
#include<string>
#include<vector>

int main(){
   std::cin.tie(nullptr);
   std::ios_base::sync_with_stdio(false);

   constexpr int total_pico = 2, total_trampolin = 1, total_vidas = 3, total_caida = 4, filas = 10, columnas = 10;

   int dummy, ok = 0, mal = 0;
   while (std::cin >> std::ws >> dummy) {
      char mapa[50][100];
      for (int i = 0; i < filas; ++i) {
         std::string s;
         std::getline(std::cin >> std::ws, s);
         std::copy(s.begin( ), s.end( ), &mapa[i][0]);
      }

      auto solucion = resuelve(mapa, filas, columnas, total_pico, total_trampolin, total_vidas, 4);
      std::cout << dummy << " " << solucion << "\n";
      ok += !solucion.empty( );
      mal += solucion.empty( );
   }

   std::cout << ok << " " << mal << "\n";
}
