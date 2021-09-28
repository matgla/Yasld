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

#include <iostream> 

#include "msos/dynamic_linker/dynamic_linker.hpp"
#include "msos/dynamic_linker/environment.hpp"
#include "msos/dynamic_linker/symbol.hpp"

const char* filename = LIB_PATH;

extern "C"
{
void my_print(const char* data)
{
    std::cout << "My printf called: " << data << std::endl;
}
} // extern "C"

int main()
{
    msos::dl::DynamicLinker dl;
    static msos::dl::Environment env{
        msos::dl::SymbolAddress{0, &my_print},
    };

    eul::error::error_code ec;
    std::cout << "Load file: " << filename << std::endl;
    const auto *module = dl.load_module(reinterpret_cast<const std::size_t*>(filename), msos::dl::LoadingModeCopyData, env, ec);
    module->execute();
}
