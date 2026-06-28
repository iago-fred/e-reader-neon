/**
 * @file tela.h
 * @brief Funções de renderização da interface do e-reader
 *
 * Atualmente a saída é feita via Serial (modo de desenvolvimento).
 * Quando a tela e-ink WeAct 3.7" chegar, as implementações serão
 * substituídas para usar GxEPD2 + U8g2.
 *
 * Cada estado da aplicação tem seu próprio layout textual:
 * - MENU_LIVROS: lista de livros com destaque no atual
 * - LENDO: página do livro com numeração
 * - CONFIG: itens de configuração editáveis
 *
 * @see tela.cpp
 */

#ifndef TELA_H
#define TELA_H

/**
 * @brief Renderiza a tela inteira de acordo com o estado atual
 *
 * Lê as variáveis globais de estado (Estado estado, livroAtual,
 * paginaAtual, configItem, etc.) e desenha o layout correspondente
 * no Serial Monitor.
 *
 * Deve ser chamada sempre que houver uma mudança de estado ou
 * ação do usuário que exija redesenho.
 */
void renderizar();

#endif // TELA_H
