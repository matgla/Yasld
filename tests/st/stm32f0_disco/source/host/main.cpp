/**
 * hello.cpp
 *
 * Copyright (C) 2023 Mateusz Stadnik <matgla@live.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version
 * 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General
 * Public License along with this program. If not, see
 * <https://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include <cstdio>
#include <span>
#include <string_view>

#include <yasld/loader.hpp>

void clock_setup()
{
  rcc_periph_clock_enable(RCC_GPIOC);
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_USART1);
}

void usart_setup()
{
  usart_set_baudrate(USART1, 115200);
  usart_set_databits(USART1, 8);
  usart_set_parity(USART1, USART_PARITY_NONE);
  usart_set_stopbits(USART1, USART_CR2_STOPBITS_1);
  usart_set_mode(USART1, USART_MODE_TX);
  usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
  usart_enable(USART1);
}

void gpio_setup()
{
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);
  gpio_set_af(GPIOA, GPIO_AF1, GPIO9);
}

std::array<uint8_t, 1024> module;

int                       main(int argc, char *argv[])
{
  clock_setup();
  gpio_setup();
  usart_setup();

  printf("STM32F0 Host implementation started!\n");

  std::array<std::byte, 8 * 1024> ram_app;
  std::array<std::size_t, 128>    ram_lot;

  std::span<std::size_t>          lot(ram_lot);
  std::span<std::byte>            app(ram_app);

  printf("Loader creation\n");
  yasld::Loader loader(lot, app);
  printf("New module\n\n\n\n\n\n");
  loader.load_module(reinterpret_cast<const void *>(0x08010000));

  while (true)
  {
  }
}
