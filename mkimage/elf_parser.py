#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# elf_parser.py
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

from elftools.elf.elffile import ELFFile 
from elftools.elf.sections import SymbolTableSection
from elftools.elf.relocation import RelocationSection
from elftools.elf.descriptions import describe_reloc_type 


class ElfParser:
    """
    A class to parse ELF file to internal data structures

    ...

    Attributes
    ----------
    filename : str 
        Path to ELF file to be processed
    """

    def __init__(self, filename):
        """
        Parameters
        ----------
        filename : str
            Path to ELF file
        """ 

        self.filename = filename 
        self.symbols = self._parse_symbols() 
        self.sections = self._parse_sections()
        self.relocations = self._parse_relocations() 
        with open(self.filename, "rb") as file:
            self.elf = ELFFile(file)
 
    def get_symbols(self):
        return self.symbols

    def get_sections(self):
        return self.sections 

    def get_relocations(self):
        return self.relocations

    def get_section(self, section_name):
        if not self.sections:
            return None 

        if not section_name in self.sections:
            return None 

        return self.sections[section_name]

    def _parse_symbols(self):
        """
        Parse ELF file to create symbols table. 

        Returns 
        -------
        dict
            Dictionary of symbols with symbol names as keys
        """
        symbols = {} 
        with open(self.filename, "rb") as file:
            elf = ELFFile(file)
            for section in elf.iter_sections():
                if not isinstance(section, SymbolTableSection):
                    continue
                for symbol in section.iter_symbols():
                    type = symbol["st_info"]["type"]
                    if type == "STT_SECTION":
                        section_index = symbol["st_shndx"]
                        name = elf.get_section(section_index).name
                    else: 
                        name = symbol.name
                    data = {}
                    data["type"] = symbol["st_info"]["type"]
                    data["binding"] = symbol["st_info"]["bind"]
                    data["name"] = name
                    data["value"] = symbol["st_value"]
                    data["size"] = symbol["st_size"]
                    data["section_index"] = symbol["st_shndx"]
                    data["visibility"] = symbol["st_other"]["visibility"]
                    symbols[name] = data
        return symbols

    def _parse_sections(self):
        """
        Parse ELF file to create sections table 

        Returns 
        -------
        dict 
            Dictionary of sections with section names as keys
        """
        
        sections = {}
        with open(self.filename, "rb") as file:
            elf = ELFFile(file)
            for i, section in enumerate(elf.iter_sections()):
                data = {}
                data["name"] = section.name
                data["address"] = section["sh_addr"]
                data["type"] = section["sh_type"]
                data["flags"] = section["sh_flags"]
                data["offset"] = section["sh_offset"]
                data["size"] = section["sh_size"]
                data["link"] = section["sh_link"]
                data["info"] = section["sh_info"]
                data["addralign"] = section["sh_addralign"]
                data["entsize"] = section["sh_entsize"]
                data["data"] = section.data()
                data["index"] = i
                sections[section.name] = data
        return sections 
    def _parse_relocations(self):
        """
        Parse ELF file to create relocations table 

        Returns 
        -------
        list 
            Array of relocations 
        """

        relocations = []
        with open(self.filename, "rb") as file:
            elf = ELFFile(file)
            for section in elf.iter_sections():
                if not isinstance(section, RelocationSection):
                    continue 
                symbols = elf.get_section(section["sh_link"])
                for relocation in section.iter_relocations():
                    if relocation["r_info_sym"] == 0:
                        continue 
                    data = {}
                    data["offset"] = relocation["r_offset"]
                    data["info"] = relocation["r_info"]
                    data["info_type"] = describe_reloc_type(relocation["r_info_type"], elf)
                    data["symbol"] = relocation["r_info_sym"]
                    symbol = symbols.get_symbol(data["symbol"])
                    if symbol["st_name"] == 0:
                        symbol_section = elf.get_section(symbol["st_shndx"])
                        data["symbol_name"] = symbol_section.name
                    else:
                        data["symbol_name"] = symbol.name 

                    data["symbol_value"] = symbol["st_value"]
                    data["section_index"] = symbol["st_shndx"]
                    relocations.append(data)
        return relocations

    def _parse_public_functions(self):
        """
        Parse ELF file to create public symbols table

        Returns
        -------
        dict 
            Dictionary with public symbols
        """ 

        symbols = get_symbols(self)
        filtered_symbols = {}
        for symbol_key in symbols:
            symbol = symbols[symbol_key]
            if symbol["type"] == "STT_FUNC":
                if symbol["visibility"] != "STV_HIDDEN" and (symbol["binding"] == "STB_GLOBAL" or symbol["binding"] == "STB_WEAK"):
                    filtered_symbols[symbol_key] = symbol 
                if symbol_key == "main" and (symbol["binding"] == "STB_GLOBAL" or symbol["binding"] == "STB_WEAK"):
                    filtered_symbols[symbol_key] = symbol

        return filtered_symbols 

    def get_symbol_names(self):
        """
        Parse ELF file to create symbols list 

        Returns
        -------
        list 
            Array of symbols existing in ELF file
        """
        
        symbol_names = []
        for key in self.symbols:
            symbol_names.append(key)
        return symbol_names
    
    def get_file_type(self):
        return self.elf.header["e_type"]
