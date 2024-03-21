#include "spelunky.h"
#include <algorithm>
#include <fstream>
#include <iostream> // -Wl,--stack,1073741824
#include <map>
#include <string>
#include <vector>
#include <iomanip> 

int main() {
   std::cin.tie(nullptr);
   std::ios_base::sync_with_stdio(false);

   constexpr int total_pico = 2, total_trampolin = 1, total_vidas = 3, total_caida = 4, filas = 10, columnas = 10;

   // std::string carpeta;
   // std::cin >> carpeta;
   std::vector<std::string> carpetas = {"entrenamiento", "validacion", "pruebas"};
   std::map<std::string, std::string> vistos; // std::set<std::string> vistos;

   int total = 0, no_repetidos = 0, repetidos = 0;
   int dummy, ok = 0, mal = 0;

   size_t cadena_maxima = 0;
   while (std::cin >> std::ws >> dummy) {
      // std::cout<<contador<<" -> "<<carpeta<<"\n";
      ++total;
      char mapa[50][100];
      for (int i = 0; i < filas; ++i) {
         std::string s;
         std::getline(std::cin >> std::ws, s);
         std::copy(s.begin(), s.end(), &mapa[i][0]);
      }

      std::string flat;
      for (int i = 0; i < filas; ++i) {
         for (int j = 0; j < columnas; ++j) {
            flat.push_back(mapa[i][j]);
         }
      }

      auto solucion = resuelve(mapa, filas, columnas, total_pico, total_trampolin, total_vidas, 4);

      std::cout << dummy << " " << solucion << "\n";
      ok += !solucion.empty();
      mal += solucion.empty();

      cadena_maxima = std::max(cadena_maxima, solucion.size());

      if (solucion.size() == 0 || !vistos.insert({flat, solucion}).second) {
         continue;
      }
      ++no_repetidos;
   }

   int cuantos = vistos.size() / 3; // int cuantos = (16 * 200) / 2;
   auto iter = vistos.begin();
   for (auto &carpeta : carpetas) {
      for (int contador = 0; contador < cuantos; ++contador) {
         std::ofstream ofs(carpeta + "/ejemplo" + std::to_string(contador) + ".txt");
         for (int i = 1; i < filas - 1; ++i) {
            std::string renglon(iter->first, columnas * i + 1, columnas - 2);
            ofs << renglon << "\n" ;
         }
         ofs << std::left << std::setw(cadena_maxima) << std::setfill('-') << iter->second << "\n";

         ++iter;
      }
   }

   std::cout << "Bien: " << ok << "\n";
   std::cout << "Mal: " << mal << "\n\n";

   std::cout << "Total: " << total << "\n";
   std::cout << "No repetidos: " << no_repetidos << "\n";
   std::cout << "Repetidos: " << total - no_repetidos << "\n";
   std::cout << "Total casos: " << vistos.size() <<"\n";       // Debe de concidir con no_repetidos
   std::cout << "Solucion maxima: " << cadena_maxima <<"\n";
}