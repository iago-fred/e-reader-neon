/**
 * @file deep_sleep.h
 * @brief Gerenciamento de deep sleep — timer de inatividade e wake por botão
 *
 * Monitora o tempo desde a última atividade do usuário. Quando o
 * período de inatividade ultrapassa IDLE_SLEEP_MS, o ESP32 entra
 * em deep sleep. O wake é feito pelo botão ● (GPIO 25, EXT0).
 *
 * @see deep_sleep.cpp
 */

#ifndef DEEP_SLEEP_H
#define DEEP_SLEEP_H

#include <Arduino.h>

/** Tempo de inatividade antes de entrar em deep sleep (ms) — 30 segundos */
#define IDLE_SLEEP_MS   30000

/**
 * @brief Timestamp (millis()) da última atividade do usuário
 *
 * Atualizado via atualizarTimer() sempre que o usuário interage.
 * Comparado com millis() em verificarSleep() para decidir o sleep.
 */
extern unsigned long lastActivity;

/**
 * @brief Atualiza o marcador de atividade para o instante atual
 *
 * Deve ser chamada sempre que o usuário pressionar um botão
 * (na prática, dentro das funções handleMenu / handleLendo / handleConfig).
 */
void atualizarTimer();

/**
 * @brief Entra em deep sleep com wake por botão ● (GPIO 25)
 *
 * Configura EXT0 wake no GPIO 25 (LOW), exibe mensagem no Serial,
 * faz flush e entra em deep sleep. Ao pressionar ● o ESP32 reinicia
 * o setup().
 */
void entrarDeepSleep();

/**
 * @brief Verifica se o tempo de inatividade expirou e, se sim, dorme
 *
 * Deve ser chamada uma vez por iteração do loop(), antes de processar
 * os estados. Se millis() - lastActivity > IDLE_SLEEP_MS e o estado
 * atual não for DORMINDO, chama entrarDeepSleep().
 */
void verificarSleep();

#endif // DEEP_SLEEP_H
