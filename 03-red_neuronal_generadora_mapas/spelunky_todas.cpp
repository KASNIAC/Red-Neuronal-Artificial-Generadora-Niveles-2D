#include"spelunky.h"
#include<algorithm>
#include<iostream>    // -Wl,--stack,1073741824
#include<string>
#include<vector>

int main(){
   std::cin.tie(nullptr);
   std::ios_base::sync_with_stdio(false);

   int total_pico, total_trampolin, total_vidas;
   std::cin>>total_pico>>total_trampolin>>total_vidas;

   char mapa[50][100];
   std::string s;
   int filas = 0, columnas = 0;
   while(std::getline(std::cin, s)){
      std::copy(s.begin( ), s.end( ), &mapa[filas++][0]);
      columnas = s.size();
   }

   auto soluciones_opt = resuelve(mapa, filas, columnas, total_pico, total_trampolin, total_vidas, false);
   for (const auto& s : soluciones_opt) {
      std::cout << s.second << "\n";
   }
}
