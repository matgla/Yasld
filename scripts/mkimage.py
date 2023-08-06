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
# You should have received a copy of the GNU General
# Public License along with this program. If not, see
# <https://www.gnu.org/licenses/>.
#

import argparse 
import struct
import sys 

from enum import Enum

from colorama import Fore, Style, init

from printers import Logger 
from elf_parser import ElfParser 
from relocation import RelocationSet

class SectionCode(Enum):
    Code = 0 
    Data = 1 
    Unknown = 2

def parse_args():
    parser = argparse.ArgumentParser(description = 
                                    """
                                    MkImage converts ELF file to YASIFF.
                                    It generates relocatable executable or shared library for Cortex-M MCUs.
                                    """)

    parser.add_argument("-i", "--input", dest="input", action="store", help="Path to ELF file", required=True)
    parser.add_argument("-o", "--output", dest="output", action="store", help="Path to output file", required=True)
    parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="Enable verbose output")
    parser.add_argument("-q", "--quiet", dest="quiet", action="store_true", help="Disable output")
    parser.add_argument("-d", "--dry", dest="dryrun", action="store_true", help="Dry run")
    args, rest = parser.parse_known_args()
    return args



class Application:
    def __init__(self, args):
        self.args = args 
        self.logger = Logger(self.args.verbose, self.args.quiet)
        self.image = None 

    def print_header(self):
        self.logger.log(Fore.YELLOW + 
"""
#############################################
#    MKIMAGE - (ELF to YASIFF converter)    #
#############################################
""" + Style.RESET_ALL)

    def validate_section(self, section, position, section_name):
        if section == None:
            self.logger.error("Section not found in the file: " + section_name)
            self.logger.error("Sections available in the file: " + str(self.parser.get_sections().keys()))
            return False 
        if section["address"] != position:
            self.logger.error(section["name"] + " must be placed at address " + hex(position))
            return False
        
        return True

    def fetch_section(self, section_name, position):
        section = self.parser.get_section(section_name)
        if not self.validate_section(section, position, section_name):
            sys.exit(-1)
        self.logger.info("Found " + section_name + ", with size: " + hex(section["size"]))
        return section 

    def fetch_sections(self):
        self.logger.step("Parsing ELF file: " + self.args.input)
        self.parser = ElfParser(self.args.input)
        self.is_executable = self.parser.get_file_type() == "ET_EXEC"
        self.text_section = self.fetch_section(".text", 0x00000000)
        
        data_section_address = self.text_section["address"] + self.text_section["size"]
        self.data_section = self.fetch_section(".data", data_section_address)

        bss_section_address = self.data_section["address"] + self.data_section["size"]
        self.bss_section = self.fetch_section(".bss", bss_section_address)

    def process_symbols(self):
        self.logger.step("Processing symbol table") 
        self.processed_symbols = {}
        for name, data in self.parser.get_symbols().items():
            if not name or name == "$t" or name == "$d":
                continue 

            if data["type"] == "STT_FILE":
                continue 

            is_main = name == "main"
            is_global_and_visible = data["binding"] == "STB_GLOBAL" and data["visibility"] != "STV_HIDDEN"
            
            if name in self.processed_symbols:
                self.logger.error("Found duplicated symbol: " + name) 
                continue 

            self.processed_symbols[name] = data
            
            # If executable then hide all symbols except main
            if self.is_executable:
                if is_main: 
                    self.processed_symbols[name]["localization"] = "exported"
                else: 
                    self.processed_symbols[name]["localization"] = "internal"
            else:
                if is_main or is_global_and_visible:
                    if data["section_index"] == "SHN_UNDEF":
                        self.processed_symbols[name]["localization"] = "external"
                    else:
                        self.processed_symbols[name]["localization"] = "exported"
                else:
                    self.processed_symbols[name]["localization"] = "internal"
            

    def dump_processed_symbols(self):
        self.logger.verbose("Symbol table")
        if self.args.verbose: 
            items = self.processed_symbols.items() 
            exported = dict(filter(lambda i: i[1]["localization"] == "exported", items))
            
            self.logger.verbose("Exported symbols:")
            for s in exported: 
                self.logger.verbose("    " + s)

            external = dict(filter(lambda i: i[1]["localization"] == "external", items))
            self.logger.verbose("External symbols:")
            for s in external: 
                self.logger.verbose("    " + s)

            internal = dict(filter(lambda i: i[1]["localization"] == "internal", items))
            self.logger.verbose("Internal symbols:")
            for s in internal:
                self.logger.verbose("    " + s)
   
    def dump_processed_relocations(self):
        self.logger.verbose("Relocation table")
        if self.args.verbose:
            for rel in self.relocation_table.get_relocations():
                row = [rel["name"], rel["type"], hex(rel["offset"]),
                       str(rel["index"]), hex(rel["symbol_value"])]
                self.logger.verbose("{: >40.40} {: >20.20} {: >20.20} {: >10.10} {: >20.20}"
                                    .format(*row))

    def process_relocations(self):
        self.logger.step("Processing relocation table") 
        # Only GOT relocations must be added to GOT table
        # Great ARM ABI description for relocations: https://github.com/ARM-software/abi-aa/blob/main/aaelf32/aaelf32.rst
        # https://android.googlesource.com/toolchain/gdb/+/refs/heads/honeycomb/gdb-6.4/bfd/elf32-arm.c 
        skipped_relocations = [
            "R_ARM_THM_CALL", 
            "R_ARM_ABS32",
            "R_ARM_THM_JUMP24",
            "R_ARM_THM_JUMP11",
            "R_ARM_THM_JUMP8",
            "R_ARM_REL32",
            "R_ARM_PREL31",
            "R_ARM_CALL",
            "R_ARM_JUMP24",
        ]

        self.relocation_table = RelocationSet()

        for relocation in self.parser.get_relocations():
            if relocation["info_type"] in skipped_relocations:
                continue 
            elif relocation["info_type"] == "R_ARM_GOT_BREL":
                visibility = self.processed_symbols[relocation["symbol_name"]]["localization"]
                if visibility == "internal":
                    self.relocation_table.add_local_relocation(relocation)
                else:
                   self.relocation_table.add_relocation(relocation, visibility)
            else:
                raise RuntimeError("Unknown relocation: " + relocation["info_type"] + ". Mkimage must be updated!")
    
    def process_data_relocations(self, code_size):
        self.logger.info("Processing data relocations")
        skipped_relocations = [
            "R_ARM_THM_CALL",
            "R_ARM_GOT_BREL",
            "R_ARM_THM_JUMP24",
            "R_ARM_THM_JUMP11",
            "R_ARM_THM_JUMP8",
            "R_ARM_REL32",
            "R_ARM_PREL31",
            "R_ARM_CALL",
            "R_ARM_JUMP24"
        ] 

        for relocation in self.parser.get_relocations():
            if relocation["info_type"] in skipped_relocations:
                continue 
            elif relocation["info_type"] == "R_ARM_ABS32":
                if relocation["symbol_name"] in self.processed_symbols:
                    visibility = self.processed_symbols[relocation["symbol_name"]]["localization"]
                      
                    index = int(relocation["offset"] - code_size)
                    if (index < 0):
                      raise RuntimeError("Index negative for: " + str(relocation["symbol_name"]))
                    if relocation["symbol_value"] <= code_size:
                      # Relocation to code section 
                      original_offset = struct.unpack_from("<I", self.data, index)[0]
                      offset = original_offset << 1 
                    else:
                      original_offset = struct.unpack_from("<I", self.data, index)[0]
                      offset = ((original_offset - code_size) << 1) | 1
                    self.relocation_table.add_data_relocation(relocation, index, offset)
                else:
                    raise RuntimeError("Symbol not found in symbol table: " + relocation["symbol_name"])
            else:
                raise RuntimeError("Unknown relocation: " + relocation["info_type"] + ". Mkimage must be updated!")
            
    def fix_offsets_in_code(self):
        self.logger.step("Fixing offsets in code section")
        for rel in self.relocation_table.get_relocations():
            if rel["type"] == "data":
                continue 
            old = struct.unpack_from("<I", self.code, rel["offset"])[0]
            new = rel["index"] * 4
            struct.pack_into("<I", self.code, rel["offset"], new)
            self.logger.verbose("Fixing offset at: " + hex(rel["offset"]) 
                                + ", from: " + hex(old) + ", to: " + hex(new))
   
    @staticmethod
    def align_bytes(data, alignment):
        if len(data) % alignment != 0:
            return data + bytearray(alignment - len(data) % alignment) 
        return data 

    def get_relocations(self, visibility, skip_duplication):
        rels = self.relocation_table.get_relocations()
        processed_map = []
        relocations = []
        for r in rels:
            if r["type"] != visibility:
                continue
            if r["name"] in processed_map and skip_duplication:
                continue 

            relocations.append(r)
            processed_map.append(r["name"])
        return relocations

    def get_symbol_section(self, symbol):
        if symbol["section_index"] == self.text_section["index"]:
            return SectionCode.Code 
        elif symbol["section_index"] == self.bss_section["index"] \
            or symbol["section_index"] == self.data_section["index"]:
            return SectionCode.Data 

        return SectionCode.Unknown

    def get_relocation_section(self, relocation):
        if relocation["section"] == self.text_section["index"]:
            return SectionCode.Code 
        elif relocation["section"] == self.bss_section["index"] \
            or relocation["section"] == self.data_section["index"]:
            return SectionCode.Data 
        return SectionCode.Unknown

    def build_symbol_table(self):
        self.exported_symbol_table = []
        self.external_symbol_table = []
        self.exported_symbol_table_size = 0
        self.external_symbol_table_size = 0
        for symbol, data in self.processed_symbols.items(): 
            visibility = data["localization"] 
            if visibility == "exported":
                self.exported_symbol_table_size += 1
                self.exported_symbol_table.append({
                    "section": self.get_symbol_section(data),
                    "value": data["value"],
                    "name": symbol
                })             
            elif visibility == "external":
                self.external_symbol_table_size += 1
                self.external_symbol_table.append({
                    "section": self.get_symbol_section(data),
                    "value": data["value"],
                    "name": symbol
                })             

    def calculate_offset_to_exported_symbol(self, name):
        offset = 0
        for sym in self.exported_symbol_table:
            if sym["name"] == name:
                return offset
            # section + symbol name size + \0
            offset += 1 + len(sym["name"]) + 1

        raise RuntimeError("Can't find symbol on exported list: " + name)

    def calculate_offset_to_external_symbol(self, name):
        offset = 0
        for sym in self.external_symbol_table:
            if sym["name"] == name:
                return offset
            # section + symbol name size + \0
            offset += 1 + len(sym["name"]) + 1

        raise RuntimeError("Can't find symbol on exported list: " + name)

    def build_binary_symbol_table_for(self, symbols):
        symbol_table = bytearray() 
        for symbol in symbols:
            value = symbol["value"]
            if symbol["section"] == SectionCode.Data:
                value -= len(self.code)
           
            value = value << 1 | symbol["section"].value 
            symbol_table += struct.pack("<I", value)
            symbol_table += Application.align_bytes(bytearray(symbol["name"] + "\0", "ascii"), 4) 
        
        return Application.align_bytes(symbol_table, 4) 

    def build_relocation_table(self, exported_relocations, external_relocations, local_relocations, 
                            data_relocations, exported_symbol_table):
        relocation_position = 0
        size_of_relocation = 4 + 4
        relocation_table = []
       
        number_of_relocations = len(external_relocations) + len(local_relocations) \
            + len(data_relocations)
        
        if len(exported_relocations) > 0:
          raise RuntimeError("Add support for exported relocations")
