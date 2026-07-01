# 📖 Projeto E-Reader Neon

> **Criado:** 16/jun/2026
> **Status:** 🟡 Em desenvolvimento (Onda 1 concluída)
> **Dono:** Iago Frederick

---

## 🎯 Visão Geral

E-reader caseiro usando ESP32 + tela e-ink WeAct Studio 3.7", bateria KG40 Motorola 3.7V 4000mAh, carcaça impressa em 3D, acrílico com frontlight lateral.

---

## 📦 Componentes

| Componente | Especificação | Preço | Status |
|------------|--------------|-------|--------|
| ESP32 | WROOM-32 (30 pinos) | ~R$ 35 | ✅ OK |
| Protoboard + jumpers | Para testes | ~R$ 15 | ✅ OK |
| Botões táctil | 3x 6x6mm | ~R$ 3 | ✅ OK |
| Tela E-Ink | WeAct Studio 3.7" (1005009712159337) | ~R$ 40-120 | Pendente |
| Bateria | KG40 Motorola 3.7V 4000mAh | R$ 0 | ✅ OK |
| TP4056 | Carregador + proteção 1S | ~R$ 8 | Pendente |
| LDO XC6206P332 | Baixo dropout ~100mV | ~R$ 5 | Pendente |
| SD Card Module | SPI | ~R$ 10 | Pendente |
| LEDs + MOSFET | 4x branco 0603 + 2N7002 | ~R$ 10 | Pendente |
| Resistores | 10k + 47R + 1k | R$ 0 | ✅ OK |
| Capacitores | 4x 0,47µF (a1p3) + 2x 68nF (zx683k) + 1x 22µF (CD11-G) | R$ 0 | ✅ OK |
| Diodo 1N5819 | Proteção reversa | ~R$ 1 | Pendente |
| Acrílico 3mm | ~30x20cm | ~R$ 15 | Pendente |
| Filamento 3D | PLA/PETG | ~R$ 10 | Pendente |
| **Total** | | **~R$ 135-215** | |

---

## ⚡ Pinagem ESP32

| Função | GPIO | Notas |
|--------|------|-------|
| Tela SCK | 18 | SPI Clock |
| Tela MOSI | 23 | SPI Data |
| Tela CS | 5 | Chip Select |
| Tela DC | 17 | Data/Command |
| Tela RST | 16 | Reset |
| Tela BUSY | 4 | Status |
| SD CS | 15 | Chip Select SD |
| SD MOSI | 13 | |
| SD MISO | 12 | |
| SD SCK | 14 | |
| Botão ▲ (Próximo) | 32 | Pull-up 10k |
| Botão ▼ (Anterior) | 33 | Pull-up 10k |
| Botão ● (Selecionar) | 25 | Pull-up 10k |
| Frontlight PWM | 26 | MOSFET gate |
| Bateria ADC | 34 | Divisor resistivo |

> GPIOs 6,7,8,9,10,11 = NÃO USAR

---

## 🖥️ Arquitetura

### Fluxo do Usuário

```
BOOT
  │
  ├── /firmware/*.bin existe?
  │     ├── SIM → atualiza flash + reinicia
  │     └── NÃO → continua
  │
  ▼
MENU PRINCIPAL (lista de livros da pasta /books/)
  │
  │  ▲/▼ = navegar na lista
  │  ● curto = abrir livro
  │  ● longo (>1s) = abrir CONFIG
  │  ▲ + ● (3s) = ESP.restart()
  │
  ├──▶ LIVRO ABERTO
  │      ▲/▼ = virar página
  │      ● curto = voltar ao MENU
  │      ● longo = abrir CONFIG
  │      ▲ + ● (3s) = ESP.restart()
  │      sem input por N min → DEEP SLEEP
  │
  └──▶ CONFIGURAÇÕES
         ▲/▼ = navegar opções
         ● curto = alterar valor
         ● longo = voltar (menu anterior)
         ├── Margem da tela (px)
         ├── Espaçamento linhas (px)
         ├── Tempo de tela (min)
         ├── Tamanho da fonte
         └── Intensidade LEDs (0-100%)
         sem input por N min → DEEP SLEEP

═══ DEEP SLEEP ═══
qualquer botão acorda → MENU PRINCIPAL
```

### Lógica

```
loop():
  estado = MENU_LIVROS

  se estado == MENU_LIVROS:
    listar .neon da pasta /books/
    ▲/▼ = navegar lista
    ● curto = abrir livro
    ● longo = config
    sem input N min → deep_sleep()

  se estado == LENDO:
    mostrar página do capítulo atual
    ▲ = próxima página
    ▼ = página anterior
    ● curto = menu
    ● longo = config
    sem input N min → salva bookmark (dentro do .neon) + deep_sleep()

  se estado == CONFIG:
    navegar opções
    ▲/▼ = muda opção
    ● curto = altera valor
    ● longo = volta
```

---

## 💾 O que salvar e onde

### 📂 SD Card

```
/books/
├── dom_quixote.neon     ← livro + metadados + bookmark
├── memorias.neon        ← livro + metadados + bookmark
└── ...

/config.json             ← preferências do usuário

/firmware/
└── neon_v1.1.bin        ← firmware novo pra atualizar (opcional)
```

