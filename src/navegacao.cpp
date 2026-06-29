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

// ─── CONFIG MENU ───────────────────────────────────────────
bool editMode = false;

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
  if (editMode) {
    // ─── MODO EDIÇÃO: ▲/▼ ajustam valor ● curto volta ───
    if (botoes[0].pressed) {
      switch (configItem) {
        case 0: if (config.margem < 50) config.margem++; break;
        case 1: if (config.espacamento < 20) config.espacamento++; break;
        case 2: if (config.tempo_tela < 30) config.tempo_tela++; break;
        case 3:
          config.fonte = (config.fonte == 14) ? 18 : (config.fonte == 18) ? 24 : 14;
          break;
        case 4: if (config.leds < 100) config.leds += 5; break;
      }
      atualizarTimer();
    }
    if (botoes[1].pressed) {
      switch (configItem) {
        case 0: if (config.margem > 0) config.margem--; break;
        case 1: if (config.espacamento > 0) config.espacamento--; break;
        case 2: if (config.tempo_tela > 1) config.tempo_tela--; break;
        case 3:
          config.fonte = (config.fonte == 24) ? 18 : (config.fonte == 18) ? 14 : 24;
          break;
        case 4: if (config.leds > 0) config.leds -= 5; break;
      }
      atualizarTimer();
    }
    if (botoes[2].pressed) {
      editMode = false;
      atualizarTimer();
    }
    if (botoes[2].longPressed) {
      editMode = false;
      estado = (paginaAtual > 0) ? LENDO : MENU_LIVROS;
      atualizarTimer();
    }
  } else {
    // ─── MODO NAVEGAÇÃO: ▲/▼ navegam ● curto seleciona ───
    if (botoes[0].pressed) {
      configItem = (configItem - 1 + 5) % 5;
      atualizarTimer();
    }
    if (botoes[1].pressed) {
      configItem = (configItem + 1) % 5;
      atualizarTimer();
    }
    if (botoes[2].pressed) {
      editMode = true;
      atualizarTimer();
    }
    if (botoes[2].longPressed) {
      estado = (paginaAtual > 0) ? LENDO : MENU_LIVROS;
      atualizarTimer();
    }
  }
}
