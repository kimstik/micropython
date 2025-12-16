/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Konstantin Kim
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <string.h>

// CH592 GPIO registers
#define R32_PB_DIR          (*(volatile uint32_t *)0x400010C0)
#define R32_PB_OUT          (*(volatile uint32_t *)0x400010C8)
#define GPIO_Pin_10         (1 << 10)

// CH592 UART0 registers
#define R8_UART0_IER        (*(volatile uint8_t *)0x40003001)
#define R8_UART0_FCR        (*(volatile uint8_t *)0x40003002)
#define R8_UART0_LCR        (*(volatile uint8_t *)0x40003003)
#define R8_UART0_LSR        (*(volatile uint8_t *)0x40003005)
#define R8_UART0_THR        (*(volatile uint8_t *)0x40003008)
#define R16_UART0_DL        (*(volatile uint16_t *)0x4000300C)
#define R8_UART0_DIV        (*(volatile uint8_t *)0x4000300E)

// UART register bits
#define RB_IER_TXD_EN       0x40
#define RB_FCR_FIFO_EN      0x01
#define RB_FCR_TX_FIFO_CLR  0x04
#define RB_FCR_RX_FIFO_CLR  0x02
#define RB_FCR_FIFO_TRIG_2B (2 << 6)
#define RB_LCR_WORD_SZ      0x03
#define RB_LSR_TX_FIFO_EMP  0x20

extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss, _eusrstack;

static inline void uart_write_char(int c);
void Reset_Handler(void) __attribute__((naked, section(".init")));
void bare_main(void);

// The CPU runs this function after a reset.
void Reset_Handler(void) {
    __asm volatile (
        "la sp, _eusrstack\n"
        ".option push\n"
        ".option norelax\n"
        "la gp, __global_pointer$\n"
        ".option pop\n"
        // QingKe pipeline control
        "csrwi 0xbc0, 0x1f\n"
        // Enable nested interrupt and hardware stack
        "csrwi 0x804, 0x3\n"
        );

    memcpy(&_sdata, &_sidata, (char *)&_edata - (char *)&_sdata);
    memset(&_sbss, 0, (char *)&_ebss - (char *)&_sbss);

    // PB10 as output for debug
    R32_PB_DIR |= GPIO_Pin_10;
    R32_PB_OUT |= GPIO_Pin_10;  // Set high on entry

    // UART0: 115200 baud @ 32MHz (default clock), 8N1
    // Divisor = (10 * 32000000 / 8 / 115200 + 5) / 10 = 35
    R16_UART0_DL = 35;
    R8_UART0_FCR = RB_FCR_FIFO_TRIG_2B | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
    R8_UART0_LCR = RB_LCR_WORD_SZ;
    R8_UART0_IER = RB_IER_TXD_EN;
    R8_UART0_DIV = 1;

    R32_PB_OUT ^= GPIO_Pin_10;  // Toggle before bare_main

    bare_main();

    for (;;) {
        R32_PB_OUT ^= GPIO_Pin_10;  // Toggle in idle loop
        uart_write_char('.');
    }
}

static inline void uart_write_char(int c) {
    while ((R8_UART0_LSR & RB_LSR_TX_FIFO_EMP) == 0) {
    }
    R8_UART0_THR = c;
}

void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    while (len--) {
        if (*str == '\n') {
            uart_write_char('\r');
        }
        uart_write_char(*str++);
    }
}