### 📄 `/books/*.neon` — O livro completo

O bookmark fica **dentro do próprio arquivo .neon**:

```
══════════ CABEÇALHO ══════════
[4 bytes]  Magic: "NEON"
[1 byte]   Versão
[1 byte]   Flags
[4 bytes]  BOOKMARK ← byte offset da última posição lida
[2 bytes]  Tamanho título
[N bytes]  Título UTF-8
[2 bytes]  Tamanho autor
[M bytes]  Autor UTF-8
[4 bytes]  Qtd capítulos
[...]
```

**Como grava o bookmark:**
- Abre o .neon em modo r+w
- Pula pro offset do campo BOOKMARK (fixo, posição 6)
- Sobrescreve 4 bytes com o novo byte offset
- Fecha — sem ler ou reescrever o arquivo todo

**Vantagem:** se copiar o livro pra outro SD, o marcador vai junto.

### 📄 `/config.json` — Configurações

```json
{
  "margem": 10,
  "espacamento": 4,
  "tempo_tela": 5,
  "fonte": 22,
  "leds": 50
}
```

- **Lê** no BOOT
- **Grava** quando sai do menu Config
- **Se não existir no SD, o ESP32 cria com valores padrão:**

```
No setup():
  se SD.exists("/config.json") == false:
    criar /config.json com:
      margem = 10, espaçamento = 4, tempo_tela = 5
      fonte = 22, leds = 50
```

Isso garante que:
- ✅ Primeiro uso funciona sem depender de nada
- ✅ Usuário pode deletar o config.json manualmente pra resetar (tipo fábrica)
- ✅ Nunca trava por falta de arquivo

### ⚡ Flash do ESP32 (NVS) — Cache de boot

| Chave | Valor | Quando | Pra quê |
|-------|-------|--------|--------|
| `last_book` | "dom_quixote.neon" | Ao abrir livro | Destacar no menu ao acordar |
| `has_sd` | true/false | BOOT | Saber se pode tentar acessar SD |

### 🔄 Fluxo de leitura e escrita

```
BOOT:
  NVS → has_sd?
    ├── Não → mostra "Sem SD"
    └── Sim → SD → config.json existe?
                  ├── Não → cria config.json com defaults
                  └── Sim → carrega configurações
               NVS → last_book
               SD → lista /books/*.neon
               → MENU

ABRIU LIVRO:
  SD → .neon → lê bookmark do cabeçalho
  NVS → grava last_book

VIROU PÁGINA:
  Timer de inatividade RESET

VOLTOU AO MENU:
  SD → .neon → overwrite 4 bytes do bookmark

DEEP SLEEP:
  SD → .neon → bookmark ← byte atual
  → dormir

ACORDOU:
  NVS → last_book
  SD → config.json
  → MENU
```

### 🔐 Falhas e recuperação

| Problema | Consequência | Recuperação |
|----------|-------------|-------------|
| SD removido | Não carrega nada | Mostra "Sem SD" na tela |
| config.json corrompido | Ignora arquivo | Cria novo com defaults |
| Bookmark no .neon corrompido | Perde página | Começa do byte 0 (início do livro) |
| NVS corrompido (raro) | Perde last_book | Destaca primeiro livro |
| Tudo corrompido | Começa do zero | Ainda lê lista de .neon do SD |

---

## 📦 Bibliotecas

| Biblioteca | Pra quê |
|-----------|---------|
| GxEPD2 | Controlar a tela e-ink |
| SD + FS | Ler cartão SD |
| ArduinoJson | Ler/gravar config.json |
| Preferences | Salvar cache de boot na NVS |
| Update.h | Atualizar firmware via SD (OTA offline) |

---

## 🔤 Fonte

**Escolhida: New Century Schoolbook (ncen) — u8g2**

Serifada projetada para leitura contínua. Ideal para e-ink porque:
- Serifa guia o olho na linha seguinte
- Caracteres abertos e bem diferenciados
- Legibilidade superior em texto longo comparada a sans-serif

### 3 tamanhos disponíveis

| Config | Fonte | Aparência |
|--------|-------|-----------|
| Pequena | `u8g2_font_ncenR14_tf` | ~20 linhas/página, letra miúda |
| **Média** | **`u8g2_font_ncenR18_tf`** | **~16 linhas, padrão confortável** |
| Grande | `u8g2_font_ncenR24_tf` | ~12 linhas, letrão |

### Interface (menus)

`u8g2_font_helvR10_tf` (Helvetica) — limpa, ocupa pouco espaço.

### Acentos

Todas as fontes com sufixo `_tf` incluem Latin Extended A (á, à, â, ã, é, ê, í, ó, ô, õ, ú, ç, etc).

### Consumo de flash
~15-20KB para as 3 variações de tamanho.

---

## 🧊 Carcaça 3D

