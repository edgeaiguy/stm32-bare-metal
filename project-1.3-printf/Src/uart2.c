#include <stdint.h>
#include <stdarg.h>
#include "stm32f407xx.h"

/* perform UART tranmission: 10 bits per transmission (start/stop bits + 1 byte of data)*/
void uart2_write_byte(char ch) {
  // wait until TXE flag in SR is set (bit 7). then write ch to DR
  while (!(USART2_SR & (1 << 7))) {}
  USART2_DR = (unsigned char)ch;
}

/* loop through each character in a string for UART transmission */
void uart2_write_string(const char *str) {
  // note: *str is 'falsy' when hitting the null terminator ('\0'), ending the loop
  while (*str) {
    uart2_write_byte(*str++);
  }
}

/* convert int to ASCII for UART transmission */
void uart2_write_int(int32_t value) {
  char buf[11]; // max: "-2147483648" = 10 chars + null
  int i = 0;
  // handle negative
  if (value < 0) {
    uart2_write_byte('-');
    value = -value;
  }
  // handle zero
  if (value == 0) {
    uart2_write_byte('0');
    return;
  }
  // extract digits in reverse into buf
  while (value > 0) {
    buf[i++] = (value % 10) + '0'; // covert last digit into ASCII
    value /= 10; // continuously divide by 10
  }
  // send in reverse (correct) order
  while (i > 0) {
    uart2_write_byte(buf[--i]);
  }
}

/* convert unsigned int to ASCII for UART transmission */
void uart2_write_uint(uint32_t value) {
  char buf[11]; // max: "-2147483648" = 10 chars + null
  int i = 0;

  // handle zero
  if (value == 0) {
    uart2_write_byte('0');
    return;
  }
  // extract digits in reverse into buf
  while (value > 0) {
    buf[i++] = (value % 10) + '0'; // covert last digit into ASCII
    value /= 10; // continuously divide by 10
  }
  // send in reverse (correct) order
  while (i > 0) {
    uart2_write_byte(buf[--i]);
  }
}

void uart2_write_hex(uint32_t value) {
    uart2_write_string("0x");
    for (int i = 7; i >= 0; i--) {
        uint8_t nibble = (value >> (i * 4)) & 0xF;
        uart2_write_byte(nibble < 10 ? nibble + '0' : (nibble - 10) + 'A');
    }
}

void uart2_printf(const char *fmt, ...) {
  va_list args; // track position in argument list
  va_start(args, fmt); // initialize args after fmt
  // loop through the string
  while (*fmt) {
    if (*fmt == '%') {
      fmt++; // skip past '%'
      switch (*fmt) {
        case 'd': // pull next arg as int
          uart2_write_int(va_arg(args, int));
          break;
        case 'u': // pull next arg as unsigned int
          uart2_write_uint(va_arg(args, unsigned int));
          break;
        case 'x': // pull next arg as uint32_t
          uart2_write_hex(va_arg(args, uint32_t));  
          break;
        case 's': // pull next arg as string pointer
            uart2_write_string(va_arg(args, char *));  
            break;
        case 'c': // pull next arg as char
            uart2_write_byte((char)va_arg(args, int)); 
            break;
        case '%': // "%%" prints a literal %
            uart2_write_byte('%');   
            break;
      }
    } else {
      uart2_write_byte(*fmt); // regular character ,pass through
    }
    fmt++;
  }
  
  va_end(args); // clean up
}