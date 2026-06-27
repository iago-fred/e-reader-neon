/*
 * 📖 E-Reader Neon — Teste de Botões + Navegação Serial
 * 
 * Testa antes da tela e-ink chegar:
 * - Leitura dos 3 botões (GPIO 32, 33, 25)
 * - Menu fake via Serial
 * - Deep sleep + wake por botão
 * 
 * Montagem:
 *   Botão ▲ (Próximo)  → GPIO 32 → GND (INPUT_PULLUP)
 *   Botão ▼ (Anterior) → GPIO 33 → GND (INPUT_PULLUP)
 *   Botão ● (Selecionar) → GPIO 25 → GND (INPUT_PULLUP)
 *   Plugar no USB e abrir Serial Monitor (115200 baud)
 */

#include <Arduino.h>

// ─── PINAGEM ──────────────────────────────────────────────
#define BTN_CIMA    32
#define BTN_BAIXO   33
#define BTN_SEL     25
#define LED_PIN     2   // LED interno do ESP32 (feedback visual)

// ─── DEBOUNCE ─────────────────────────────────────────────
#define DEBOUNCE_MS     50      // ms de debounce
#define LONG_PRESS_MS   1000    // ms pra considerar "pressão longa"
#define IDLE_SLEEP_MS   30000   // 30s sem input → deep sleep (pro teste)

// ─── ESTADOS ──────────────────────────────────────────────
enum Estado {
  MENU_LIVROS,
  LENDO,
  CONFIG,
  DORMINDO
};

Estado estado = MENU_LIVROS;

// ─── LIVROS FAKE (pro teste) ──────────────────────────────
const char* livros[] = {
  "Dom Quixote",
  "Memorias Postumas",
  "1984",
  "O Pequeno Principe",
  "Neuromancer"
};
const int NUM_LIVROS = 5;
int livroAtual = 0;

// ─── CONFIG FAKE ──────────────────────────────────────────
struct Config {
  int margem = 10;
  int espacamento = 4;
  int tempo_tela = 5;
  int fonte = 22;
  int leds = 50;
} config;

int configItem = 0;
const char* configLabels[] = {
  "Margem (px)",
  "Espacamento (px)",
  "Tempo tela (min)",
  "Tamanho fonte",
  "Intensidade LEDs (%)"
};
int* configValues[] = {
  &config.margem,
  &config.espacamento,
  &config.tempo_tela,
  &config.fonte,
  &config.leds
};

// ─── PÁGINA FAKE ──────────────────────────────────────────
int paginaAtual = 1;
int totalPaginas = 42;

// ─── DEBOUNCE ─────────────────────────────────────────────
struct Botao {
  int pin;
  int lastState;
  unsigned long lastDebounce;
  bool pressed;
  bool longPressed;
};

Botao botoes[3] = {
  { BTN_CIMA,   HIGH, 0, false, false },
  { BTN_BAIXO,  HIGH, 0, false, false },
  { BTN_SEL,    HIGH, 0, false, false },
};

// ─── TIMER DE INATIVIDADE ────────────────────────────────
unsigned long lastActivity = 0;

void atualizarTimer() {
  lastActivity = millis();
}

// ─── DEEP SLEEP ──────────────────────────────────────────
void entrarDeepSleep() {
  Serial.println("\n💤 DEEP SLEEP... Pressione qualquer botao para acordar!");
  delay(100);

  // Wake pelo botão ● (Select) via ext0
  // ⚠️ ext1 com ALL_LOW exigiria os 3 botões JUNTOS (bug)
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_25, LOW);

  estado = DORMINDO;
  Serial.flush();
  esp_deep_sleep_start();
}

