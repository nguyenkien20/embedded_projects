# PROJECT_01 blinking LED using bare-metal 
- In this project, I will demonstrate how to set up a bare-metal C project from scratch for the STM32F411RE microcontroller. 
- This includes finding register memory addresses in the Reference Manual, writing custom startup code and linker scripts, and understanding how embedded hardware operates under the hood without relying on HAL/SPL libraries
****
## Project purpose
- The project is the first challenge on my way to becoming an **Embedded Software Engineer**. 
- It serves as a hands-on learning milestone to master low-level register manipulation, build systems, and memory management, as well as a reference to look back on what I have gone through.
****
 
## Features
- 100% Bare-metal, no using HAL Library, direct peripheral register manipulation.
- Using STM32F411xC/E Reference Manual (RM0383) & Datasheet.
- Custom Startup Code: Interrupt Vector Table setup and `Reset_Handler` implementation in C
- Hardware Control: Toggling on-board User LED (LD2 - pin `PA5`) using RCC and GPIO registers.
## Project Structure
- the project has structure like this: 
```

├── Inc/
│   └── stm32f411re.h          # Memory map & register definitions (RCC, GPIO, etc.)
├── Src/
│   ├── main.c                 # Application logic (LED toggle loop & delays)
│   └── startup.c              # Vector table, stack initialization & Reset Handler
├── STM32F411RE_FLASH.ld       # Linker script defining Flash/RAM memory regions
└── README.md                  # Project documentation
```
****
## Hardware using
- in this project, i using ***NUCLEO-F411RE (STM32F411RET6)*** like a main microcontroller with target component is on-board User LED (LD2 connected to pin **PA5**)
- Debugger: On-board ST-LINK/V2-1 via Mini-USB cable
<img width="1525" height="1438" alt="anh2" src="https://github.com/user-attachments/assets/2037b971-55a4-498e-b9f7-07fb29b49d30" />

****

## Software using
- IDE: STM32CubeIDE
- Compiler Toolchain: Integrated GNU Arm Embedded Toolchain.
****

## Hand-on Practice
1. Identifying the Required Register Addresses
firstly, we have to identify the required register address, it include: RCC, GPIOA_MODER and GPIOA_ODR
a. RCC (Reset and Clock Control) – Enable the clock for GPIOA
- let take a look at RCC base address: `0x40023800` and RCC_AHB1ENR offset: `0x30`
- After that we take RCC base address plus offset to obtain actual address (memory-mapped I/O): 
`0x40023800 + 0x30 = 0x40023830`
- Stored in the pClock pointer variable.
b. GPIOA_MODER – Configure the GPIO pin mode
- GPIOA base address: `0x40020000`
- PIOx_MODER offset: `0x00`
→ actual address (memory-mapped I/O): `0x40020000`
- Stored in the pMode pointer variable
c. GPIOA_ODR (Output Data Register) – Control the output logic level
GPIOA base address: `0x40020000`
GPIOx_ODR offset: `0x14`
→ actual address (memory-mapped I/O): `0x40020014`
Stored in the pOutput_data pointer variable
2. Hardware Configuration
Enable the GPIOA clock: Set bit 0 (GPIOAEN) of RCC_AHB1ENR to 1.
```RCC_AHB1ENR |= (1<<0);```
- Without enabling the clock, subsequent read/write operations to the GPIOA registers will have no effect because the peripheral has not been supplied with a clock signal.
- Configure PA5 as an output: Each GPIO pin occupies 2 bits in the MODER register. For GPIO pin n, the corresponding bits are 2n and 2n+1. Therefore, PA5 uses bits 10 and 11.
- Clear bits 10–11 to 00 (reset state) before configuring the new mode.
```
GPIOA_MODER &= ~(1<<10);
GPIOA_MODER &= ~(1<<11);
```
- This prevents the new configuration from being unintentionally combined with the previous register value.
- Afterward, we set bit 10 to 1 and bit 11 to 0, resulting in 01 = General-purpose output mode.
```
GPIOA_MODER |= (1<<10);
```
3. LED Blink Loop
- and finally is the loop we using to turn on and off LED:
```
while(1){
GPIOA_ODR |= (1<<5); // turn on led
for(uint32_t i = 0; i < 600000; i++);
GPIOA_ODR &= ~(1<<5); // turn off led
for(uint32_t i = 0; i < 600000; i++);
	}
```
****

## Build project
**1. Import Project into STM32CubeIDE**
- Open STM32CubeIDE.
- Go to File -> Import... -> General -> Existing Projects into Workspace.
- Select the root folder of this project and click Finish.
**2. Build Project**
- Press Ctrl + B (or Cmd + B on Mac) or click the *Hammer icon* on the toolbar to compile the project.
- Verify that the compilation finishes with 0 errors, 0 warnings.
**3. Flash & Debug**
- Connect your NUCLEO-F411RE board to your PC using a Mini-USB cable.
Click the Run icon (▶) or Debug icon (ST-LINK GDB Server).
- STM32CubeIDE will automatically build, flash the firmware into Flash memory, and start execution.
****
## Usage
- Once flashed successfully, the User LED (LD2 on pin PA5) will start blinking at the programmed delay interval.
Insight:
`
Looking closely at register addresses in the reference manual helps reveal how hardware interacts directly with software—an essential understanding that higher-level libraries often conceal.
`
****

## Conclusion and Knowledge Gained
- **Limitation:** The empty `for` loop delay is inaccurate (compiler-optimization dependent, may even get optimized away) and blocks the CPU from doing anything else while it runs. 
- **Next step:** switch to SysTick/hardware timer for precise ms-level delays, then move to an interrupt-driven, non-blocking design — a natural bridge into FreeRTOS.
**Skills gained:**
- **Register-level programming** — used RM0383 to compute base addresses/offsets and configure RCC & GPIO bit fields directly.
- **Boot process & startup code** — wrote a custom Vector Table and `Reset_Handler`, including `.data` initialization and `.bss` clearing.
- **Linker scripts (`.ld`)** — mapped FLASH vs. SRAM, handled LMA/VMA, set stack alignment.
- **Bitwise operations** — set/clear/toggle/read bits for hardware control.
- **Build system** — configured STM32CubeIDE for bare-metal builds without CubeMX/HAL.
Though simple, this project builds the register-level foundation needed before moving on to HAL or RTOS.
****
