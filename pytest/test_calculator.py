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

from pypuq import Quantity, Calculator

def test_calculator():

    q = Calculator("34*km + 25*m")
    assert q.to_string() == "34.025*km"
    
    q = Calculator("29*kg * 2*mg")
    assert q.to_string() == "58*kg*mg"
