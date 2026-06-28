/**
 * @file botoes.h
 * @brief Leitura dos 3 botões físicos com debounce e detecção de long-press
 *
 * Gerencia GPIO 32 (▲ Próximo), 33 (▼ Anterior) e 25 (● Selecionar),
 * todos configurados como INPUT_PULLUP.
 *
 * Oferece debounce por software (50 ms) e detecção de pressão longa
 * (>1 s) usando temporização com millis() — sem interrupções.
 *
 * @see botoes.cpp
 */

#ifndef BOTOES_H
#define BOTOES_H

#include <Arduino.h>

// ─── PINAGEM ─────────────────────────────────────────────
#define BTN_CIMA    32   /**< GPIO do botão ▲ (Próximo) — INPUT_PULLUP */
#define BTN_BAIXO   33   /**< GPIO do botão ▼ (Anterior) — INPUT_PULLUP */
#define BTN_SEL     25   /**< GPIO do botão ● (Selecionar) — INPUT_PULLUP */
#define LED_PIN     2    /**< GPIO do LED interno da placa ESP32 */

// ─── TIMING ──────────────────────────────────────────────
#define DEBOUNCE_MS     50    /**< Janela de debounce (ms) */
#define LONG_PRESS_MS   1000  /**< Tempo para considerar pressão longa (ms) */

/**
 * @struct Botao
 * @brief Estado de um botão individual
 *
 * Mantém o pino, último estado lido, timestamp do último debounce
 * e flags de pressionamento curto e longo resetadas a cada ciclo
 * da função lerBotoes().
 */
struct Botao {
  int pin;                    /**< Número do GPIO */
  int lastState;             /**< Último estado lido (HIGH/LOW) */
  unsigned long lastDebounce; /**< millis() da última transição */
  bool pressed;              /**< True se houve pressionamento curto neste ciclo */
  bool longPressed;          /**< True se houve pressionamento longo neste ciclo */
};

/** Array com os 3 botões (índices: 0=CIMA, 1=BAIXO, 2=SEL) */
extern Botao botoes[3];

/**
 * @brief Inicializa os pinos dos botões e LED
 *
 * Configura GPIO 32, 33, 25 como INPUT_PULLUP e GPIO 2 como OUTPUT.
 * Deve ser chamada uma vez no setup().
 */
void initBotoes();

/**
 * @brief Lê e processa o estado de todos os botões
 *
 * Deve ser chamada a cada iteração do loop(). Implementa:
 * - Debounce por software com janela de DEBOUNCE_MS
 * - Detecção de pressionamento curto (pressed) ao soltar o botão
 * - Detecção de pressionamento longo (longPressed) após LONG_PRESS_MS
 *
 * As flags pressed / longPressed são válidas apenas durante o ciclo
 * atual do loop() — o código consumidor deve processá-las imediatamente.
 */
void lerBotoes();

#endif // BOTOES_H
