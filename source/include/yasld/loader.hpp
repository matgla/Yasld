/**
 * loader.hpp
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

#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace yasld
{

class Loader
{
public:
  Loader(std::span<std::byte> memory_for_lot, std::span<std::byte> memory_for_app);
  void load_module(const void *module_address);
  //  Loader();
  //
  //  enum class Mode : uint8_t
  //  {
  //    CopyData,
  //    CopyTextAndData
  //  };
  //
  //  // Todo: add way to load from not mapped memory, like mmc card
  //  const LoadedModule *load_module(const void *module_address);

  std::span<std::byte> memory_for_lot_;
  std::span<std::byte> memory_for_app_;
};

} // namespace yasld
