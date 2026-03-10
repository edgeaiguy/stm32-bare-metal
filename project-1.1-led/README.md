# STM32F4 Bare-Metal LED Blink

Bare-metal LED control on STM32F407 Discovery using direct register manipulation — no HAL, no CMSIS, no libraries.

## Overview

This project drives the four onboard LEDs (LD3–LD6) on the STM32F407G-DISC1 Discovery board using nothing but direct memory-mapped register writes. Every peripheral — clock enable, GPIO configuration, and output control — is configured by writing to specific memory addresses derived from the STM32F4 reference manual (RM0090).

The LEDs cycle sequentially: one LED on at a time, rotating through green → orange → red → blue.

## Hardware

- **Board:** STM32F407G-DISC1 Discovery Board
- **MCU:** STM32F407VGT6 (ARM Cortex-M4, 168 MHz)
- **LEDs:**
  - LD4 (Green) — PD12
  - LD3 (Orange) — PD13
  - LD5 (Red) — PD14
  - LD6 (Blue) — PD15

## Register-Level Approach

No vendor HAL or CMSIS convenience headers are used. All register addresses and bit positions are derived directly from the reference manual:

### 1. Clock Enable (RCC_AHB1ENR)
- Base: `0x40023800` + Offset: `0x30`
- Set bit 3 (GPIODEN) to enable the GPIOD peripheral clock
- Readback after write to ensure the clock is stable before accessing GPIO registers

### 2. GPIO Mode Configuration (GPIOD_MODER)
- Base: `0x40020C00` + Offset: `0x00`
- Each pin uses 2 bits: `01` = General purpose output mode
- Pins 12–15 configured (bits 24–31)

### 3. Output Control (GPIOD_BSRR)
- Base: `0x40020C00` + Offset: `0x18`
- Write-only register: lower 16 bits set pins, upper 16 bits reset pins
- Writing `0` to a bit has no effect — no read-modify-write needed

### Key Registers Referenced

| Register | Address | Purpose |
|---|---|---|
| RCC_AHB1ENR | `0x40023830` | Peripheral clock enable |
| GPIOD_MODER | `0x40020C00` | Pin direction (input/output/alt/analog) |
| GPIOD_ODR | `0x40020C14` | Output data (current pin states) |
| GPIOD_BSRR | `0x40020C18` | Atomic bit set/reset |

## Project Structure

```
├── Src/
│   ├── main.c          # Application code — register definitions and LED control
│   ├── syscalls.c       # Newlib system call stubs
│   └── sysmem.c         # Newlib memory management stubs
├── Startup/
│   └── startup_stm32f407vgtx.s   # Vector table and Reset_Handler
├── STM32F407VGTX_FLASH.ld        # Linker script (Flash memory layout)
├── STM32F407VGTX_RAM.ld          # Linker script (RAM — for debug)
├── Makefile                       # Build and flash commands
└── .vscode/
    └── launch.json                # Cortex-Debug configuration
```

## Prerequisites

- `arm-none-eabi-gcc` — ARM cross-compiler toolchain
- `openocd` — On-chip debugger (communicates with ST-LINK)
- `gdb-multiarch` — Debugger with ARM support (for step debugging via VSCode)
- STM32F407G-DISC1 board connected via USB

### Install on Ubuntu

```bash
sudo apt install gcc-arm-none-eabi openocd gdb-multiarch
```

### USB Permissions (Linux)

If you get `LIBUSB_ERROR_ACCESS` when flashing, add a udev rule for the ST-LINK:

```bash
sudo tee /etc/udev/rules.d/99-stlink.rules << 'EOF'
ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374b", MODE="0666", TAG+="uaccess"
EOF
sudo udevadm control --reload-rules && sudo udevadm trigger
```

Unplug and replug the board after adding the rule.

## Build & Flash

```bash
make            # Compile — outputs build/led-blink.elf and build/led-blink.bin
make flash      # Flash to board via OpenOCD and ST-LINK
make clean      # Remove build artifacts
```

## Debug (VSCode + Cortex-Debug)

1. Install the **Cortex-Debug** extension in VSCode
2. Open this project folder in VSCode
3. Press F5 to launch a debug session (uses the included `launch.json`)

The debug chain: **VSCode → gdb-multiarch → OpenOCD → ST-LINK → Cortex-M4**

## Reference Documents

- [RM0090](https://www.st.com/resource/en/reference_manual/dm00031020.pdf) — STM32F405/407 Reference Manual (registers, peripherals, memory map)
- [DS8626](https://www.st.com/resource/en/datasheet/stm32f407vg.pdf) — STM32F407 Datasheet (pinout, electrical characteristics)
- [UM1472](https://www.st.com/resource/en/user_manual/dm00039084.pdf) — STM32F4 Discovery Board User Manual (board layout, LED/button assignments)

## What I Learned

- How ARM Cortex-M4 peripherals are accessed through memory-mapped I/O
- The three essential documents for STM32 development: reference manual (chip), datasheet (package), user manual (board)
- Bitwise operations for register manipulation: OR to set, AND with inverted mask to clear, XOR to toggle
- Why `volatile` is critical for hardware registers: prevents compiler optimization of memory-mapped I/O
- The difference between read-write registers (use `|=`) and write-only registers like BSRR (use `=`)
- How the startup file and linker script work together at build time to place code and data in the correct memory regions
- The overall embedded process: generate a bare-bones project in STM32CubeIDE, peruse the essential documents for related peripherals and clock enablers, setting up the code and flashing to the chip
