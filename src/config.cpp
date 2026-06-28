/**
 * @file config.cpp
 * @brief Implementação das configurações do usuário
 */

#include "config.h"

Config config;
int configItem = 0;

const char* configLabels[] = {
  "Margem (px)", "Espacamento (px)", "Tempo tela (min)",
  "Tamanho fonte", "Intensidade LEDs (%)"
};

int* configValues[] = {
  &config.margem, &config.espacamento, &config.tempo_tela,
  &config.fonte, &config.leds
};
