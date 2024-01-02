#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# build_binary.py
#
# Copyright (C) 2024 Mateusz Stadnik <matgla@live.com>
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

import argparse
import os

parser = argparse.ArgumentParser(
    description="""
Target binary builder which combines many binary files
"""
)

parser.add_argument(
    "-i",
    "--input",
    action="store",
    nargs="*",
    help="List of inputs in form file_path:hexadecimal_size;another_file_path:another_hexadecimal_size",
)

parser.add_argument("-o", "--output", action="store", help="Path to output file")

args, _ = parser.parse_known_args()

image = bytearray()

if os.path.exists(args.output):
    os.remove(args.output)

for line in args.input:
    parts = line.split(":")
    filename = parts[0]
    size = int(parts[1], 16)
    with open(filename, "rb") as file:
        part = file.read()
        if len(part) > size:
            raise RuntimeError(
                "File not fit to image size: ",
                hex(len(part)),
                ", requested: ",
                hex(size),
            )
        append = size - len(part)
        print("Adding", filename, ", at: ", hex(len(image)))
        image += part
        image += bytearray(append)
        print("Appended", filename, ", now size is: ", hex(len((image))))


with open(args.output, "wb") as file:
    file.write(image)
