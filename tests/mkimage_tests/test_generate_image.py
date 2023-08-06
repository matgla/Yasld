#!/usr/bin/python3 

import sys 
from pathlib import Path 

scripts_path = Path(__file__).parent.parent.parent / "scripts"
sys.path.append(str(scripts_path.absolute()))

from types import SimpleNamespace

from mkimage import Application, SectionCode

import unittest 

import struct

class TestMkImage(unittest.TestCase):
  def test_validate_section(self):
    args = {
      "verbose": True,
      "quiet": False,
      "dryrun": True,
      "input": "executable_example.elf"
    }

    args = SimpleNamespace(**args)
    app = Application(args)
    app.fetch_sections()
    self.assertTrue(app.is_executable)
   
    # Taken from readelf -S 
    self.assertEqual(app.text_section["address"], 0)
    self.assertEqual(app.text_section["size"], 0x6d98)
    self.assertEqual(app.data_section["address"], 0x6d98)
    self.assertEqual(app.data_section["size"], 0x1d0)
    self.assertEqual(app.bss_section["address"], 0x6d98 + 0x1d0)
    self.assertEqual(app.bss_section["size"], 0xd58)

    app.process_symbols()
    # main symbol export
    self.assertEqual(app.processed_symbols["main"]["localization"], "exported")

    app.process_relocations()
    # Verify some local relocations 
    self.assertEqual(app.relocation_table.get_relocation("_impure_ptr")["type"], "local") 
    self.assertEqual(app.relocation_table.get_relocation("errno")["type"], "local") 

    # verify skipped relocations 
    self.assertIsNone(app.relocation_table.get_relocation("_kill")) # R_ARM_THM_CALL 
    self.assertIsNone(app.relocation_table.get_relocation("__ascii_wctomb")) # R_ARM_ABS32

    app.fill_code_and_data_arrays()
    app.process_data_relocations(app.text_section["size"])

    # verify data relocations 
    self.assertEqual(app.relocation_table.get_relocation("_ctype_")["type"], "data")
    self.assertEqual(app.relocation_table.get_relocation(".text")["type"], "data")

    # precheck 
    data = struct.unpack_from("<I", app.code, 0xb8)[0] 
    self.assertEqual(data, 0x18)
    data = struct.unpack_from("<I", app.code, 0x5080)[0] 
    self.assertEqual(data, 0x1e4)
    data = struct.unpack_from("<I", app.code, 0x3b50)[0] 
    self.assertEqual(data, 0x1a0)

    app.fix_offsets_in_code()
    
    data = struct.unpack_from("<I", app.code, 0xb8)[0] 
    self.assertEqual(data, 0x0)
    data = struct.unpack_from("<I", app.code, 0x5080)[0] 
    self.assertEqual(data, 0x368)
    data = struct.unpack_from("<I", app.code, 0x3b50)[0] 
    self.assertEqual(data, 0x278)

    app.build_symbol_table() 
    self.assertEqual(app.exported_symbol_table_size, 1)
    self.assertEqual(app.external_symbol_table_size, 0)

    self.assertEqual(app.exported_symbol_table[0]["section"], SectionCode.Code)
    self.assertEqual(app.exported_symbol_table[0]["value"], 0x369)
    self.assertEqual(app.exported_symbol_table[0]["name"], "main")

    app.build_image()
    data = struct.unpack_from("<4s", app.image, 0)[0]
    self.assertEqual(data, bytes("YAFF", "utf-8"))
    module_type = struct.unpack_from("<B", app.image, 4)[0]
    self.assertEqual(module_type, 1)
    arch_type = struct.unpack_from("<H", app.image, 5)[0]
    self.assertEqual(arch_type, 1)
    yasiff_version = struct.unpack_from("<B", app.image, 7)[0]
    self.assertEqual(yasiff_version, 1)

    code_length = struct.unpack_from("<I", app.image, 8)[0]
    self.assertEqual(code_length, app.text_section["size"])

    data_length = struct.unpack_from("<I", app.image, 12)[0]
    self.assertEqual(data_length, app.data_section["size"])

    bss_length = struct.unpack_from("<I", app.image, 16)[0]
    self.assertEqual(bss_length, app.bss_section["size"])

    external_libs = struct.unpack_from("<H", app.image, 20)[0]
    self.assertEqual(external_libs, 0)

    alignment = struct.unpack_from("<B", app.image, 22)[0]
    self.assertEqual(alignment, 4)

    _ = struct.unpack_from("<B", app.image, 23)[0]

    version_major = struct.unpack_from("<H", app.image, 24)[0]
    self.assertEqual(version_major, 0)

    version_minor = struct.unpack_from("<H", app.image, 26)[0]
    self.assertEqual(version_minor, 0)

    external_rels = struct.unpack_from("<H", app.image, 28)[0]
    self.assertEqual(external_rels, 0)
    local_rels = struct.unpack_from("<H", app.image, 30)[0]
    self.assertEqual(local_rels, 219)
    data_rels = struct.unpack_from("<H", app.image, 32)[0]
    self.assertEqual(data_rels, 18)
    
    exported_symbols = struct.unpack_from("<H", app.image, 34)[0]
    self.assertEqual(exported_symbols, 1)
    external_symbols = struct.unpack_from("<H", app.image, 36)[0]
    self.assertEqual(external_symbols, 0)

if __name__ == "__main__":
  unittest.main()
