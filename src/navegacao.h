/**
 * @file navegacao.h
 * @brief Máquina de estados principal — navegação entre telas do e-reader
 *
 * Define os estados da aplicação (MENU_LIVROS, LENDO, CONFIG, DORMINDO)
 * e as funções que processam a entrada do usuário em cada estado.
 *
 * As funções handle* são chamadas pelo loop principal e dependem do
 * estado atual dos botões (lido previamente por lerBotoes()) e do
 * módulo de configuração.
 *
 * @see navegacao.cpp
 */

#ifndef NAVEGACAO_H
#define NAVEGACAO_H

#include <Arduino.h>

// ─── ESTADOS ─────────────────────────────────────────────
/**
 * @enum Estado
 * @brief Estados possíveis da máquina de estados do e-reader
 */
enum Estado {
  MENU_LIVROS,  /**< Lista de livros disponíveis */
  LENDO,        /**< Leitura de um livro aberto */
  CONFIG,       /**< Menu de configurações */
  DORMINDO      /**< Em transição para deep sleep (não processa entrada) */
};

/** Estado atual da aplicação — modificado pelas funções handle* */
extern Estado estado;

// ─── LIVROS ──────────────────────────────────────────────
/** Lista de livros (fake, usado enquanto a tela e-ink não chega) */
extern const char* livros[];
/** Quantidade de livros na lista */
extern const int NUM_LIVROS;
/** Índice do livro atualmente selecionado no menu */
extern int livroAtual;

// ─── PAGINAÇÃO ───────────────────────────────────────────
/** Página atual do livro em leitura (1-indexed) */
extern int paginaAtual;
/** Total de páginas do livro em leitura */
extern int totalPaginas;

// ─── CONFIG MENU ─────────────────────────────────────────
/** Indica se o menu de configurações está em modo de edição */
extern bool editMode;

// ─── HANDLERS ────────────────────────────────────────────

/**
 * @brief Processa entrada do usuário no estado MENU_LIVROS
 *
 * ▲ = livro anterior (cíclico)
 * ▼ = próximo livro (cíclico)
 * ● curto = abre o livro selecionado (vai para LENDO)
 * ● longo = vai para CONFIG
 */
void handleMenu();

/**
 * @brief Processa entrada do usuário no estado LENDO
 *
 * ▲ = página anterior (limitado a 1)
 * ▼ = próxima página (limitado a totalPaginas)
 * ● curto = volta para MENU_LIVROS
 * ● longo = vai para CONFIG
 */
void handleLendo();

/**
 * @brief Processa entrada do usuário no estado CONFIG
 *
 * ▲ = item anterior (cíclico entre 5 itens)
 * ▼ = próximo item (cíclico)
 * ● curto = incrementa o valor do item atual
 * ● longo = volta para o estado anterior (LENDO ou MENU_LIVROS)
 */
void handleConfig();

#endif // NAVEGACAO_H
