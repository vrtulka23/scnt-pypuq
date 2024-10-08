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

from pypuq import Q, UnitSystem
from pypuq.systems import US, ESU, EMU
from pypuq.formats import MKS, MGS, CGS, FPS

def test_init_scalars():

    # initialize with a number/string pair
    q = Q(23)    
    assert q.to_string() == "23"
    q = Q(23, 'km')    
    assert q.to_string() == "23*km"
    q = Q(23, 'ft', US)    
    assert q.to_string() == "23*ft"

    # initialize with a string
    q = Q('23*km')
    assert q.to_string() == "23*km"
    q = Q('23*ft', US)
    assert q.to_string() == "23*ft"

def test_init_arrays():

    # initialize with an list/string pair
    q = Q([2,3,4])    
    assert q.to_string() == "{2, 3, ...}"      # string representation
    assert q.size() == 3                       # size of the array
    q = Q([2.,3.,4.], 'km')    
    assert q.to_string() == "{2, 3, ...}*km"
    q = Q([2.,3.,4.], 'ft', US)    
    assert q.to_string() == "{2, 3, ...}*ft"

    # initialize with a numpy array
    q = Q(np.array([2,3,4]))
    assert q.to_string() == "{2, 3, ...}"
    
    # initialize with a string
    q = Q('{2,3,4}*ft', US)
    assert q.to_string() == "{2, 3, ...}*ft"
    
def test_init_arrays_errors():
    
    # initialize with an list/string pair
    q = Q([2.34,3.45,4.56],[0.1,0.2,0.3], 'km')    
    assert q.to_string() == "{2.34(10), 3.45(20), ...}*km"
    
def test_init_errors():

    q = Q(23.34, 0.2)
    assert q.to_string() == "2.334(20)e+01"
    q = Q(23.34, 0.2, 'km')
    assert q.to_string() == "2.334(20)e+01*km"
    q = Q(23.34, 0.2, 'ft', US)
    assert q.to_string() == "2.334(20)e+01*ft"
    
def test_string():

    q = Q("23*cm")
    assert str(q) == "23*cm"
    
def test_unit_system():

    q = Q('23*km')          # default unit system is used
    assert q.unit_system() == "SI"
    q = Q('US_23*ft')       # unit system is defined in the expression
    assert q.unit_system() == "US"
    q = Q('23*ft', US)      # explicitely defined unit system
    assert q.unit_system() == "US"

    with UnitSystem(EMU) as us:  # set unit system in a context manager
        assert Q('abA').unit_system() == "EMU"
    assert Q('km').unit_system() == "SI"
        
    us = UnitSystem(ESU)    # unit system environment is applied
    q = Q('statA')    
    assert q.unit_system() == "ESU"
    us.change(US)           # change of unit system environment
    q = Q('ft')    
    assert q.unit_system() == "US"
    us.close()              # unit system environment is closed
    q = Q('au')    
    assert q.unit_system() == "SI"
    
def test_conversion():

    # direct conversion
    q = Q(23, 'km')    
    assert q.to_string() == "23*km"
    q = q.convert('m')
    assert q.to_string() == "23000*m"
    q = q.convert('yd', US)
    assert q.unit_system() == "US"
    assert q.to_string() == "25153.1*yd"
    q = q.convert('cm', ESU, 'l')
    assert q.unit_system() == "ESU"
    assert q.to_string() == "2.3e+06*cm"

    # conversion with a quantity context
    q = Q(23, 'A')    
    q = q.convert('statA', ESU, 'I')
    assert q.unit_system() == "ESU"
    assert q.to_string() == "6.89523e+10*statA"

    q = Q(23, '<B>')    
    q = q.convert('<B>', EMU, 'B')
    assert q.unit_system() == "EMU"
    assert q.to_string() == "230000*<B>"

    # conversion to dimension formats
    q = Q(23, 'J')
    assert q.convert(MKS).to_string() == "23*m2*kg*s-2"
    assert q.convert(MGS).to_string() == "23000*m2*g*s-2"
    assert q.convert(CGS).to_string() == "2.3e+08*cm2*g*s-2"
    q = Q(23, 'yd2*s/oz', US)
    assert q.convert(FPS).to_string() == "3312*ft2*lb-1*s"

def test_arithmetics():

    n = Q(12)
    q = Q(21, 'km')    
    r = Q(3, 'm')    
    s = 2
    a = [2,3,4]

    # quantity/quantity
    assert (q+r).to_string() == "21.003*km"
    assert (q-r).to_string() == "20.997*km"
    assert (q*r).to_string() == "63*km*m"
    assert (q/r).to_string() == "7*km*m-1"

    # quantity/scalar
    assert (n+s).to_string() == "14"
    assert (s+n).to_string() == "14"
    assert (n-s).to_string() == "10"
    assert (s-n).to_string() == "-10"
    assert (q*s).to_string() == "42*km"
    assert (s*q).to_string() == "42*km"
    assert (q/s).to_string() == "10.5*km"
    assert (s/q).to_string() == "0.0952381*km-1"

    # quantity/array
    assert (n+a).to_string() == "{14, 15, ...}"
    assert (a+n).to_string() == "{14, 15, ...}"
    assert (n-a).to_string() == "{10, 9, ...}"
    assert (a-n).to_string() == "{-10, -9, ...}"
    assert (q*a).to_string() == "{42, 63, ...}*km"
    assert (a*q).to_string() == "{42, 63, ...}*km"
    assert (q/a).to_string() == "{10.5, 7, ...}*km"
    assert (a/q).to_string() == "{0.0952381, 0.142857, ...}*km-1"

    assert (Q('4*dm') * Q('3*m') / Q('6*s')).to_string() == "2*dm*m*s-1"

def test_rebase_units():

    q = Q("23*cm*m2*kg*mg")
    q = q.rebase_prefixes()
    assert q.to_string() == "0.23*kg2*cm3"

    q = Q("23*km*au2*s2/h")
    q = q.rebase_dimensions()
    assert q.to_string() == "1.4298e+14*km3*s"

def test_magnitude_output():

    # return scalar value
    q = Q("23.45(12)*km")
    assert q.value() == 23.45
    assert q.error() == 0.12

    # return list of values
    q = Q("{2.23(12),3.5,4.48(94),5.293}*km")
    np.testing.assert_almost_equal(q.value(), [2.23, 3.5, 4.48, 5.293])
    np.testing.assert_almost_equal(q.error(), [0.12, 0, 0.94, 0])

    # return numpy arrays
    q = Q("{2.23(12),3.5,4.48(94),5.293}*km")
    np.testing.assert_almost_equal(q.value(numpy=True), [2.23, 3.5, 4.48, 5.293])
    np.testing.assert_almost_equal(q.error(numpy=True), [0.12, 0, 0.94, 0])
