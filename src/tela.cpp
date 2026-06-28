/**
 * @file tela.cpp
 * @brief Implementação da renderização da interface
 */

#include "tela.h"
#include "navegacao.h"
#include "config.h"

void renderizar() {
  Serial.println("\n┌────────────────────────────────┐");

  switch (estado) {
    case MENU_LIVROS:
      Serial.println("│     📖  BIBLIOTECA           │");
      Serial.println("├────────────────────────────────┤");
      for (int i = 0; i < NUM_LIVROS; i++) {
        if (i == livroAtual) {
          Serial.printf("│  ▶ %-26s│\n", livros[i]);
        } else {
          Serial.printf("│    %-26s│\n", livros[i]);
        }
      }
      Serial.println("├────────────────────────────────┤");
      Serial.println("│  ▲/▼ = navegar  ● = abrir     │");
      Serial.println("│  ● (>1s) = Config             │");
      break;

    case LENDO:
      Serial.printf("│  📄 %-24s │\n", livros[livroAtual]);
      Serial.println("├────────────────────────────────┤");
      Serial.printf("│         Pagina %d/%-3d        │\n", paginaAtual, totalPaginas);
      Serial.println("│  Lorem ipsum dolor sit amet,  │");
      Serial.println("│  consectetur adipiscing...    │");
      Serial.println("├────────────────────────────────┤");
      Serial.println("│  ▲/▼ = virar pagina           │");
      Serial.println("│  ● = voltar | ● (1s) = Config │");
      break;

    case CONFIG:
      Serial.println("│  ⚙️  CONFIGURACOES            │");
      Serial.println("├────────────────────────────────┤");
      for (int i = 0; i < 5; i++) {
        if (i == configItem) {
          Serial.printf("│  ▶ %s: %-2d            │\n", configLabels[i], *configValues[i]);
        } else {
          Serial.printf("│    %s: %-2d            │\n", configLabels[i], *configValues[i]);
        }
      }
      Serial.println("├────────────────────────────────┤");
      Serial.println("│  ● curto = alterar | longo =  │");
      Serial.println("│  voltar ao menu               │");
      break;

    default:
      break;
  }

  Serial.println("└────────────────────────────────┘");
}
