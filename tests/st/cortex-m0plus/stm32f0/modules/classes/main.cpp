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

#include <cstdio>
#include <memory>
#include <string_view>

#include "interface.hpp"

extern "C"
{
  void __cxa_pure_virtual()
  {
    printf("Pure virtual function called!\n");
    while (1)
    {
    }
  }
} // extern "C"

class InternalImplementation : public InternalClassInterface
{
public:
  InternalImplementation(const std::string_view &name, int a, int b)
    : a_(a)
    , b_(b)
    , name_(name)
  {
  }

  ~InternalImplementation()
  {
    printf("~InternalImplementation()\n");
  }

  std::string_view get_name() override
  {
    return name_;
  }

  int sum() override
  {
    return static_cast<int>(name_.size()) + a_ + b_;
  }

private:
  int              a_;
  int              b_;
  std::string_view name_;
};

class TestClassModule
{
public:
  TestClassModule()
  {
    printf("TestClassModule()\n");
  }

  ~TestClassModule()
  {
    printf("~TestClassModule()\n");
  }
};

int main(int argc, char *argv[])
{
  static_cast<void>(argc);
  static_cast<void>(argv);

  {
    printf("Test local scope\n");
    TestClassModule t;
  }

  InternalClassInterface *i =
    new InternalImplementation("its working :) ", 19, -45);
  printf("Sum from internal: %d\n", i->sum());
  printf("and the name is: %s\n", i->get_name().data());
  delete i;

  InternalClassInterface *e =
    new ExternalImplementation("other implementation", 12, 8);
  printf("Sum from external: %d\n", e->sum());
  printf("and the name is: %s\n", e->get_name().data());

  {
    std::unique_ptr<InternalClassInterface> ui(
      std::make_unique<InternalImplementation>("unique_ptr test", 20, 23));
    printf("Sum from internal unique_ptr: %d\n", ui->sum());
    printf("and the name is: %s\n", ui->get_name().data());
  }

  std::unique_ptr<InternalClassInterface> ue(
    std::make_unique<ExternalImplementation>(
      "this is another external implementation", 12, 199292));
  printf("Sum from external unique_ptr: %d\n", ue->sum());
  printf("and the name is: %s\n", ue->get_name().data());

  printf("CleanUp\n");
  delete e;
}
