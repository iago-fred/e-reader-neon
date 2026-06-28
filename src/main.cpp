/*
 * 📖 E-Reader Neon — Teste de Botões + Navegação Serial
 * 
 * Plataforma: ESP32 Dev Module
 * Framework: Arduino (via PlatformIO)
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

// ─── PINAGEM ─────────────────────────────────────────────
#define BTN_CIMA    32
#define BTN_BAIXO   33
#define BTN_SEL     25
#define LED_PIN     2

// ─── DEBOUNCE ────────────────────────────────────────────
#define DEBOUNCE_MS     50
#define LONG_PRESS_MS   1000
#define IDLE_SLEEP_MS   30000

// ─── ESTADOS ─────────────────────────────────────────────
enum Estado { MENU_LIVROS, LENDO, CONFIG, DORMINDO };
Estado estado = MENU_LIVROS;

// ─── LIVROS FAKE ─────────────────────────────────────────
const char* livros[] = {
  "Dom Quixote", "Memorias Postumas", "1984",
  "O Pequeno Principe", "Neuromancer"
};
const int NUM_LIVROS = 5;
int livroAtual = 0;

// ─── CONFIG FAKE ─────────────────────────────────────────
struct Config {
  int margem = 10, espacamento = 4, tempo_tela = 5, fonte = 22, leds = 50;
} config;

int configItem = 0;
const char* configLabels[] = {
  "Margem (px)", "Espacamento (px)", "Tempo tela (min)",
  "Tamanho fonte", "Intensidade LEDs (%)"
};
int* configValues[] = {
  &config.margem, &config.espacamento, &config.tempo_tela,
  &config.fonte, &config.leds
};

int paginaAtual = 1, totalPaginas = 42;
unsigned long lastActivity = 0;

// ─── BOTÕES ──────────────────────────────────────────────
struct Botao { int pin; int lastState; unsigned long lastDebounce; bool pressed; bool longPressed; };
Botao botoes[3] = {
  { BTN_CIMA,  HIGH, 0, false, false },
  { BTN_BAIXO, HIGH, 0, false, false },
  { BTN_SEL,   HIGH, 0, false, false },
};

void atualizarTimer() { lastActivity = millis(); }

void entrarDeepSleep() {
  Serial.println("\n💤 DEEP SLEEP... Pressione ● para acordar!");
  delay(100);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_25, LOW);
  Serial.flush();
  esp_deep_sleep_start();
}

// ─── LEITURA DOS BOTÕES ─────────────────────────────────
void lerBotoes() {
  unsigned long now = millis();
  static unsigned long pressStart[3] = {0};
  static bool longPressReported[3] = {false};

  for (int i = 0; i < 3; i++) {
    int reading = digitalRead(botoes[i].pin);
    botoes[i].pressed = false;
    botoes[i].longPressed = false;

    if (reading != botoes[i].lastState) botoes[i].lastDebounce = now;

    if ((now - botoes[i].lastDebounce) > DEBOUNCE_MS) {
      if (reading == LOW) {
        if (pressStart[i] == 0) { pressStart[i] = now; longPressReported[i] = false; }
        if (!longPressReported[i] && (now - pressStart[i]) >= LONG_PRESS_MS) {
          botoes[i].longPressed = true; longPressReported[i] = true;
        }
      } else {
        if (pressStart[i] != 0 && !longPressReported[i]) botoes[i].pressed = true;
        pressStart[i] = 0; longPressReported[i] = false;
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
      for (int i = 0; i < NUM_LIVROS; i++)
        Serial.printf(i == livroAtual ? "│  ▶ %-26s│\n" : "│    %-26s│\n", livros[i]);
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
      for (int i = 0; i < 5; i++)
        Serial.printf(i == configItem ? "│  ▶ %s: %-2d            │\n" : "│    %s: %-2d            │\n", configLabels[i], *configValues[i]);
      Serial.println("├────────────────────────────────┤");
      Serial.println("│  ● curto = alterar | longo =  │");
      Serial.println("│  voltar ao menu               │");
      break;
    default: break;
  }
  Serial.println("└────────────────────────────────┘");
}

// ─── HANDLERS ────────────────────────────────────────────
void handleMenu() {
  if (botoes[0].pressed) { livroAtual = (livroAtual - 1 + NUM_LIVROS) % NUM_LIVROS; atualizarTimer(); }
  if (botoes[1].pressed) { livroAtual = (livroAtual + 1) % NUM_LIVROS; atualizarTimer(); }
  if (botoes[2].pressed) { paginaAtual = 1; estado = LENDO; atualizarTimer(); }
  if (botoes[2].longPressed) { configItem = 0; estado = CONFIG; atualizarTimer(); }
}

void handleLendo() {
  if (botoes[0].pressed && paginaAtual > 1) { paginaAtual--; atualizarTimer(); }
  if (botoes[1].pressed && paginaAtual < totalPaginas) { paginaAtual++; atualizarTimer(); }
  if (botoes[2].pressed) { estado = MENU_LIVROS; atualizarTimer(); }
  if (botoes[2].longPressed) { configItem = 0; estado = CONFIG; atualizarTimer(); }
}

void handleConfig() {
  if (botoes[0].pressed) { configItem = (configItem - 1 + 5) % 5; atualizarTimer(); }
  if (botoes[1].pressed) { configItem = (configItem + 1) % 5; atualizarTimer(); }
  if (botoes[2].pressed) {
    (*configValues[configItem]) += 5;
    if (*configValues[configItem] > 255) *configValues[configItem] = 0;
    atualizarTimer();
  }
  if (botoes[2].longPressed) { estado = (paginaAtual > 0) ? LENDO : MENU_LIVROS; atualizarTimer(); }
}

// ─── SETUP ───────────────────────────────────────────────
void setup() {
  Serial.begin(115200); delay(500);
  Serial.println("\n══════════════════════════════════");
  Serial.println("  📖 E-Reader NEON — Teste Serial");
  Serial.println("══════════════════════════════════");
  Serial.println("\n🔌 GPIO 32=▲  33=▼  25=●  (PULLUP)");

  pinMode(BTN_CIMA, INPUT_PULLUP); pinMode(BTN_BAIXO, INPUT_PULLUP);
  pinMode(BTN_SEL, INPUT_PULLUP); pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); delay(300); digitalWrite(LED_PIN, LOW);

  atualizarTimer(); renderizar();
}

// ─── LOOP ────────────────────────────────────────────────
void loop() {
  lerBotoes();
  if (millis() - lastActivity > IDLE_SLEEP_MS && estado != DORMINDO) entrarDeepSleep();

  switch (estado) {
    case MENU_LIVROS: handleMenu(); break;
    case LENDO:       handleLendo(); break;
    case CONFIG:      handleConfig(); break;
    default: break;
  }

  if (botoes[0].pressed || botoes[1].pressed || botoes[2].pressed || botoes[2].longPressed)
    renderizar();

  digitalWrite(LED_PIN, (millis() % 2000 < 100) ? HIGH : LOW);
  delay(20);
}
