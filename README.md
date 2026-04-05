# SciNumTools v3 <br/>Physical units and quantities for Python (pypuq)

This module is a Python binding of the [C++ library `scnt-puq`](https://github.com/vrtulka23/scnt-puq).
You can read more about this module and its functionality in the `SciNumTool` [documentation](https://vrtulka23.github.io/scnt-docs/).

> [!NOTE]
> This project is currently archived and is further developed in a new joint [SciNumTools3](https://github.com/vrtulka23/scinumtools3) repository.

## Installation

To start, one has to install the [`scnt-puq`](https://github.com/vrtulka23/scnt-puq) library.
On macOS this can be easily done using homebrew formula:

``` bash
brew tap vrtulka23/scinumtools
brew install vrtulka23/scinumtools/scnt-puq
```

After that, one can proceed with the usual PyPi installation:

```bash
pip3 install pypuq
```

## Examples of use

As already mentioned, `pypuq` is a binding to the standalone C++ library `scnt-puq`.
Therefore, `pypuq` inherits all its usual functionality.
However, the Python interface of exposed classes (`Quantity`, `UnitSystem`) is slightly adapted.
```python
>>> from pypuq import Quantity
>>> 
>>> q = Quantity(16, 'm')
>>> r = Quantity(223, 's')
>>> speed = (q/r).convert('mph')
0.0717489*m*s-1
```

More examples and explanation are available in the `SciNumTools` [documentation](https://vrtulka23.github.io/scnt-docs/).
You can also look at the test problems in the [pytest directory](https://github.com/vrtulka23/scnt-pypuq/tree/main/pytest).

## Banchmarks

Here is a preliminary benchmark comparing the C++ implementation of PUQ `SciNumTools3` in this repository, the PINT units, and the original Python implementation of PUQ in `SciNumTools2`.

``` bash
pytest/test_module.py:18: AssertionError

------------------------------------------------------------------------------------ benchmark: 3 tests ------------------------------------------------------------------------------------
Name (time in ms)               Min                Max               Mean            StdDev             Median               IQR            Outliers       OPS            Rounds  Iterations
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
test_scalar_pypuq            7.5011 (1.0)       8.4626 (1.0)       7.6681 (1.0)      0.1696 (2.27)      7.6121 (1.0)      0.1595 (1.62)         12;8  130.4099 (1.0)         103           1
test_scalar_pint             9.6163 (1.28)     10.0365 (1.19)      9.7461 (1.27)     0.0746 (1.0)       9.7354 (1.28)     0.0986 (1.0)          24;2  102.6054 (0.79)        101           1
test_scalar_scinumtools     32.0431 (4.27)     32.6510 (3.86)     32.3082 (4.21)     0.1330 (1.78)     32.2975 (4.24)     0.1179 (1.20)          9;4   30.9519 (0.24)         28           1
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Legend:
  Outliers: 1 Standard Deviation from Mean; 1.5 IQR (InterQuartile Range) from 1st Quartile and 3rd Quartile.
  OPS: Operations Per Second, computed as 1 / Mean
```
