#include"spelunky.h"
#include<algorithm>
#include<cctype>
#include<ctime>
#include<fstream>
#include<iostream>    // -Wl,--stack,1073741824}
#include<map>
#include<random>
#include<set>
#include<string>
#include<vector>

int main(){
   std::cin.tie(nullptr);
   std::ios_base::sync_with_stdio(false);

   constexpr int total_pico = 2, total_trampolin = 1, total_vidas = 3, total_caida = 4, filas = 10, columnas = 10;
   std::string carpeta; int generar;
   std::cin>>carpeta>>generar;

   std::mt19937_64 gen(std::time(nullptr));
   std::set<std::string> vistos;
   std::map<int, std::vector<std::string>> por_score;

   for (int k0 = 0; k0 <= 9; ++k0) {
      for (int k1 = k0; k1 <= 9; ++k1) {
         for (int k2 = k1; k2 <= 9; ++k2) {
            for (int i = 0; i < generar; ++i) {
               char mapa[50][100] = { };
               for (int i = 1; i < filas - 1; ++i) {
                  for (int j = 1; j < columnas - 1; ++j) {
                     int v = std::uniform_int_distribution<int>(0, 9)(gen);
                     mapa[i][j] = (v <= k0 ? ' ' : (v <= k1 ? '#' : v <= k2 ? '*' : '@'));
                  }
               }
               for (int i = 0; i < filas; ++i) {
                  mapa[i][0] = mapa[i][columnas - 1] = '#';
               }
               for (int j = 0; j < columnas; ++j) {
                  mapa[0][j] = mapa[filas - 1][j] = '#';
               }
               mapa[1][columnas / 2] = 'E';
               mapa[2][columnas / 2] = '#';
               mapa[filas - 2][columnas / 2] = 'S';

               std::string flat;
               for (int i = 0; i < filas; ++i) {
                  for (int j = 0; j < columnas; ++j) {
                     flat.push_back(mapa[i][j]);
                  }
               }
               if (!vistos.insert(flat).second) {
                  continue;
               }

               por_score[evalua(mapa, filas, columnas, total_pico, total_trampolin, total_vidas, total_caida)].push_back(std::move(flat));
            }
         }
      }
   }

   int id = 0;
   for (const auto& [score, vec] : por_score) {
      std::cout << score << " " << vec.size( ) << "\n";
      for (int x = 0; x < std::min(int(vec.size( )), 5); ++x) {
         std::ofstream ofs(carpeta + "/nivel" + std::to_string(id++) + ".data");
         for (int i = 1; i < filas - 1; ++i) {
            for (int j = 1; j < columnas - 1; ++j) {
               char c = vec[x][i * columnas + j];
               std::cout << c;
               ofs << (std::isalpha(c) ? ' ' : c);
            }
            std::cout << "\n";
            ofs << "\n";
         }
         std::cout << score << "\n";
         ofs << score << "\n";
      }
   }
}
