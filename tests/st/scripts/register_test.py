#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# register_test.py
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

import yaml
import argparse
import os
import sys
from pathlib import Path

parser = argparse.ArgumentParser(
    description="""
Script to register test in yaml tests file
"""
)

parser.add_argument(
    "-i",
    "--input",
    dest="file",
    action="store",
    help="Path to test file to be registered",
    required=True,
)

parser.add_argument(
    "-f",
    "--tests_set",
    dest="tests_file",
    action="store",
    help="Path to YAML file containing tests",
    required=True,
)

args, _ = parser.parse_known_args()

current_tests = []

if os.path.exists(Path(args.tests_file).absolute()):
    with open(args.tests_file, "r") as file:
        current_tests = yaml.safe_load(file.read())
        if current_tests is None:
            current_tests = []

test_path = Path(args.file).absolute()
if str(test_path) in current_tests:
    sys.exit(0)
else:
    current_tests.append(str(test_path))
    print("Appending test: " + str(test_path))
    with open(args.tests_file, "w") as file:
        file.write(yaml.safe_dump(current_tests))
