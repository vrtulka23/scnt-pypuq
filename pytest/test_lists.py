import pytest
import importlib.util
import sys
import os
import glob
import numpy as np

libs = glob.glob("build/pypuq*.so")
spec = importlib.util.spec_from_file_location("pypuq", libs[0])
pp = importlib.util.module_from_spec(spec)
sys.modules["pypuq"] = pp

from pypuq.lists import *

def test_lists():

    assert prefixes()          != ""
    assert base_units()        != ""
    assert derived_units()     != ""
    assert logarithmic_units() != ""
    assert temperature_units() != ""
    assert constants()         != ""
    assert quantities()        != ""
    assert unit_systems()      != ""
