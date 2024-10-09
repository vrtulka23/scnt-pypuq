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

def test_prefixes(capfd):

    prefixes()    
    captured = capfd.readouterr()
    assert captured.out != ""
    
def test_base_units(capfd):
    
    base_units()  
    captured = capfd.readouterr()
    assert captured.out != ""
    
def test_derived_units(capfd):
    
    derived_units()
    captured = capfd.readouterr()
    assert captured.out != ""
    
def test_logarithmic_units(capfd):
    
    logarithmic_units()
    captured = capfd.readouterr()
    assert captured.out != ""
    
def test_temperature_units(capfd):
    
    temperature_units()
    captured = capfd.readouterr()
    assert captured.out != ""
    
def test_constants(capfd):
    
    constants()   
    captured = capfd.readouterr()
    assert captured.out != ""
    
def test_quantities(capfd):
    
    quantities()  
    captured = capfd.readouterr()
    assert captured.out != ""
    
def test_systems(capfd):
    
    unit_systems()
    captured = capfd.readouterr()
    assert captured.out != ""
