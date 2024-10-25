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

import pypuq as pp
import scinumtools as snt
from pint import UnitRegistry

def arithmetics(Q):
    q = Q(23.23, 'km')
    for i in range(1,100):
        q += Q(float(i), 'cm')
        q -= Q(float(i), 'au')
    for i in range(1,5):
        q *= Q(float(i), 'h')
        q /= Q(float(i), 'mg')
    return q

def test_scalar_pypuq(benchmark):
    result = benchmark(arithmetics, Q=pp.Quantity)
    assert str(result) == "-7.40509e+11*km*h4*mg-4"

def test_scalar_scinumtools(benchmark):
    result = benchmark(arithmetics, Q=snt.units.Quantity)
    assert str(result) == "Quantity(-7.405e+11 km*h4*mg-4)"

def test_scalar_pint(benchmark):

    ureg = UnitRegistry()
    result = benchmark(arithmetics, Q=ureg.Quantity)

    assert str(result) == "-740509459941.7206 hour ** 4 * kilometer / milligram ** 4"

"""
def arithmetics_arrays(Q):
    arr = np.arange(1,200,10)
    q = Q(arr, 'km')
    for i in range(1,100):
        q += Q(arr*i, 'cm')
        q -= Q(arr*i, 'au')
    for i in range(1,5):
        q *= Q(arr*i, 'h')
        q /= Q(arr*i, 'mg')
    return q

def test_arrays_pypuq(benchmark):
    result = benchmark(arithmetics_arrays, Q=pp.Quantity)
    #assert str(result) == "-7.40509e+11*km*h4*mg-4"

def test_arrays_scinumtools(benchmark):
    result = benchmark(arithmetics_arrays, Q=snt.units.Quantity)
    #assert str(result) == "Quantity(-7.405e+11 km*h4*mg-4)"

def arithmetics_arrays_pint(Q):
    arr = np.arange(1,200,10)
    q = Q(arr, 'km')
    for i in range(1,100):
        q += Q(arr*i, 'cm')
        q -= Q(arr*i, 'au')
    for i in range(1,5):
        q *= Q(arr*i, 'h')
        q /= Q(arr*i, 'mg')
    return q
        
def test_benchmark_pint(benchmark):

    ureg = UnitRegistry()
    result = benchmark(arithmetics_arrays_pint, Q=ureg.Quantity)
"""
