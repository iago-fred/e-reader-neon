/**
 * @file deep_sleep.cpp
 * @brief Implementação do deep sleep e timer de inatividade
 */

#include "deep_sleep.h"
#include "navegacao.h"   // para acessar estado e verificar DORMINDO

unsigned long lastActivity = 0;

void atualizarTimer() {
  lastActivity = millis();
}

void entrarDeepSleep() {
  Serial.println("\n💤 DEEP SLEEP... Pressione ● para acordar!");
  delay(100);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_25, LOW);
  Serial.flush();
  esp_deep_sleep_start();
}

void verificarSleep() {
  if (millis() - lastActivity > IDLE_SLEEP_MS && estado != DORMINDO) {
    entrarDeepSleep();
  }
}
