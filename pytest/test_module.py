import pytest
import importlib.util
import sys
import os
import glob
import subprocess

libs = glob.glob("build/pypuq*.so")
spec = importlib.util.spec_from_file_location("pypuq", libs[0])
pp = importlib.util.module_from_spec(spec)
sys.modules["pypuq"] = pp

import pypuq

def test_version():

    p = subprocess.Popen("source settings.env;echo $CODE_VERSION", stdout=subprocess.PIPE, shell=True, executable='/bin/bash')
    assert pypuq.__version__ == p.stdout.readlines()[0].strip().decode("utf-8")
