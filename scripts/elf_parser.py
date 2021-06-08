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

from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection
from elftools.elf.relocation import RelocationSection
from elftools.elf.descriptions import describe_reloc_type

class ElfParser:
    def __init__(self, elf_filename):
        self.filename = elf_filename

    def get_symbols(self):
        symbols = {}
        with open(self.filename, "rb") as elf_file:
            elf = ELFFile(elf_file)
            for section in elf.iter_sections():
                if not isinstance(section, SymbolTableSection):
                    continue
                for symbol in section.iter_symbols():
                    data = {}
                    data["type"] = symbol["st_info"]["type"]
                    data["binding"] = symbol["st_info"]["bind"]
                    data["name"] = symbol["st_name"]
                    data["value"] = symbol["st_value"]
                    data["size"] = symbol["st_size"]
                    data["section_index"] = symbol["st_shndx"]
                    data["visibility"] = symbol["st_other"]["visibility"]
                    symbols[symbol.name] = data
        return symbols

    def get_sections(self):
        sections = {}
        with open(self.filename, "rb") as elf_file:
            elf = ELFFile(elf_file)
            for i, section in enumerate(elf.iter_sections()):
                section_data = {}
                section_data["name"] = section.name
                section_data["address"] = section["sh_addr"]
                section_data["type"] = section["sh_type"]
                section_data["flags"] = section["sh_flags"]
                section_data["offset"] = section["sh_offset"]
                section_data["size"] = section["sh_size"]
                section_data["link"] = section["sh_link"]
                section_data["info"] = section["sh_info"]
                section_data["addralign"] = section["sh_addralign"]
                section_data["entsize"] = section["sh_entsize"]
                section_data["data"] = section.data()
                section_data["index"] = i
                sections[section.name] = section_data

        return sections

    def get_relocations(self):
        relocations = []
        with open(self.filename, "rb") as elf_file:
            elf = ELFFile(elf_file)
            for section in elf.iter_sections():
                if not isinstance(section, RelocationSection):
                    continue
                symbols = elf.get_section(section["sh_link"])
                for relocation in section.iter_relocations():
                    if relocation["r_info_sym"] == 0:
                        continue
                    relocation_data = {}
                    relocation_data["offset"] = relocation["r_offset"]
                    relocation_data["info"] = relocation["r_info"]
                    relocation_data["info_type"] = describe_reloc_type(relocation["r_info_type"], elf)
                    relocation_data["symbol"] = relocation["r_info_sym"]
                    symbol = symbols.get_symbol(relocation_data["symbol"])
                    if symbol["st_name"] == 0:
                        symbol_section = elf.get_section(symbol["st_shndx"])
                        relocation_data["symbol_name"] = symbol_section.name
                    else:
                        relocation_data["symbol_name"] = symbol.name

                    relocation_data["symbol_value"] = symbol["st_value"]
                    relocation_data["section_index"] = symbol["st_shndx"]
                    relocations.append(relocation_data)
        return relocations

    def get_public_functions(self):
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
        symbols = get_symbols(self)
        symbol_names = []
        for key in symbols:
            symbol_names.append(key)
        return symbol_names


