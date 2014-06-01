#!/usr/bin/python

import os
import sys
import unittest
import time
import subprocess
import re
import shutil

class CGCOS(unittest.TestCase):
    def setUp(self):
        self._cwd = os.getcwd()
        dirname = os.path.dirname(__file__)
        if dirname:
            os.chdir(dirname)
        self.run_cmd(['make', 'clean'])
        self.run_cmd(['make'])

    def tearDown(self):
        os.chdir(self._cwd)

    def modify_byte(self, src, dst, offset, value):
        shutil.copyfile(os.path.join('files', src), os.path.join('files', dst))
        filp = open(os.path.join('files', dst), 'r+')
        filp.seek(offset)
        filp.write(chr(value))
        filp.close()

    def run_cmd(self, cmd):
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        return p.communicate()

    def run_verify(self, testfile):
        return self.run_cmd(['cgcef_verify', os.path.join('files', testfile)])

    def check_results(self, results, expected):
        expected_re = re.compile(expected)
        for line in results:
            if expected_re.search(line):
                return

        # KLUDGE alert. Just throw an error
        self.assertEqual(expected, results)

    def test_elf32(self):
        results = self.run_verify('elf32')
        self.assertEqual("Executable did not verify as CGC\n", results[0])
        self.check_results(results, "ident ELF")

    def test_elf64(self):
        results = self.run_verify('elf64')
        self.assertEqual("Executable did not verify as CGC\n", results[0])
        self.check_results(results, "bad e_shentsize")

    def test_good(self):
        results = self.run_verify('cgc-testcase')
        self.assertEqual("Executable verified as CGC\n", results[0])

        # We'll have to remove this soonish
        self.check_results(results, "PT_GNU_STACK program header was detected. These will be considered invalid prior to CQE.")

    def test_section_headers(self):
        results = self.run_verify('cgc-testcase')
        self.check_results(results, "CGC Executable contained optional section headers")
        self.assertEqual("Executable verified as CGC\n", results[0])

    def test_cgc_program_headers(self):
        results = self.run_verify('cgc-testcase-bad')
        self.check_results(results, "Invalid program header")

        # Program headers are just a warning, for now
        self.assertEqual("Executable verified as CGC\n", results[0])
        #self.assertEqual("Executable did not verify as CGC\n", results[0])

    def test_cgc64(self):
        # stomp CLASS64 identifier into EI_CLASS
        self.modify_byte('cgc-testcase', 'cgc-64bit', 4, 2)
        results = self.run_verify('cgc-64bit')
        self.assertEqual("Executable did not verify as CGC\n", results[0])

    def test_cgc_big_endian(self):
        # stomp DATA2MSB identifier into EI_DATA
        self.modify_byte('cgc-testcase', 'cgc-bigendian', 5, 2)
        results = self.run_verify('cgc-bigendian')
        self.assertEqual("Executable did not verify as CGC\n", results[0])

    def test_cgc_version(self):
        # stomp NONE identifier into EI_VERSION
        self.modify_byte('cgc-testcase', 'cgc-version0', 6, 0)
        results = self.run_verify('cgc-version0')
        self.assertEqual("Executable did not verify as CGC\n", results[0])
        self.check_results(results, "unknown CGCEF version")

        # stomp CURRENT+1 identifier into EI_VERSION
        self.modify_byte('cgc-testcase', 'cgc-version2', 6, 2)
        results = self.run_verify('cgc-version2')
        self.assertEqual("Executable did not verify as CGC\n", results[0])
        self.check_results(results, "unknown CGCEF version")

    def test_cgc_osabi(self):
        # stomp LINUX identifier into EI_OSABI
        self.modify_byte('cgc-testcase', 'cgc-osabi', 7, 3)
        results = self.run_verify('cgc-osabi')
        self.assertEqual("Executable did not verify as CGC\n", results[0])
        self.check_results(results, "did not identify as a CGC OS ABI binary")

    def test_cgc_osabi_version(self):
        # stomp low identifier into EI_OSABIVERSION
        self.modify_byte('cgc-testcase', 'cgc-osabiversion', 8, 0)
        results = self.run_verify('cgc-osabiversion')
        self.assertEqual("Executable did not verify as CGC\n", results[0])
        self.check_results(results, "did not identify as a v1 CGC OS ABI binary")

    def test_cgc_dynamic_so(self):
        # stomp ET_DYN into e_type
        self.modify_byte('cgc-testcase', 'cgc-library', 16, 3)
        results = self.run_verify('cgc-library')
        self.assertEqual("Executable did not verify as CGC\n", results[0])
        self.check_results(results, "did not identify as an executable")

    def test_cgc_non_x86(self):
        # stomp non-x86 into e_machine
        self.modify_byte('cgc-testcase', 'cgc-non-x86', 18, 4)
        results = self.run_verify('cgc-non-x86')
        self.assertEqual("Executable did not verify as CGC\n", results[0])
        self.check_results(results, "did not identify as i386")

    def test_cgc_e_version(self):
        # stomp 0 into e_version
        self.modify_byte('cgc-testcase', 'cgc-eversion', 20, 0)
        results = self.run_verify('cgc-eversion')
        self.assertEqual("Executable did not verify as CGC\n", results[0])
        self.check_results(results, "did not identify as a version 1 binary")

    def test_cgc_e_flags(self):
        # stomp 1 into e_flags
        self.modify_byte('cgc-testcase', 'cgc-eflags', 36, 1)
        results = self.run_verify('cgc-eflags')
        self.assertEqual("Executable did not verify as CGC\n", results[0])
        self.check_results(results, "contained unsupported flag")


        # XXXMIKE
        # section header(s)
        

#    def test_file2(self):
#        print "Testing 2"

if __name__ == '__main__':
    unittest.main()


# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4
