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

def test_prefixes():

    assert prefixes() != ""
    assert prefixes(True) != ""
    
def test_base_units():
    
    assert base_units() != ""
    assert base_units(True) != ""
    
def test_derived_units():
    
    assert derived_units() != ""
    assert derived_units(True) != ""
    
def test_logarithmic_units():
    
    assert logarithmic_units() != ""
    assert logarithmic_units(True) != ""
    
def test_temperature_units():
    
    assert temperature_units() != ""
    assert temperature_units(True) != ""
    
def test_constants():
    
    assert constants() != ""
    assert constants(True) != ""
    
def test_quantities():
    
    assert quantities() != ""
    assert quantities(True) != ""
    
def test_systems():

    assert unit_systems() == ""
    assert unit_systems(True) != ""
