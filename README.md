# RTL-Design-LAB2 — SystemC 建模與模擬

在 Linux 環境下使用 SystemC 2.3.1 撰寫並模擬四個練習。內容從基本的時脈驅動行程開始，依序練習 FIFO 通道通訊、模組化的 testbench 架構，最後以 SystemC 定點數（fixed-point）型別建立 RGB 轉 YUV 的模型，並與浮點數及整數版本比較數值誤差。

| 練習 | 主題 | 重點概念 |
|---|---|---|
| [Exercise 1](#exercise-1shift-register) | 4 級 Shift Register | `SC_CTHREAD`、時脈正緣觸發 |
| [Exercise 2](#exercise-2producer--consumer) | Producer / Consumer | `sc_fifo`、不同速率的模組間通訊 |
| [Exercise 3](#exercise-3systemc-testbench) | SystemC Testbench | Stimulus / DUT / Monitor 分離、`sc_port` 介面 |
| [Exercise 4](#exercise-4rgb-轉-yuv) | RGB 轉 YUV | `sc_fixed` 定點數建模、多種公式的誤差分析 |

## 目錄結構

```
RTL-Design-LAB2/
├── exercise1/
│   ├── main.cpp              # 4 級 shift register
│   └── Makefile
├── exercise2/
│   ├── exercise2.cpp         # Producer / Consumer + sc_fifo
│   └── Makefile
├── exercise3/
│   ├── main.cpp              # sc_main：模組連線
│   ├── stimgen.h / .cpp      # Stimulus Generator
│   ├── adder.h / .cpp        # 待測模組（DUT）
│   ├── monitor.h / .cpp      # 輸出監看
│   └── Makefile
└── exercise4/
    ├── bmp_utils.h                   # 24-bit BMP 讀寫工具
    ├── version1_formula1.cpp         # 公式 1，浮點數
    ├── version2_formula2.cpp         # 公式 2（BT.601 limited range），浮點數
    ├── version3_formula22.cpp        # 公式 2 的整數近似版
    ├── version4_systemc_fixed.cpp    # 公式 1，SystemC 定點數模型
    ├── compare_y.cpp                 # Y 通道逐像素誤差比較
    ├── mountain256.bmp               # 測試影像（256×256）
    └── Makefile
```

## 建置與執行

開發環境為 Linux + SystemC 2.3.1，Makefile 預設 SystemC 安裝在 `/usr/local/systemc-2.3.1a`。若安裝路徑不同，請修改各 Makefile 開頭的路徑。

```bash
cd exercise2
make
./exercise2
```

Exercise 4 一次編譯所有版本，再依序產生輸出影像並比較誤差：

```bash
cd exercise4
make
./version1_formula1      mountain256.bmp v1
./version2_formula2      mountain256.bmp v2
./version3_formula22     mountain256.bmp v3
./version4_systemc_fixed mountain256.bmp v4
./compare_y v1_Y.bmp v2_Y.bmp v3_Y.bmp v4_Y.bmp
```

每個版本會輸出 `<prefix>_Y.bmp`、`<prefix>_U.bmp`、`<prefix>_V.bmp` 三張灰階影像。

---

## Exercise 1：Shift Register

以 `SC_CTHREAD` 建立同步行程，敏感於時脈正緣（週期 20 ns）。每個時脈週期產生一筆 0–99 的隨機資料推入 Q1，原有資料依序往 Q2、Q3、Q4 移動，行為等同硬體中的 4 級移位暫存器。

```
At time 20 ns  Q1: 0  Q2: 0  Q3: 0  Q4: 0
At time 40 ns  Q1: 83 Q2: 0  Q3: 0  Q4: 0
At time 60 ns  Q1: 86 Q2: 83 Q3: 0  Q4: 0
At time 80 ns  Q1: 77 Q2: 86 Q3: 83 Q4: 0
At time 100 ns Q1: 15 Q2: 77 Q3: 86 Q4: 83
```

可以看到同一筆資料（例如 83）每經過一個時脈就往後移一級，第 4 個時脈後抵達 Q4。

## Exercise 2：Producer / Consumer

兩個 `SC_CTHREAD` 模組透過深度 8 的 `sc_fifo<int>` 交換資料，時脈週期為 5 ns：

| 模組 | 行為 | 週期 |
|---|---|---|
| Producer | `wait(2)` 後產生一筆隨機資料寫入 FIFO | 每 10 ns |
| Consumer | `wait(3)` 後從 FIFO 讀出一筆資料 | 每 15 ns |

```
At time 10 ns produces data 83
At time 15 ns consumes data 83
At time 20 ns produces data 86
At time 30 ns produces data 77
At time 30 ns consumes data 86
...
```

由於生產速率（每 10 ns）高於消費速率（每 15 ns），資料會逐漸累積在 FIFO 中。`sc_fifo` 的 `write()` 在 FIFO 滿時會阻塞，因此模擬時間足夠長時，Producer 最終會被 Consumer 的速率限制住，這就是 FIFO 提供的 back-pressure 機制。

## Exercise 3：SystemC Testbench

將測試流程拆成三個獨立模組，以 `sc_fifo` 連接，模組之間只透過 `sc_port<sc_fifo_in_if<int>>` / `sc_port<sc_fifo_out_if<int>>` 介面溝通：

```
┌──────────┐  sig_a  ┌───────┐  sig_c  ┌─────────┐
│ stimgen  │────────▶│ Adder │────────▶│ monitor │
│          │────────▶│ (DUT) │         │         │
└──────────┘  sig_b  └───────┘         └─────────┘
```

- **stimgen：** 以 seed = 10 起始，每次送出 `seed + 1` 與 `seed + 6` 兩筆資料，再更新 `seed = (seed + 19) % 123`，共產生 20 組，結束後呼叫 `sc_stop()`。
- **Adder：** 從兩個輸入 FIFO 各讀一筆資料相加後送出，其中第偶數筆結果會額外加 2（例如第 2 組輸入為 30 與 35，輸出為 30 + 35 + 2 = 67）。
- **monitor：** 從輸出 FIFO 讀取結果並印出。

輸出序列：

```
27 67 103 143 179 219 9 49 85 125 161 201 237 31 67 107 143 183 219 13
```

這種 stimulus / DUT / monitor 分離的架構，讓同一個待測模組可以搭配不同的測試資料重複驗證，與實際硬體驗證流程的概念相同。

## Exercise 4：RGB 轉 YUV

以四種方式實作 RGB 轉 YUV，並以 Version 1 為基準，用 `compare_y` 逐像素比較 Y 通道的誤差。

| 版本 | 公式 | 數值表示 |
|---|---|---|
| Version 1 | 公式 1（full range） | `double` 浮點數 |
| Version 2 | 公式 2（BT.601 limited range） | `double` 浮點數 |
| Version 3 | 公式 2 | 整數係數 + 右移 8 位元 |
| Version 4 | 公式 1 | SystemC `sc_fixed` 定點數 |

**公式 1（full range，Y 範圍 0–255）**

```
Y =  0.299·R + 0.587·G + 0.114·B
U = -0.169·R - 0.331·G + 0.500·B + 128
V =  0.500·R - 0.419·G - 0.081·B + 128
```

**公式 2（BT.601 limited range，Y 範圍 16–235）**

```
Y =  0.2568·R + 0.5041·G + 0.0979·B + 16
U = -0.1482·R - 0.2910·G + 0.4392·B + 128
V =  0.4392·R - 0.3678·G - 0.0714·B + 128
```

**Version 3：公式 2 的整數近似**

```
Y = ((  66·R + 129·G +  25·B + 128) >> 8) + 16
U = (( -38·R -  74·G + 112·B + 128) >> 8) + 128
V = (( 112·R -  94·G -  18·B + 128) >> 8) + 128
```

係數乘以 256 取整數，加 128 後右移 8 位元達到四捨五入的效果，只需要整數乘加與移位，適合直接對應到硬體。

**Version 4：SystemC 定點數模型**

`RGB2YUV_FX` 模組以 `SC_METHOD` 實作，對 R、G、B 三個 `sc_signal<sc_uint<8>>` 輸入敏感。輸入轉為 `sc_ufixed<16, 8>`，運算結果存為 `sc_fixed<32, 12, SC_RND, SC_SAT>`（12 位元整數、20 位元小數，四捨五入並飽和），最後轉回 8-bit 輸出。Testbench 每寫入一個像素就執行 `sc_start(1, SC_NS)`，讓模組完成一次 evaluate 後再讀回結果。

### 誤差比較結果（Y 通道，相對於 Version 1）

| 比較 | 最大絕對誤差 | 平均絕對誤差 |
|---|---|---|
| Version 1 vs. Version 2 | 18 | 3.58131 |
| Version 1 vs. Version 3 | 18 | 3.55475 |
| Version 1 vs. Version 4 | 1 | 0.000167847 |

### 分析

- **Version 2、3 與 Version 1 的差距主要來自數值範圍不同，而不是精度。** 公式 1 將 Y 對應到 0–255，公式 2 則對應到 BT.601 的 16–235。在這張測試影像上，Version 1 的 Y 介於 0–244，Version 2 介於 16–226，暗部被抬高、亮部被壓低，因此在最亮與最暗的區域誤差最大，最大值達 18。
- **整數近似幾乎不損失精度。** Version 2 與 Version 3 使用相同公式，兩者之間的最大誤差只有 1、平均約 0.087，表示「係數 × 256 再右移」的整數化方式已經足夠精確。
- **定點數模型與浮點數幾乎一致。** Version 4 與 Version 1 使用相同公式，65,536 個像素中只有約 11 個像素差 1，其餘完全相同。20 位元的小數精度遠高於 8-bit 輸出所需，差異只發生在結果剛好落在四捨五入邊界的像素上。這驗證了在進入 RTL 實作之前，先用 `sc_fixed` 決定位元寬度、確認精度是否足夠的 ESL 設計流程。

## 使用工具

SystemC 2.3.1 · C++11 · GCC · GNU Make · Linux
