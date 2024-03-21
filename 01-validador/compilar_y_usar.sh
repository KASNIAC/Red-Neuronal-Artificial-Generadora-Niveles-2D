#!/bin/bash

g++ validador-grafico-flexible.cpp -std=c++20 -O3 -o validador-grafico-flexible
./validador-grafico-flexible 03-arg01-entrada.txt 03-arg02-cadena.txt