// ─── LEITURA DOS BOTÕES ──────────────────────────────────
void lerBotoes() {
  unsigned long now = millis();

  for (int i = 0; i < 3; i++) {
    int reading = digitalRead(botoes[i].pin);
    botoes[i].pressed = false;
    botoes[i].longPressed = false;

    if (reading != botoes[i].lastState) {
      botoes[i].lastDebounce = now;
    }

    if ((now - botoes[i].lastDebounce) > DEBOUNCE_MS) {
      if (reading == LOW) {
        // Botão pressionado
        static unsigned long pressStart[3] = {0};
        static bool longPressReported[3] = {false};

        if (pressStart[i] == 0) {
          pressStart[i] = now;
          longPressReported[i] = false;
        }

        if (!longPressReported[i] && (now - pressStart[i]) >= LONG_PRESS_MS) {
          botoes[i].longPressed = true;
          longPressReported[i] = true;
        }
      } else {
        // Botão soltou
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

// ─── RENDER ──────────────────────────────────────────────
void renderizar() {
  Serial.println("\n┌────────────────────────────────┐");

  switch (estado) {
    case MENU_LIVROS:
      Serial.println("│     📖  BIBLIOTECA           │");
      Serial.println("├────────────────────────────────┤");
      for (int i = 0; i < NUM_LIVROS; i++) {
        if (i == livroAtual) {
          Serial.printf("│  ▶ %s", livros[i]);
          int espacos = 30 - 6 - strlen(livros[i]);
          for (int e = 0; e < espacos; e++) Serial.print(" ");
          Serial.println("│");
        } else {
          Serial.printf("│    %s", livros[i]);
          int espacos = 30 - 6 - strlen(livros[i]);
          for (int e = 0; e < espacos; e++) Serial.print(" ");
          Serial.println("│");
        }
      }
      Serial.println("├────────────────────────────────┤");
      Serial.println("│  ▲/▼ = navegar  ● = abrir     │");
      Serial.println("│  ● (>1s) = Config             │");
      break;

    case LENDO:
      Serial.printf("│  📄 %-23s │\n", livros[livroAtual]);
      Serial.println("├────────────────────────────────┤");
      Serial.printf("│         Pagina %d/%d           │\n", paginaAtual, totalPaginas);
      Serial.println("│                                │");
      Serial.println("│  Lorem ipsum dolor sit amet,   │");
      Serial.println("│  consectetur adipiscing elit,  │");
      Serial.println("│  sed do eiusmod tempor...      │");
      Serial.println("│                                │");
      Serial.println("├────────────────────────────────┤");
      Serial.println("│  ▲/▼ = virar pagina            │");
      Serial.println("│  ● = voltar ao menu            │");
      Serial.println("│  ● (>1s) = Config              │");
      break;

    case CONFIG:
      Serial.println("│  ⚙️  CONFIGURACOES            │");
      Serial.println("├────────────────────────────────┤");
      for (int i = 0; i < 5; i++) {
        if (i == configItem) {
          Serial.printf("│  ▶ %s: %d", configLabels[i], *configValues[i]);
        } else {
          Serial.printf("│    %s: %d", configLabels[i], *configValues[i]);
        }
        int espacos = 30 - 6 - strlen(configLabels[i]) - 2;
        if (*configValues[i] >= 10) espacos--;
        if (*configValues[i] >= 100) espacos--;
        for (int e = 0; e < espacos; e++) Serial.print(" ");
        Serial.println("│");
      }
      Serial.println("├────────────────────────────────┤");
      Serial.println("│  ▲/▼ = navegar                │");
      Serial.println("│  ● curto = alterar valor      │");
      Serial.println("│  ● longo = voltar             │");
      break;

    default:
      break;
  }

  Serial.println("└────────────────────────────────┘");
}

// ─── HANDLERS DE BOTÃO POR ESTADO ────────────────────────
void handleMenu() {
  if (botoes[0].pressed) {          // ▲ = anterior
    livroAtual = (livroAtual - 1 + NUM_LIVROS) % NUM_LIVROS;
    Serial.printf("📖 Livro selecionado: %s\n", livros[livroAtual]);
    atualizarTimer();
  }
  if (botoes[1].pressed) {          // ▼ = próximo
    livroAtual = (livroAtual + 1) % NUM_LIVROS;
    Serial.printf("📖 Livro selecionado: %s\n", livros[livroAtual]);
    atualizarTimer();
  }
  if (botoes[2].pressed) {          // ● curto = abrir
    paginaAtual = 1;
    estado = LENDO;
    Serial.printf("\n📖 Abrindo \"%s\"...\n", livros[livroAtual]);
    atualizarTimer();
  }
  if (botoes[2].longPressed) {      // ● longo = config
    configItem = 0;
    estado = CONFIG;
    Serial.println("\n⚙️  Abrindo configuracoes...");
    atualizarTimer();
  }
}

void handleLendo() {
  if (botoes[0].pressed) {          // ▲ = página anterior
    if (paginaAtual > 1) {
      paginaAtual--;
      Serial.printf("⬆ Pagina %d/%d\n", paginaAtual, totalPaginas);
    } else {
      Serial.println("⛔ Ja esta na primeira pagina");
    }
    atualizarTimer();
  }
  if (botoes[1].pressed) {          // ▼ = próxima página
    if (paginaAtual < totalPaginas) {
      paginaAtual++;
      Serial.printf("⬇ Pagina %d/%d\n", paginaAtual, totalPaginas);
    } else {
      Serial.println("⛔ Ja esta na ultima pagina");
    }
    atualizarTimer();
  }
  if (botoes[2].pressed) {          // ● curto = menu
    estado = MENU_LIVROS;
    Serial.println("\n📚 Voltando a biblioteca...");
    atualizarTimer();
  }
  if (botoes[2].longPressed) {      // ● longo = config
    configItem = 0;
    estado = CONFIG;
    Serial.println("\n⚙️  Abrindo configuracoes...");
    atualizarTimer();
  }
}

void handleConfig() {
  if (botoes[0].pressed) {          // ▲ = item anterior
    configItem = (configItem - 1 + 5) % 5;
    atualizarTimer();
  }
  if (botoes[1].pressed) {          // ▼ = próximo item
    configItem = (configItem + 1) % 5;
    atualizarTimer();
  }
  if (botoes[2].pressed) {          // ● curto = altera valor
    (*configValues[configItem]) += 5;
    if (*configValues[configItem] > 255) *configValues[configItem] = 0;
    Serial.printf("⚙️  %s alterado para %d\n", configLabels[configItem], *configValues[configItem]);
    atualizarTimer();
  }
  if (botoes[2].longPressed) {      // ● longo = voltar
    Serial.println("\n📚 Voltando...");
    if (paginaAtual > 0) {
      estado = LENDO;
    } else {
      estado = MENU_LIVROS;
    }
    atualizarTimer();
  }
}

// ─── SETUP ───────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\n══════════════════════════════════");
  Serial.println("  📖 E-Reader NEON — Teste Serial");
  Serial.println("══════════════════════════════════");
  Serial.println("\n🔌 Montagem esperada:");
  Serial.println("  GPIO 32 → Botao ▲ (PULLUP → GND)");
  Serial.println("  GPIO 33 → Botao ▼ (PULLUP → GND)");
  Serial.println("  GPIO 25 → Botao ● (PULLUP → GND)");
  Serial.println("\n⚠️  Se usar INPUT_PULLUP, NAO precisa");
  Serial.println("   de resistor externo!");
  Serial.println("══════════════════════════════════\n");

  pinMode(BTN_CIMA,  INPUT_PULLUP);
  pinMode(BTN_BAIXO, INPUT_PULLUP);
  pinMode(BTN_SEL,   INPUT_PULLUP);
  pinMode(LED_PIN,   OUTPUT);

  // Pisca LED pra confirmar que ligou
  digitalWrite(LED_PIN, HIGH);
  delay(300);
  digitalWrite(LED_PIN, LOW);

  atualizarTimer();
  renderizar();
}

// ─── LOOP ────────────────────────────────────────────────
void loop() {
  lerBotoes();

  // Verifica deep sleep por inatividade
  if (millis() - lastActivity > IDLE_SLEEP_MS && estado != DORMINDO) {
    Serial.println("\n⏰ Sem atividade por 30s...");
    entrarDeepSleep();
  }

  // Processa ações conforme o estado
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

  // Renderiza se algo mudou
  if (botoes[0].pressed || botoes[1].pressed || botoes[2].pressed || botoes[2].longPressed) {
    renderizar();
  }

  // LED pisca fraco durante deep sleep iminente (feedback visual)
  digitalWrite(LED_PIN, (millis() % 2000 < 100) ? HIGH : LOW);

  delay(20);
}
