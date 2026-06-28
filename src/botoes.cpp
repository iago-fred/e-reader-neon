/**
 * @file botoes.cpp
 * @brief Implementação da leitura dos botões com debounce e long-press
 */

#include "botoes.h"

Botao botoes[3] = {
  { BTN_CIMA,  HIGH, 0, false, false },
  { BTN_BAIXO, HIGH, 0, false, false },
  { BTN_SEL,   HIGH, 0, false, false },
};

void initBotoes() {
  pinMode(BTN_CIMA, INPUT_PULLUP);
  pinMode(BTN_BAIXO, INPUT_PULLUP);
  pinMode(BTN_SEL, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
}

void lerBotoes() {
  unsigned long now = millis();
  static unsigned long pressStart[3] = {0};
  static bool longPressReported[3] = {false};

  for (int i = 0; i < 3; i++) {
    int reading = digitalRead(botoes[i].pin);
    botoes[i].pressed = false;
    botoes[i].longPressed = false;

    // Detectou transição → reinicia timer de debounce
    if (reading != botoes[i].lastState) {
      botoes[i].lastDebounce = now;
    }

    // Passou a janela de debounce → estado estável
    if ((now - botoes[i].lastDebounce) > DEBOUNCE_MS) {
      if (reading == LOW) {
        // Botão pressionado
        if (pressStart[i] == 0) {
          pressStart[i] = now;
          longPressReported[i] = false;
        }
        // Verifica se já passou do limiar de long-press
        if (!longPressReported[i] && (now - pressStart[i]) >= LONG_PRESS_MS) {
          botoes[i].longPressed = true;
          longPressReported[i] = true;
        }
      } else {
        // Botão solto — se não foi long-press, é pressionamento curto
        if (pressStart[i] != 0 && !longPressReported[i]) {
          botoes[i].pressed = true;
        }
        pressStart[i] = 0;
        longPressReported[i] = false;
      }
    }
    botoes[i].lastState = reading;
  }
}