#         for rel in exported_relocations:
#             # offset from current relocation to start of symbol table
#             offset_to_symbol_table = (number_of_relocations - relocation_position) * size_of_relocation 
#   
#             # calculate offset to symbol
#             offset_to_symbol = offset_to_symbol_table + self.calculate_offset_to_exported_symbol(rel["name"])
#             relocation_table.append({"index": rel["index"], "offset": offset_to_symbol}) 
#             relocation_position += 1
# 
        for rel in external_relocations:
            offset_to_symbol_table = len(exported_symbol_table) + (number_of_relocations - relocation_position) * size_of_relocation
            # offset to external symbols table
            offset_to_symbol = offset_to_symbol_table + self.calculate_offset_to_external_symbol(rel["name"])
            relocation_table.append({"index": rel["index"], "offset": offset_to_symbol}) 
            relocation_position += 1

        for rel in local_relocations:
            section = self.get_relocation_section(rel)
            value = rel["symbol_value"] 
            if section == SectionCode.Data:
                value -= len(self.code)

            if section == SectionCode.Unknown:
              raise RuntimeError("Unknown section code: " + str(section) + ", for symbol: " + rel["name"])

            index_with_section = rel["index"] << 1 | section.value
            relocation_table.append({"index": index_with_section, "offset": value}) 

        for rel in data_relocations:
            offset = rel["offset"]

            relocation_table.append({"index": rel["index"], "offset": offset}) 
        
        return relocation_table

    def print_relocations(self, local_relocations, exported_relocations, external_relocations, data_relocations):
        self.logger.verbose("  Local relocations:")  
        row = [Fore.BLUE + "name", "symbol value", "relocation offset", "relocation index" + Style.RESET_ALL]
        self.logger.verbose("{: >25} {: >20} {: >25} {: >22}".format(*row))
        for relocation in local_relocations:
            self.logger.verbose(str(relocation))

    def build_image(self):
        self.logger.step("Creating binary image") 
        self.image = bytearray("YAFF", "ascii")
        self.image += struct.pack("<BHB", 1, 1, 1) # some dummy values for now
        self.image += struct.pack("<III", len(self.code), len(self.data), len(self.bss))
        
        # Add here architecture section depending on arch enum 
       
        # This is necessary to select from which libraries select external symbols
        # In first iteration we can provide first known
        self.image += struct.pack("<HBB", 0, 4, 0) # no dependencies export supported right now, alignment 4
        # Followed by dependencies, but none for now, encoded in ASCII name + 4B version 
        # And at the end aligned to 4 

        self.image += struct.pack("<HH", 0, 0) # same for version 

        exported_relocations = self.get_relocations("exported", True)
        external_relocations = self.get_relocations("external", True)
        local_relocations = self.get_relocations("local", False)
        data_relocations = self.get_relocations("data", False)
       
        self.print_relocations(local_relocations, exported_relocations, 
                               external_relocations, data_relocations)

        self.image += struct.pack("<HHH", len(external_relocations),
                                  len(local_relocations),
                                  len(data_relocations))
                                  #len(exported_relocations))
 
        exported_symbol_table = self.build_binary_symbol_table_for(self.exported_symbol_table)
        external_symbol_table = self.build_binary_symbol_table_for(self.external_symbol_table) 
        relocations = self.build_relocation_table(exported_relocations, external_relocations,
                                                local_relocations, data_relocations, exported_symbol_table) 
       
        self.image += struct.pack("<HH", self.exported_symbol_table_size, self.external_symbol_table_size)

        self.image += struct.pack("<H", 0) # alignment 

        for rel in relocations:
            self.image += struct.pack("<II", rel["index"], rel["offset"])

        self.image += exported_symbol_table 
        self.image += external_symbol_table

        self.image = Application.align_bytes(self.image, 16)

        self.image += self.code 
        self.image += self.data 

        if not self.args.dryrun:
          self.logger.info("Writing to: " + self.args.output)
          with open(self.args.output, "wb") as file:
              file.write(self.image)

    def fill_code_and_data_arrays(self):
        self.code = bytearray(self.text_section["data"])
        self.data = bytearray(self.data_section["data"])
        self.bss = bytearray(self.bss_section["data"])
 

    def execute(self):
        self.fetch_sections()
        self.process_symbols()
        self.dump_processed_symbols()
        self.process_relocations()
        self.fill_code_and_data_arrays()
        self.process_data_relocations(len(self.code))
        self.dump_processed_relocations() 
        self.fix_offsets_in_code()
        self.build_symbol_table()
        self.build_image()

def main():
    init()
    args = parse_args()
    app = Application(args) 
    app.print_header()
    app.execute()

if __name__ == "__main__":
    main()

