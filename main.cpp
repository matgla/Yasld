/*
 *   Copyright (c) 2021 Mateusz Stadnik

 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.

 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.

 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <dlfcn.h>

#include <iostream>

int main()
{
    void* lib = dlopen("build/tests/modules/module_a/libmodule_a.so", RTLD_LOCAL | RTLD_LAZY);
    if (!lib)
    {
        std::cerr << "Failed to load library: " << dlerror() << std::endl;
    }
    
    int (*fun)(int, int);
    *(void**)(&fun) = dlsym(lib, "sum");

    std::cout << "Sum (3, 5): " << fun(3, 5) << std::endl;
}