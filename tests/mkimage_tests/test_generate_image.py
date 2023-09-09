#!/usr/bin/python3

import sys
from pathlib import Path

scripts_path = Path(__file__).parent.parent.parent / "mkimage"
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
            "input": "executable_example.elf",
            "log": None,
        }

        args = SimpleNamespace(**args)
        app = Application(args)
        app.execute()
        # self.assertTrue(app.is_executable)

        # # Taken from readelf -S
        self.assertEqual(app.text_section["address"], 0)
        self.assertEqual(app.text_section["size"], 0x6D98)
        self.assertEqual(app.data_section["address"], 0x6D98)
        self.assertEqual(app.data_section["size"], 0x1D0)
        self.assertEqual(app.bss_section["address"], 0x6D98 + 0x1D0)
        self.assertEqual(app.bss_section["size"], 0xD58)

        # main symbol export
        self.assertEqual(app.symbols["main"]["localization"], "exported")

        # Verify some local relocations

        errno = None
        impure_ptr = None
        for rel in app.relocations.get_relocations("local"):
            if rel["name"] == "errno":
                errno = rel
            if rel["name"] == "_impure_ptr":
                impure_ptr = rel

        self.assertIsNotNone(errno)
        self.assertIsNotNone(impure_ptr)

        # # verify skipped relocations

        kill = None
        ascii_wctomb = None
        for rel in app.relocations.get_relocations("local"):
            if rel["name"] == "_kill":  # R_ARM_THM_CALL
                kill = rel
            if rel["name"] == "__ascii_wctomb":  # R_ARM_ABS32
                ascii_wctomb = rel

        self.assertIsNone(kill)
        self.assertIsNone(ascii_wctomb)

        ctype = None
        text = None
        for rel in app.relocations.get_relocations("data"):
            if rel["name"] == "_ctype_":
                ctype = rel
            if rel["name"] == ".text":
                text = rel

        self.assertIsNotNone(ctype)
        self.assertIsNotNone(text)

        data = struct.unpack_from("<I", app.text, 0xB8)[0]
        self.assertEqual(data, 0x0)
        data = struct.unpack_from("<I", app.text, 0x5080)[0]
        self.assertEqual(data, 0x368)
        data = struct.unpack_from("<I", app.text, 0x3B50)[0]
        self.assertEqual(data, 0x278)

        self.assertEqual(len(app.exported_symbol_table), 1)
        self.assertEqual(len(app.external_symbol_table), 0)

        self.assertEqual(app.exported_symbol_table[0]["section"], SectionCode.Code)
        self.assertEqual(app.exported_symbol_table[0]["value"], 0x369)
        self.assertEqual(app.exported_symbol_table[0]["name"], "main")

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
        exported_rels = struct.unpack_from("<H", app.image, 34)[0]
        self.assertEqual(exported_rels, 0)

        exported_symbols = struct.unpack_from("<H", app.image, 36)[0]
        self.assertEqual(exported_symbols, 1)
        external_symbols = struct.unpack_from("<H", app.image, 38)[0]
        self.assertEqual(external_symbols, 0)


if __name__ == "__main__":
    unittest.main()
