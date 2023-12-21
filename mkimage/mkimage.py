#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# mkimage.py
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

import argparse
import struct

from logger import LoggerSet, FileLogger, StdoutLogger

from colorama import Fore

from elf_parser import ElfParser
from relocation_set import RelocationSet
from enum import Enum


class SectionCode(Enum):
    Code = 0
    Data = 1
    Unknown = 2


def parse_cli_arguments():
    parser = argparse.ArgumentParser(
        description="""
                    MKImake converts ELF file to relocatable YASIFF modules.
                    It supports executables and libraries (WIP).
                    """
    )

    parser.add_argument(
        "-i",
        "--input",
        dest="input",
        action="store",
        help="Path to ELF file to be converted",
        required=True,
    )
    parser.add_argument(
        "-o",
        "--output",
        dest="output",
        action="store",
        help="Path to generated YASIFF",
    )
    parser.add_argument(
        "-v",
        "--verbose",
        dest="verbose",
        action="store_true",
        help="Enable verbose mode",
    )
    parser.add_argument(
        "-q",
        "--quiet",
        dest="quiet",
        action="store_true",
        help="Disable stdout output",
    )
    parser.add_argument(
        "-d", "--dryrun", dest="dryrun", action="store_true", help="Dry run"
    )
    parser.add_argument(
        "-l", "--log", dest="log", action="store", help="Path to log file"
    )

    args, _ = parser.parse_known_args()
    return args


