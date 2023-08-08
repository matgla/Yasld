#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# printers.py
#
# Copyright (C) 2023 Mateusz Stadnik <matgla@live.com>
#
# This program is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation, either version
# 3 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General
# Public License along with this program. If not, see
# <https://www.gnu.org/licenses/>.
#

from colorama import Fore, Style, init 

class Logger:
    def __init__(self, verbose, disable):
        self.verbose_output = verbose 
        self.disable = disable 

    def verbose(self, text):
        if self.verbose_output and not self.disable: 
            print(Fore.BLUE + "  [VERBOSE] " + Style.RESET_ALL + text)

    def info(self, text):
        if not self.disable:
            print(Fore.GREEN + "  [INFO]    " + Style.RESET_ALL + text)

    def log(self, text): 
        if not self.disable:
            print(text)

    def error(self, text):
        if not self.disable:
            print(Fore.RED + "  [ERROR]   " + Style.RESET_ALL + text)

    def step(self, text):
        if not self.disable:
            print(Fore.CYAN + "[STEP]    " + Style.RESET_ALL + text)

    
