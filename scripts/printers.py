# This file is part of MSOS project. This is simple OS for embedded development devices.
# Copyright (C) 2019 Mateusz Stadnik
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

from colorama import Fore, Style, init

enable_debugs = False
enable_logs = True

def print_debug(text):
    global enable_debugs
    if enable_debugs and enable_logs:
        print(Fore.BLUE + "[DBG] " + Style.RESET_ALL + text)

def print_error(text):
    print(Fore.RED + "[ERR] " + text + Style.RESET_ALL)

def print_step(text):
    global enable_logs
    if enable_logs:
        print(Fore.YELLOW + "[INF] " + Style.RESET_ALL + text)

def print_menu(text):
    global enable_logs
    if enable_logs:
        print(Fore.CYAN + text + Style.RESET_ALL)

def enable_debugs():
    global enable_debugs
    enable_debugs = True

def disable_prints():
    global enable_logs
    enable_logs = False
