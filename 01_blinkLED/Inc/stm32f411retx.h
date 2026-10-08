/*
 * stm32f411retx.h
 *
 *  Created on: Aug 13, 2026
 *      Author: Kien
 */

#ifndef STM32F411RETX_H_
#define STM32F411RETX_H_

#include <stdint.h>
//RCC enable Clock
#define RCC_BASE 0x40023800UL
#define RCC_AHB1ENR (*(volatile uint32_t *)(RCC_BASE + 0x30)) // //RCC + offset + cast
//GPIOA
#define GPIOA_BASE (0x40020000UL) // base address
//address memory mapped I/O
#define GPIOA_MODER (*(volatile uint32_t *)(GPIOA_BASE + 0x00)) //GPIO BASE + offser MODER + cast
#define GPIOA_ODR (*(volatile uint32_t *)(GPIOA_BASE + 0x14)) // GPIO BASE + offser ODR

#endif /* STM32F411RETX_H_ */
 