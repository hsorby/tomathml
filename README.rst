
To MathML
=========

ToMathML is a simple C++ library that converts mathematical expressions written in text format into content MathML.
There is a Python wrapper for the library that can be installed with pip::

  pip install tomathml

The library only has one function, and that function is *process*.
The function takes one parameter, a string, and returns a string.
The function also has an optional parameter to switch the CellML generation mode on and off.
The CellML generation mode is on by default.

This library expects mathematical expressions of the form::

  a = b + 2;

or for an ODE::

  ode(x, t) = 5;

The semi-colon (;) is used to mark the end of an expression.

By default, the library outputs content MathML suitable for `CellML <cellml.org>`_.
The CellML mode adds a requirement that all constants described in an equation must have some units defined.
The units should be the correct units appropriate for the equation.
To correctly generate content MathML from the examples above in CellML mode (the default), the units of the constants must be set.
The following examples show how units are set on constants::

  a = b + 2{kg};

simimlarly for the ODE::

  ode(x, t) = 5{m_per_s}

CellML mode can be turned off by setting the second parameter to the *process* function to false.
When the CellML mode is inactive, the constants' dimensions do not need to be set.
Mulitple expressions can be added to a single text block::

  a = 6;b = e -3;

Some examples of using the 'tomathml' library with Python to create context MathML output::

  >>> import tomathml
  >>> print(tomathml.process("a=b;"))
  <?xml version="1.0" encoding="UTF-8"?>
  <math xmlns="http://www.w3.org/1998/Math/MathML">
    <apply>
      <eq />
      <ci>
        a
      </ci>
      <ci>
        b
      </ci>
    </apply>
  </math>

  >>> print(tomathml.process("a=b+2{kg};"))
  <?xml version="1.0" encoding="UTF-8"?>
  <math xmlns="http://www.w3.org/1998/Math/MathML">
    <apply>
      <eq />
      <ci>
        a
      </ci>
      <apply>
        <plus />
        <ci>
          b
        </ci>
        <cn cellml:units="kg" xmlns:cellml="http://www.cellml.org/cellml/2.0#">
          2
        </cn>
      </apply>
    </apply>
  </math>

  >>> print(tomathml.process("a=b+2;", False))
  <?xml version="1.0" encoding="UTF-8"?>
  <math xmlns="http://www.w3.org/1998/Math/MathML">
    <apply>
      <eq />
      <ci>
        a
      </ci>
      <apply>
        <plus />
        <ci>
          b
        </ci>
        <cn>
          2
        </cn>
      </apply>
    </apply>
  </math>

  >>> print(tomathml.process("ode(x,t,2)=m*x;", False))
  <?xml version="1.0" encoding="UTF-8"?>
  <math xmlns="http://www.w3.org/1998/Math/MathML">
    <apply>
      <eq />
      <apply>
        <diff />
        <bvar>
          <ci>
            t
          </ci>
          <degree>
            <cn>
              2
            </cn>
          </degree>
        </bvar>
        <ci>
          x
        </ci>
      </apply>
      <apply>
        <times />
        <ci>
          m
        </ci>
        <ci>
          x
        </ci>
      </apply>
    </apply>
  </math>

  >>> print(tomathml.process("ode(x,t,2{dimensionless})=m*x;"))
  <?xml version="1.0" encoding="UTF-8"?>
  <math xmlns="http://www.w3.org/1998/Math/MathML">
    <apply>
      <eq />
      <apply>
        <diff />
        <bvar>
          <ci>
            t
          </ci>
          <degree>
            <cn cellml:units="dimensionless" xmlns:cellml="http://www.cellml.org/cellml/2.0#">
              2
            </cn>
          </degree>
        </bvar>
        <ci>
          x
        </ci>
      </apply>
      <apply>
        <times />
        <ci>
          m
        </ci>
        <ci>
          x
        </ci>
      </apply>
    </apply>
  </math>

  >>>


When in CellML mode and a constant in a mathematical equation does not have a dimension assigned, the *process* function will return an error message like the following::

  >>> print(tomathml.process("ode(x,t,2)=m*x;"))
  Messages from parser (1)
  [1, 10]: '{' is expected, but ')' was found instead.

This error message tells us that on line 1, column 10, the '{' character was expected, but ')' was found instead.
It expects the '{' character because that is the marker for defining units.

Building
--------

The following actions are required to build the library and Python bindings.

The main library is a C++ library, and to build this library, you will need CMake with a version of at least 3.25 and a compiler that supports the C++20 standard.
To build only the C++ library, follow these commands::

  git clone https://github.com/hsorby/tomathml
  cmake -S tomathml -B build-tomathml
  cd build-tomathml
  cmake --build .

If you are building with a tool that supports multiple configurations, the last line must be changed to::

  cmake --build . --config Release

The text *Release* in this command can be replaced with the name of the configuration desired.

If you wish to build the Python bindings, you will need, in addition to the above requirements, the `Doxygen <https://www.doxygen.nl/download.html>`_ application and a version of Python (as a virtual environment) with *nanobind* installed.
CMake must be able to find the *Doxygen* application and *nanobind*.
If CMake cannot find these tools, the bindings will not be able to be built.
To install *Doxygen*, follow the platform-specific instructions that match your operating system.
To make *nanobind* available, create a Python virtual environment, install *nanobind* and activate the virtual environment.
For a Linux or macOS operating system, this can be done from the terminal with the following commands::

  python -m venv venv_nanobind
  source venv_nanobind/bin/activate
  pip install nanobind

For the Windows operating system, this can be done from the *cmd* application with the following::

  python -m venv venv_nanobind
  ./venv_nanobind/Scripts/Activate
  pip install nanobind

When the library is configured and *Doxygen* and *nanobind* are found, the bindings will be built at the same time as the library.
Following the instructions on building above, will create the binding library for Python.

Installing
----------

The C++ library cannot be installed at this time.
However, the Python bindings can be.
To install the Python bindings, perform the following commands::

  cd build-tomathml
  pip install .

Ensure an active Python virtual environment that matches the Python version the library was built for is currently available.
With newer Python versions, 3.12 and beyond, the stable API means you can install the bindings from older Python environments into newer Python environments.

Testing
-------

The C++ library has some tests that can be run with *ctest*.
To run the tests after building the library, perform the following commands::

  cd build-tomathml
  ctest