```
┌─────────────────────────────────┐
│ 1. Tampa traseira              │ ← bateria + ESP32 + TP4056
├─────────────────────────────────┤
│ 2. Placa ESP32 + SD + BMS      │
├─────────────────────────────────┤
│ 3. Espaçador                    │
├─────────────────────────────────┤
│ 4. Tela E-Ink (voltada p/cima) │
├─────────────────────────────────┤
│ 5. Acrílico 3mm (guia de luz)  │ ← LEDs laterais
├─────────────────────────────────┤
│ 6. Moldura frontal (borda)     │ ← Botões nas laterais
└─────────────────────────────────┘
```

---

## 🛒 Planos de Compra

### 🥇 Onda 1 — Concluído ✅

| Item | Preço | Status |
|------|-------|--------|
| ESP32 WROOM-32 | ~R$ 35 | ✅ OK |
| Protoboard + jumpers | ~R$ 15 | ✅ OK |
| Botões | ~R$ 3 | ✅ OK |

### 🥈 Onda 2 — Junto do AliExpress (~R$ 50-130)

| Item | Preço |
|------|-------|
| 🔵 Tela WeAct 3.7" | ~R$ 40-120 |
| Módulo SD Card SPI | ~R$ 10 |
| Botões (lote) | ~R$ 3 |

### 🥉 Onda 3 — Depois (~R$ 20-30)

| Item | Preço |
|------|-------|
| TP4056 | ~R$ 8 |
| LDO XC6206P332 | ~R$ 5 |
| Resistores (10k + 47R + 1k) | R$ 0 | ✅ OK |
| Diodo 1N5819 | ~R$ 1 |
| LEDs + MOSFET 2N7002 | ~R$ 10 |
| Acrílico | ~R$ 15 |
| Filamento 3D | ~R$ 10 |

---

## 🖥️ Programa Conversor Desktop

### Função

Converte .txt cru → formato .neon otimizado pro ESP32.

```
Entrada: .txt (UTF-8)
         ├── Detecta capítulos automaticamente
         ├── Normaliza texto
         ├── Extrai título/autor (ou pergunta)
         └── Cover opcional (PNG → 1-bit)

Saída: .neon
         ├── Cabeçalho com metadados + bookmark
         ├── Cover (opcional)
         ├── Índice de capítulos (byte offset)
         └── Texto UTF-8 puro
```

### Detecção de Capítulos

1. **Padrões comuns:** "Capítulo", "CHAPTER", "Parte I", "1." no início de linha, "***" como separador, numerais romanos isolados
2. **Se nada encontrar:** Divide a cada ~3000-4000 palavras, quebra no próximo parágrafo
3. **Usuário revisa:** aceita, ajusta pontos de quebra, ou customiza

### Implementação

Sugestão: Python com PIL (pra covers), interface Tkinter ou CLI.

---

## 💡 Anotações

- Frontlight: acrílico polido nas bordas + LEDs SMD laterais (efeito Paperwhite)
- Pintar borda do acrílico de branco refletivo
- XC6206P332 escolhido por dropout de ~100mV (ideal pra 1S Li-ion)
- TP4056 já protege contra over-discharge, over-charge e curto
- Bateria 4000mAh → ~50h sem frontlight, ~28h com frontlight
- Bookmark fica dentro do .neon — 4 bytes, overwrite no cabeçalho
- Se config.json não existir, ESP32 cria com defaults no BOOT

---

## 📁 Estrutura do Código

```
src/
├── main.cpp          ← setup() + loop() — coordena os módulos
├── botoes.h/.cpp     ← Leitura dos 3 botões (GPIO 32, 33, 25) com debounce + long-press
├── config.h/.cpp     ← Struct Config, valores padrão, tabela de itens editáveis
├── deep_sleep.h/.cpp ← Timer de inatividade, entrada em deep sleep, wake por botão
├── navegacao.h/.cpp  ← Máquina de estados (MENU_LIVROS / LENDO / CONFIG / DORMINDO) e handlers
└── tela.h/.cpp       ← Renderização da interface (Serial; futuramente e-ink)
```

### Dependências entre módulos

```
main.cpp
  ├── botoes.h      (lerBotoes, initBotoes)
  ├── navegacao.h   (Estado, handleMenu/...)
  ├── tela.h        (renderizar)
  └── deep_sleep.h  (verificarSleep, atualizarTimer)

navegacao.cpp
  ├── botoes.h      (botoes[].pressed / longPressed)
  ├── config.h      (configValues, configItem)
  └── deep_sleep.h  (atualizarTimer)

tela.cpp
  ├── navegacao.h   (Estado, livros, NUM_LIVROS, livroAtual, paginaAtual, totalPaginas)
  └── config.h      (configLabels, configValues, configItem)

deep_sleep.cpp
  └── navegacao.h   (estado para verificar DORMINDO)

botoes.cpp    → (autônomo — só depende de Arduino.h)
config.cpp    → (autônomo — só depende de Arduino.h)
```

### Ciclo do loop()

```
1. lerBotoes()          — atualiza flags pressed/longPressed
2. verificarSleep()     — se inativo por IDLE_SLEEP_MS → deep sleep
3. switch(estado)       — handleMenu / handleLendo / handleConfig
4. se houve input → renderizar()
5. LED heartbeat + delay(20)
```

---

*Última atualização: 28/jun/2026*
