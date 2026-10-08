# Project 02 — 4x4 Keypad Scanning (Bare-Metal)

A bare-metal program on STM32F411RE that scans a 4x4 matrix keypad using direct
register access (no HAL/LL library) and prints the pressed key over UART/console.

---

## Project Structure

```
├── Src/
│   ├── main.c                 # main function
│   └── startup.c              # Vector table, stack initialization & Reset Handler
├── STM32F411RE_FLASH.ld       # Linker script defining Flash/RAM memory regions
└── README.md                  # Project documentation
```

---

## Hardware Used

- **NUCLEO-F411RE** (STM32F411RET6) as the main microcontroller
- 4x4 matrix keypad
- Debugger: on-board ST-LINK/V2-1 via Mini-USB cable

**Pin mapping (GPIOC):**

| Keypad line | MCU pin | Direction | Note                  |
|-------------|---------|-----------|------------------------|
| Row 1–4     | PC0–PC3 | Output    | Driven low one at a time during scan |
| Col 1–4     | PC4–PC7 | Input     | Internal pull-up enabled |

---

## Software Used

- IDE: STM32CubeIDE
- Compiler Toolchain: GNU Arm Embedded Toolchain

---

## How It Works

### 1. Identify the required registers

Peripherals used: `RCC` (clock enable) and `GPIOC` (row/column I/O).

| Register       | Address                    |
|----------------|-----------------------------|
| RCC_AHB1ENR    | 0x40023800 + 0x30           |
| GPIOC_MODER    | 0x40020800 + 0x00           |
| GPIOC_PUPDR    | 0x40020800 + 0x0C           |
| GPIOC_IDR      | 0x40020800 + 0x10           |
| GPIOC_ODR      | 0x40020800 + 0x14           |

Each register is accessed through a `volatile` pointer so the compiler never
optimizes away a read/write, and the pointer itself is declared `const`
(e.g. `volatile uint32_t * const reg = ...;`) so it can only ever point to
that one fixed address.

### 2. Configure GPIOC

```c
// 1. Enable GPIOC peripheral clock
*RCC_AHB1ENR |= (1 << 2);

// 2. Configure PC0–PC3 as output (keypad rows)
*GPIOC_MODER &= ~(0xFF);       // clear
*GPIOC_MODER |= 0x55;          // set as output (01)

// 3. Configure PC4–PC7 as input (keypad columns)
*GPIOC_MODER &= ~(0xFF << 8);  // clear to 00 (input)

// 4. Enable internal pull-up on PC4–PC7 (columns)
*GPIOC_PUPDR &= ~(0xFF << 8);  // clear
*GPIOC_PUPDR |= (0x55 << 8);   // set pull-up (01)
```

### 3. Debounce

A key's electrical signal doesn't switch cleanly between HIGH and LOW —
mechanical bounce causes it to flicker for a few milliseconds. A busy-wait
delay is used to skip over that noisy window before trusting the reading:

```c
void delay(void) {
    for (volatile uint32_t i = 0; i < 300000; i++)
        ; // volatile prevents the compiler from optimizing the loop away
}
```

> Note: this is a cycle-count delay, not a calibrated millisecond delay — its
> real duration depends on the CPU clock configuration.

### 4. Scan the matrix

Column pins are pulled HIGH by default (internal pull-up). To detect a key
press, each row is driven LOW one at a time while the others stay HIGH; if a
key in that row is pressed, its column reads LOW.

```c
// Set all rows HIGH
*GPIOC_ODR |= 0x0F;
// Drive row 1 (PC0) LOW
*GPIOC_ODR &= ~(1 << 0);

// Check column 1 (PC4)
if (!(*GPIOC_IDR & (1 << 4))) {
    delay();                              // debounce: press
    if (!(*GPIOC_IDR & (1 << 4))) {       // confirm it's a real press
        printf("1\n");
        while (!(*GPIOC_IDR & (1 << 4)))
            ;                              // wait for release
        delay();                          // debounce: release
    }
}
```

The same check is repeated for every row/column pair (16 combinations total)
to cover all 16 keys.

---

## Build & Flash

<!-- TODO: fill in exact steps, e.g. -->
1. Open the project in STM32CubeIDE.
2. Build (`Project → Build`).
3. Connect the Nucleo board via Mini-USB and flash (`Run → Debug` or `Run`).

## License

<!-- TODO: e.g. MIT License -->

## Author

<!-- TODO: your name / GitHub profile link -->