class Application:
    def __init__(self, args):
        self.args = args
        self.logger = LoggerSet()
        self.logger.register_logger(StdoutLogger(self.args.verbose, self.args.quiet))
        if self.args.log:
            self.logger.register_logger(
                FileLogger(self.args.verbose, True, self.args.log)
            )

    def __print_header(self):
        self.logger.log(" ===========================================", Fore.YELLOW)
        self.logger.log("|    MKIMAGE - (ELF to YASIFF converter)    |", Fore.YELLOW)
        self.logger.log(" ===========================================", Fore.YELLOW)

    def __validate_section(self, section, position, name):
        if section == None:
            self.logger.error("Section '{}' not found!".format(name))
            self.logger.error("Existing sections: " + str(self.elf.sections.keys()))
            raise RuntimeError("Section validation failed")

        if section["address"] != position:
            self.logger.error(section["name"] + " is not placed at: " + hex(position))
            self.logger.error("Current address is: " + hex(section["address"]))
            raise RuntimeError("Section validation failed")

    def __fetch_section(self, name, position):
        section = self.elf.sections[name]
        self.__validate_section(section, position, name)
        self.logger.info("Found '" + name + "' with size: " + hex(section["size"]))
        return section

    def __fetch_sections(self):
        self.logger.info("Fetching sections")
        self.elf = ElfParser(self.args.input)
        self.text_section = self.__fetch_section(".text", 0x00000000)
        self.text = bytearray(self.text_section["data"])

        data_section_address = self.text_section["address"] + self.text_section["size"]
        self.data_section = self.__fetch_section(".data", data_section_address)
        self.data = bytearray(self.data_section["data"])

        bss_section_address = self.data_section["address"] + self.data_section["size"]
        self.bss_section = self.__fetch_section(".bss", bss_section_address)
        self.bss = bytearray(self.bss_section["data"])

    def __process_symbols(self):
        self.logger.info("Processing symbol table")
        self.symbols = {}
        for name, data in self.elf.symbols.items():
            if not name or name == "$t" or name == "$d":
                continue

            if data["type"] == "STT_FILE":
                continue

            is_main = name == "main"

            if name in self.symbols:
                self.logger.error("Found duplicated symbol: " + name)
                raise RuntimeError("Symbols processing failed")

            # Fix undefined symbols
            is_global_and_visible = (
                data["binding"] == "STB_GLOBAL" and data["visibility"] != "STV_HIDDEN"
            )

            self.symbols[name] = data

            print(
                "Name: ",
                name,
                " global:",
                is_global_and_visible,
                data["binding"],
                data["visibility"],
            )
            if is_global_and_visible or is_main:
                if data["section_index"] == "SHN_UNDEF":
                    self.symbols[name]["localization"] = "imported"
                else:
                    # only main can be exported in executable
                    if self.elf.is_executable():
                        if is_main:
                            self.symbols[name]["localization"] = "exported"
                        else:
                            self.symbols[name]["localization"] = "internal"
                    else:
                        self.symbols[name]["localization"] = "exported"
            else:
                self.symbols[name]["localization"] = "internal"

    def __print_symbol_table(self, visibility):
        symbols = dict(
            filter(lambda i: i[1]["localization"] == visibility, self.symbols.items())
        )

        if len(symbols) == 0:
            return

        self.logger.verbose(
            "+----------------------- {:-<8s} ------------------------+".format(
                visibility
            )
        )
        self.logger.verbose(
            "|                   name                   |    address   |"
        )
        self.logger.verbose(
            "+------------------------------------------+--------------+"
        )
        for symbol in symbols:
            self.logger.verbose(
                "| {: <40.40s} |  {: <10s}  |".format(
                    symbol, hex(self.symbols[symbol]["value"])
                )
            )

        self.logger.verbose(
            "+------------------------------------------+--------------+"
        )

    def __dump_symbol_table(self):
        if self.args.verbose:
            self.logger.verbose("Symbol table")
            self.__print_symbol_table("exported")
            self.__print_symbol_table("imported")
            self.__print_symbol_table("internal")

    def __process_relocations(self):
        self.logger.verbose("Processing relocation table")
        # Only GOT relocations must be processed, great description for relocation
        # is placed under ARM ABI: https://github.com/ARM-software/abi-aa/blob/main/aaelf32/aaelf32.rst
        # Or Android documentation: https://android.googlesource.com/toolchain/gdb/+/refs/heads/honeycomb/gdb-6.4/bfd/elf32-arm.c

        skipped_relocations = [
            "R_ARM_CALL",  # PC Relative
            "R_ARM_JUMP24",  # PC Relative
            "R_ARM_THM_CALL",  # PC Relative
            "R_ARM_ABS32",  # allowed for .data
            "R_ARM_PREL31",  # PC relative
        ]

        self.relocations = RelocationSet()

        for relocation in self.elf.relocations:
            if relocation["info_type"] in skipped_relocations:
                continue
            elif relocation["info_type"] == "R_ARM_GOT_BREL":
                visibility = self.symbols[relocation["symbol_name"]]["localization"]
                if visibility == "internal":
                    self.relocations.add_local_relocation(relocation)
                else:
                    self.relocations.add_symbol_table_relocation(relocation)
            else:
                raise RuntimeError(
                    "Unknown relocation for '{name}': {relocation}".format(
                        name=relocation["symbol_name"],
                        relocation=relocation["info_type"],
                    )
                )

    def __process_data_relocations(self, data_offset):
        self.logger.verbose("Processing data relocations")

        skipped_relocations = [
            "R_ARM_CALL",  # PC Relative
            "R_ARM_JUMP24",  # PC Relative
            "R_ARM_GOT_BREL",  # GOT
            "R_ARM_THM_CALL",  # PC Relative
        ]

        for relocation in self.elf.relocations:
            if relocation["info_type"] in skipped_relocations:
                continue
            elif relocation["info_type"] == "R_ARM_ABS32":
                if relocation["symbol_name"] in self.symbols:
                    if self.elf.get_section_name(relocation["section_index"]) == None:
                        self.logger.error(
                            "Relocation '{}' towards unsupported sections: {}".format(
                                relocation["symbol_name"], relocation["section_index"]
                            )
                        )
                        raise RuntimeError("Data relocation processing failure")
                    from_address = int(relocation["offset"] - data_offset)
                    if from_address < 0:
                        self.logger.error(
                            "Only .data relocations are allowed, symbol '{}' relocation inside .text".format(
                                relocation["symbol_name"]
                            )
                        )
                        self.logger.error(
                            "Original relocation offset: " + hex(relocation["offset"])
                        )
                        raise RuntimeError("Data relocation processing failure")
                    original_offset = struct.unpack_from("<I", self.data, from_address)[
                        0
                    ]

                    if relocation["symbol_value"] <= data_offset:
                        offset = original_offset << 1
                    else:
                        offset = ((original_offset - data_offset) << 1) | 1

                    self.relocations.add_data_relocation(
                        relocation, from_address, offset
                    )

    def __dump_local_relocations(self):
        self.logger.verbose("Dumping local relocations")
        rels = self.relocations.get_relocations("local")
        if len(rels) == 0:
            return

        self.logger.verbose(
            "+------------------------------------------+-------| local |--------------+-----------------+---------+"
        )
        self.logger.verbose(
            "|                   name                   |   lot   |       offset       |      value      | section |"
        )
        self.logger.verbose(
            "+------------------------------------------+---------+--------------------+-----------------+---------+"
        )

        for rel in rels:
            self.logger.verbose(
                "| {: <40s} | {: <7} | {: <18} | {: <15} | {: <7s} |".format(
                    rel["name"],
                    rel["index"],
                    hex(rel["offset"]),
                    hex(rel["symbol_value"]),
                    self.elf.get_section_name(rel["section"]),
                )
            )
        self.logger.verbose(
            "+------------------------------------------+---------+--------------------+-----------------+---------+"
        )

    def __dump_symbol_table_relocations(self):
        self.logger.step("Dumping symbol table relocations")
        rels = self.relocations.get_relocations("symbol_table")
        if len(rels) == 0:
            return

        self.logger.verbose(
            "+----------------------------------------| symbol table |-----------------+-----------------+"
        )

        self.logger.verbose(
            "|                   name                   |   lot   |       offset       |      value      |"
        )
        self.logger.verbose(
            "+------------------------------------------+---------+--------------------+-----------------+"
        )

        for rel in rels:
            self.logger.verbose(
                "| {: <40} | {: <7} | {: <18} | {: <15} |".format(
                    rel["name"],
                    rel["index"],
                    hex(rel["offset"]),
                    hex(rel["symbol_value"]),
                )
            )
        self.logger.verbose(
            "+------------------------------------------+---------+--------------------+-----------------+"
        )

    def __dump_data_relocations(self):
        self.logger.step("Dumping data relocations")
        rels = self.relocations.get_relocations("data")
        if len(rels) == 0:
            return

        self.logger.verbose(
            "+------------------------------------------|  data   |--------+------------------+-----------+"
        )
        self.logger.verbose(
            "|                   name                   |    from offset   |     to offset    |  section  |"
        )
        self.logger.verbose(
            "+------------------------------------------+------------------+------------------+-----------+"
        )

        for rel in rels:
            if rel["offset"] & 0x1:
                section = ".data"
            else:
                section = ".text"
            from_offset = rel["offset"] >> 1

            self.logger.verbose(
                "| {: <40} | {: <16} | {: <16} | {: <9} |".format(
                    rel["name"], hex(from_offset), hex(rel["index"]), section
                )
            )

        self.logger.verbose(
            "+------------------------------------------+----------------+--------------------+-----------+"
        )

    def __dump_relocations(self):
        self.__dump_local_relocations()
        self.__dump_symbol_table_relocations()
        self.__dump_data_relocations()

    def __process_elf_file(self):
        self.logger.step("Processing ELF file: " + self.args.input)
        self.__fetch_sections()
        self.__process_symbols()
        self.__dump_symbol_table()
        self.__process_relocations()
        self.__process_data_relocations(
            self.text_section["address"] + self.text_section["size"]
        )
        self.__dump_relocations()

    def __fix_offsets_in_code(self):
        self.logger.step("Fixing offsets in code section")
        self.logger.verbose(
            "+------- at --------+------ from -------+------- to --------+------ symbol --------+"
        )

        for rel in self.relocations.relocations:
            if rel["type"] == "data":
                continue

            old = struct.unpack_from("<I", self.text, rel["offset"])[0]
            new = rel["index"] * 4
            struct.pack_into("<I", self.text, rel["offset"], new)
            self.logger.verbose(
                "| {: <17} | {: <17} | {: <17} | {}".format(
                    hex(rel["offset"]), hex(old), hex(new), rel["name"]
                )
            )

        for rel in self.relocations.omitted_relocations:
            if rel["type"] == "data":
                continue

            old = struct.unpack_from("<I", self.text, rel["offset"])[0]
            new = rel["index"] * 4
            struct.pack_into("<I", self.text, rel["offset"], new)
            self.logger.verbose(
                "| {: <17} | {: <17} | {: <17} | {}".format(
                    hex(rel["offset"]), hex(old), hex(new), rel["name"]
                )
            )

        self.logger.verbose(
            "+-------------------+-------------------+-------------------+"
        )

    def __get_symbol_section(self, symbol):
        index = symbol["section_index"]
        if index == self.text_section["index"]:
            return SectionCode.Code
        elif index == self.data_section["index"] or index == self.bss_section["index"]:
            return SectionCode.Data
        return None

    def __get_relocation_section(self, relocation):
        index = relocation["section"]
        if index == self.text_section["index"]:
            return SectionCode.Code
        elif index == self.data_section["index"] or index == self.bss_section["index"]:
            return SectionCode.Data

    def __build_symbol_tables(self):
        self.exported_symbol_table = []
        self.imported_symbol_table = []

        for symbol, data in self.symbols.items():
            visibility = data["localization"]
            if visibility == "exported":
                self.exported_symbol_table.append(
                    {
                        "section": self.__get_symbol_section(data),
                        "value": data["value"],
                        "name": symbol,
                    }
                )

            elif visibility == "imported":
                self.imported_symbol_table.append(
                    {
                        "section": self.__get_symbol_section(data),
                        "value": data["value"],
                        "name": symbol,
                    }
                )

    def __filter_relocations(self, visibility, skip_duplications):
        filtered = []
        processed = []

        for rel in self.relocations.get_relocations(visibility):
            if rel["name"] in processed and skip_duplications:
                continue
            filtered.append(rel)
            processed.append(rel["name"])
        return filtered

    @staticmethod
    def __align_bytes(data, alignment):
        if len(data) % alignment != 0:
            return data + bytearray(alignment - len(data) % alignment)
        return data

    def __build_binary_symbol_table_for(self, symbols):
        table = bytearray()
        for symbol in symbols:
            value = symbol["value"]
            if symbol["section"] == SectionCode.Data:
                value -= len(self.text)

            # if undefined treat same as text
            section = symbol["section"].value if symbol["section"] is not None else 0
            value = value << 1 | section
            table += struct.pack("<I", value)
            table += Application.__align_bytes(
                bytearray(symbol["name"] + "\0", "ascii"), 4
            )

        return table

    def __build_binary_relocation_table(
        self,
        symbol_table_relocations,
        local_relocations,
        data_relocations,
        imported_symbol_table,
    ):
        table = []

        for rel in symbol_table_relocations:
            symbol = None
            symbol_table_index = 0
            for s in imported_symbol_table:
                if s["name"] == rel["name"]:
                    symbol = s
                    break
                else:
                    symbol_table_index += 1

            if not symbol:
                raise RuntimeError(
                    "Symbol {} not found in symbol table.".format(rel["name"])
                )
            table.append({"index": rel["index"], "offset": symbol_table_index})

        for rel in local_relocations:
            section = self.__get_relocation_section(rel)
            value = rel["symbol_value"]
            if section == SectionCode.Data:
                value -= len(self.text)

            if section == SectionCode.Unknown:
                raise RuntimeError(
                    "Unknown section for symbol '{}'".format(rel["name"])
                )

            index_with_section = rel["index"] << 1 | section.value
            table.append({"index": index_with_section, "offset": value})

        for rel in data_relocations:
            table.append({"index": rel["index"], "offset": rel["offset"]})

        return table

    def __build_image(self):
        self.logger.step("Building Yasiff image")
        image = bytearray("YAFF", "ascii")
        image += struct.pack("<BHB", 1, 1, 1)  # dummy values for now
        image += struct.pack("<III", len(self.text), len(self.data), len(self.bss))
        image += struct.pack("<HBB", 0, 4, 0)
        image += struct.pack("<HH", 0, 0)

        symbol_table_relocations = self.__filter_relocations("symbol_table", True)
        local_relocations = self.__filter_relocations("local", False)
        data_relocations = self.__filter_relocations("data", False)

        image += struct.pack(
            "<HHHH",
            len(symbol_table_relocations),
            len(local_relocations),
            len(data_relocations),
            0,  # TODO: Update header
        )

        exported_symbol_table = self.__build_binary_symbol_table_for(
            self.exported_symbol_table
        )

        imported_symbol_table = self.__build_binary_symbol_table_for(
            self.imported_symbol_table
        )

        relocations = self.__build_binary_relocation_table(
            symbol_table_relocations,
            local_relocations,
            data_relocations,
            self.imported_symbol_table,
        )

        image += struct.pack(
            "<HH", len(self.exported_symbol_table), len(self.imported_symbol_table)
        )

        for rel in relocations:
            image += struct.pack("<II", rel["index"], rel["offset"])

        image += imported_symbol_table
        image += exported_symbol_table

        image = Application.__align_bytes(image, 16)
        image += self.text
        image += self.data
        self.image = image
        if not self.args.dryrun:
            self.logger.info("Writing to: " + self.args.output)
            with open(self.args.output, "wb") as file:
                file.write(image)

    def execute(self):
        self.__print_header()
        self.__process_elf_file()
        self.__fix_offsets_in_code()
        self.__build_symbol_tables()
        self.__build_image()


if __name__ == "__main__":
    args = parse_cli_arguments()
    app = Application(args)
    app.execute()
