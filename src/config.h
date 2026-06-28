/**
 * @file config.h
 * @brief Configurações do usuário — struct, valores padrão e itens editáveis
 *
 * Fornece a struct Config com os parâmetros ajustáveis pelo usuário
 * (margem, espaçamento, tempo de tela, fonte, LEDs) e as tabelas
 * de rótulos / ponteiros usadas pelo menu de configurações.
 *
 * @see config.cpp
 */

#ifndef CONFIG_H
#define CONFIG_H

/**
 * @struct Config
 * @brief Parâmetros ajustáveis no menu de configurações
 *
 * Valores padrão definidos inline na struct. Quando o SD card estiver
 * disponível, estes valores serão sobrescritos pelo /config.json.
 */
struct Config {
  int margem = 10;       /**< Margem lateral da tela (px) */
  int espacamento = 4;   /**< Espaçamento entre linhas (px) */
  int tempo_tela = 5;    /**< Tempo máximo de tela antes do sleep (min) */
  int fonte = 22;        /**< Tamanho da fonte (índice 14/18/24) */
  int leds = 50;         /**< Intensidade dos LEDs de frontlight (0-100%) */
};

/** Instância global da configuração */
extern Config config;

/** Índice do item atualmente selecionado no menu de configurações */
extern int configItem;

/** Rótulos dos itens de configuração (5 itens) */
extern const char* configLabels[5];

/** Ponteiros para os valores de cada item (permite alteração direta) */
extern int* configValues[5];

#endif // CONFIG_H
