/**
 * @file navegacao.cpp
 * @brief Implementação da máquina de estados e handlers de navegação
 */

#include "navegacao.h"
#include "botoes.h"
#include "config.h"
#include "deep_sleep.h"

// ─── ESTADO GLOBAL ───────────────────────────────────────
Estado estado = MENU_LIVROS;

// ─── LIVROS ──────────────────────────────────────────────
const char* livros[] = {
  "Dom Quixote", "Memorias Postumas", "1984",
  "O Pequeno Principe", "Neuromancer"
};
const int NUM_LIVROS = 5;
int livroAtual = 0;

// ─── PÁGINAS ─────────────────────────────────────────────
int paginaAtual = 1;
int totalPaginas = 42;

// ─── HANDLERS ────────────────────────────────────────────

void handleMenu() {
  if (botoes[0].pressed) {
    livroAtual = (livroAtual - 1 + NUM_LIVROS) % NUM_LIVROS;
    atualizarTimer();
  }
  if (botoes[1].pressed) {
    livroAtual = (livroAtual + 1) % NUM_LIVROS;
    atualizarTimer();
  }
  if (botoes[2].pressed) {
    paginaAtual = 1;
    estado = LENDO;
    atualizarTimer();
  }
  if (botoes[2].longPressed) {
    configItem = 0;
    estado = CONFIG;
    atualizarTimer();
  }
}

void handleLendo() {
  if (botoes[0].pressed && paginaAtual > 1) {
    paginaAtual--;
    atualizarTimer();
  }
  if (botoes[1].pressed && paginaAtual < totalPaginas) {
    paginaAtual++;
    atualizarTimer();
  }
  if (botoes[2].pressed) {
    estado = MENU_LIVROS;
    atualizarTimer();
  }
  if (botoes[2].longPressed) {
    configItem = 0;
    estado = CONFIG;
    atualizarTimer();
  }
}

void handleConfig() {
  if (botoes[0].pressed) {
    configItem = (configItem - 1 + 5) % 5;
    atualizarTimer();
  }
  if (botoes[1].pressed) {
    configItem = (configItem + 1) % 5;
    atualizarTimer();
  }
  if (botoes[2].pressed) {
    (*configValues[configItem]) += 5;
    if (*configValues[configItem] > 255) *configValues[configItem] = 0;
    atualizarTimer();
  }
  if (botoes[2].longPressed) {
    estado = (paginaAtual > 0) ? LENDO : MENU_LIVROS;
    atualizarTimer();
  }
}
