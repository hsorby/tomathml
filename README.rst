
To MathML
=========

A simple C++ library to convert mathematical expressions written in text format into content MathML.
There is a Python wrapper for the library that can be installed with pip::

  pip install tomathml

This library expoects mathematical expressions of the form::

  a = b + 2;

or for an ODE::

  ode(x, t) = 5;

The semi-colon (;) is used to mark the end of an expression.
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


If you are in CellML mode and you forget to assign a dimension to a constant then you can expect an error message like the following::

  >>> print(tomathml.process("ode(x,t,2)=m*x;"))
  Messages from parser (1)
  [1, 10]: '{' is expected, but ')' was found instead.

This error messages tells us the on line 1, column 10 the '{' character was expected but ')' was found instead.
It expecets the '{' character because it starts the definition of units.
