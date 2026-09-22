#!/usr/bin/env python3
"""Exercise installer decisions with a fake Flatpak, without touching user state."""
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
COMMIT = '16573ca27c3e72932f9d7190c3686711c5d489d8c63f80658e6a74267e77e95e'

class InstallerTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.work = Path(self.temp.name)
        payload = self.work / 'bundle with spaces.flatpak'
        payload.touch()
        self.script = self.work / 'installer'
        # Relocate only the packaged constant, just as an extracted deb fixture.
        text = (ROOT / 'packaging/papers-inplace-save').read_text()
        text = text.replace('bundle=/usr/share/papers-inplace-save/papers-fixed.flatpak',
                            'bundle=' + "'" + str(payload) + "'")
        self.script.write_text(text)
        self.script.chmod(0o755)
        (self.work / 'id').write_text('#!/bin/sh\nprintf "%s\\n" "$TEST_UID"\n')
        (self.work / 'id').chmod(0o755)
        fake = self.work / 'flatpak'
        fake.write_text('''#!/usr/bin/python3
import json, os, sys
with open(os.environ['TEST_LOG'], 'a') as log: log.write(json.dumps(sys.argv[1:])+'\\n')
if sys.argv[1]=='info':
 print(os.environ.get('TEST_COMMIT',''))
 sys.exit(0 if os.environ.get('TEST_COMMIT') else 1)
if sys.argv[1]=='install': sys.exit(int(os.environ.get('TEST_INSTALL_EXIT','0')))
''')
        fake.chmod(0o755)
        self.env = dict(os.environ, PATH=str(self.work)+':'+os.environ['PATH'],
                        TEST_UID='1000', TEST_LOG=str(self.work/'calls'))

    def run_installer(self, *args):
        result = subprocess.run([str(self.script), *args], env=self.env,
                                capture_output=True, text=True)
        log = self.work / 'calls'
        calls = [json.loads(line) for line in log.read_text().splitlines()] if log.exists() else []
        return result, calls

    def test_root_never_touches_flatpak(self):
        self.env['TEST_UID']='0'
        result,calls=self.run_installer('--install')
        self.assertNotEqual(result.returncode,0)
        self.assertEqual(calls,[])

    def test_current_commit_does_not_reinstall(self):
        self.env['TEST_COMMIT']=COMMIT
        result,calls=self.run_installer('--install')
        self.assertEqual(result.returncode,0)
        self.assertEqual([c[0] for c in calls],['info'])

    def test_new_install_is_user_scoped_and_no_data_deletion(self):
        result,calls=self.run_installer('--install')
        self.assertEqual(result.returncode,0)
        self.assertEqual(calls[1][:-1],['install','--user','--noninteractive','--or-update','--no-related'])
        self.assertTrue(calls[1][-1].endswith('bundle with spaces.flatpak'))
        self.assertNotIn('--delete-data',sum(calls,[]))

    def test_failed_install_does_not_launch(self):
        self.env['TEST_INSTALL_EXIT']='7'
        result,calls=self.run_installer('a file.pdf')
        self.assertEqual(result.returncode,7)
        self.assertEqual([c[0] for c in calls],['info','install'])

    def test_launch_preserves_arguments(self):
        self.env['TEST_COMMIT']=COMMIT
        result,calls=self.run_installer('a file.pdf','file:///tmp/中文.pdf')
        self.assertEqual(result.returncode,0)
        self.assertEqual(calls[-1],['run','--user','org.gnome.Papers//inplace-save','a file.pdf','file:///tmp/中文.pdf'])

if __name__ == '__main__': unittest.main()
