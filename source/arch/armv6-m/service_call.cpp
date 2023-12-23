/**
 * service_call.cpp
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

#include "yasld/service_call.hpp"

#include <cstdlib>

#include "yasld/logger.hpp"

namespace yasld
{

void process_entry_service_call(Loader *loader, std::size_t *args)
{
  yasld::Module *m = loader->find_module(args[2]);
  if (m == nullptr)
  {
    log("Can't find module at 0x%x\n", args[2]);
    return;
  }
  m->save_caller_state({
    .r9 = args[0], .lr = args[1], .tmpReg = {args[3], args[4]}
  });
  args[0] = reinterpret_cast<std::size_t>(m->get_lot().data());
}

void process_exit_service_call(Loader *loader, std::size_t *args)
{
  yasld::Module *m = loader->find_module(args[0]);
  if (m == nullptr)
  {
    log("Danger module destroyed during work, system state is unknown\n");
    return;
  }
  const auto s = m->restore_caller_state();
  args[0]      = s.lr;
  args[1]      = s.r9;
  args[2]      = s.tmpReg[0];
  args[3]      = s.tmpReg[1];
}

} // namespace yasld
