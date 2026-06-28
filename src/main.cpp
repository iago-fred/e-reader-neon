/**
 * @file main.cpp
 * @brief Ponto de entrada — setup() e loop() do e-reader
 *
 * Plataforma: ESP32 Dev Module
 * Framework: Arduino (via PlatformIO)
 *
 * Inicializa a Serial, configura os pinos dos botões, e no loop()
 * coordena a leitura dos botões, verificação de sleep, máquina de
 * estados e renderização.
 *
 * @see botoes.h, navegacao.h, tela.h, config.h, deep_sleep.h
 */

#include <Arduino.h>
#include "botoes.h"
#include "navegacao.h"
#include "tela.h"
#include "deep_sleep.h"

// ─── SETUP ───────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("\n══════════════════════════════════");
  Serial.println("  📖 E-Reader NEON — Teste Serial");
  Serial.println("══════════════════════════════════");
  Serial.println("\n🔌 GPIO 32=▲  33=▼  25=●  (PULLUP)");

  initBotoes();
  digitalWrite(LED_PIN, HIGH);
  delay(300);
  digitalWrite(LED_PIN, LOW);

  atualizarTimer();
  renderizar();
}

// ─── LOOP ────────────────────────────────────────────────
void loop() {
  lerBotoes();
  verificarSleep();

  switch (estado) {
    case MENU_LIVROS:
      handleMenu();
      break;
    case LENDO:
      handleLendo();
      break;
    case CONFIG:
      handleConfig();
      break;
    default:
      break;
  }

  // Redesenha apenas se houve interação do usuário
  if (botoes[0].pressed || botoes[1].pressed ||
      botoes[2].pressed || botoes[2].longPressed) {
    renderizar();
  }

  // LED piscando — heartbeat indicando que o ESP32 está acordado
  digitalWrite(LED_PIN, (millis() % 2000 < 100) ? HIGH : LOW);

  delay(20);
}
