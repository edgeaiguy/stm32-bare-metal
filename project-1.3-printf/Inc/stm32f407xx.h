#define RCC_BASE  0x40023800UL // RCC base address (from the memory map chapter). Note: end all hex address defines with UL (unsigned long)
#define RCC_AHB1ENR  (*(volatile unsigned int *)(RCC_BASE + 0x30)) // AHB1ENR is at offset 0x30. GPIOA/D lives here
#define RCC_APB1ENR (*(volatile unsigned int *)(RCC_BASE + 0x40)) // APB1ENR is at offset 0x40. UART2 lives here.
#define GPIOA_BASE 0x40020000UL // GPIOA base address. PA2 and PA3 (for UART comms) live here.
#define GPIOD_BASE  0x40020C00UL // GPIOD base address (from memory map). LEDs live here. UL = unsigned long, compiler treats it as 32-bit value rather than signed integer
#define USART2_BASE 0x40004400UL // USART2 base address (from memory map)
#define GPIOA_MODER (*(volatile unsigned int *)(GPIOA_BASE + 0x00)) //GPIOA_MODER is the first register in GPIOA
#define GPIOA_AFRL (*(volatile unsigned int *)(GPIOA_BASE + 0x20)) // also define the alternate function low register offset
#define GPIOD_MODER (*(volatile unsigned int *)(GPIOD_BASE + 0x00))// GPIOD_MODER is the first register in GPIOD
#define USART2_BRR (*(volatile unsigned int *)(USART2_BASE + 0x08)) // BRR (baud rate register)
#define USART2_CR1 (*(volatile unsigned int *)(USART2_BASE + 0x0C)) // CR1 (control register 1)
#define USART2_SR (*(volatile unsigned int *)(USART2_BASE + 0x00)) // SR (status register)
#define USART2_DR (*(volatile unsigned int *)(USART2_BASE + 0x04)) // DR (data register)
#define GPIOD_ODR (*(volatile unsigned int *)(GPIOD_BASE + 0x14)) // GPIOD_ODR (output data register) is at offset 0x14
#define GPIOD_BSRR (*(volatile unsigned int *)(GPIOD_BASE + 0x18)) // GPIOD_BSRR (bit set/reset register) is at offset 0x18