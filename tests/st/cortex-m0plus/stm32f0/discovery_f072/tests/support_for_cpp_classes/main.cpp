/**
 * main.cpp
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

#include "board_init.hpp"

#include <cstdio>
#include <cstring>
#include <memory>
#include <type_traits>

#include <yasld/environment.hpp>
#include <yasld/loader.hpp>

#include "../../modules/classes/interface.hpp"

ExternalImplementation::ExternalImplementation(
  std::string_view name,
  int              a,
  int              b)
  : name_(name)
  , a_(a)
  , b_(b)
{
  printf("ExternalImplementation(%s)\n", name.data());
}

ExternalImplementation::~ExternalImplementation()
{
  printf("~ExternalImplementation()\n");
}

std::string_view ExternalImplementation::get_name()
{
  return name_;
}
int ExternalImplementation::sum()
{
  return a_ - b_;
}

void create_external_implementation(
  ExternalImplementation *self,
  std::string_view        name,
  int                     a,
  int                     b)
{
  printf("Create external implementation at: %p\n", static_cast<void *>(self));
  new (self) ExternalImplementation(name, a, b);
  printf("Placement new finished\n");
}

int sum_wrapper(ExternalImplementation *self)
{
  return self->sum();
}

int main(int argc, char *argv[])
{
  static_cast<void>(argc);
  static_cast<void>(argv);
  board_init();
  puts("[host] STM32F0 Discovery Board started!");

  const yasld::StaticEnvironment environment{
    yasld::SymbolEntry{ "printf", &printf },
    yasld::SymbolEntry{ "puts", &puts },
    yasld::SymbolEntry{ "_Znwj",
                       static_cast<void *(*)(size_t)>(&operator new) },
    yasld::SymbolEntry{ "strlen", &strlen },
    yasld::SymbolEntry{
                       "_ZdlPvj", static_cast<void (*)(void *, size_t)>(&operator delete) },
    yasld::SymbolEntry{ "_ZN22ExternalImplementationC1ESt17basic_string_"
                        "viewIcSt11char_traitsIcEEii", &create_external_implementation },
    yasld::SymbolEntry{ "_ZN22ExternalImplementation3sumEv", &sum_wrapper },
  };

  yasld::Loader loader(
    [](std::size_t size, yasld::AllocationType)
    {
      return malloc(size);
    },
    [](void *ptr)
    {
      free(ptr);
    });

  loader.set_environment(environment);

  void *module     = reinterpret_cast<void *>(0x08011000);
  auto  executable = loader.load_executable(module);

  if (executable)
  {
    printf("[host] Module loaded\n");
    char  arg[]  = { "executable" };
    char *args[] = { arg };

    (*executable)->execute(1, args);
    printf("Errno is: %s\n", strerror(errno));
  }
  else
  {
    printf("[host] Module loading failed\n");
  }

  while (true)
  {
  }
}
