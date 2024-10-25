# SciNumTools v3: Physical units and quantities for Python (pypuq)

This module is a Python binding of the [C++ library `scnt-puq`](https://github.com/vrtulka23/scnt-puq).
You can read more about this module and its functionality in the `SciNumTool` documentation.

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

More examples and explanation are available in the documentation.
You can also look at the test problems in the [pytest directory](https://github.com/vrtulka23/scnt-pypuq/tree/main/pytest).
