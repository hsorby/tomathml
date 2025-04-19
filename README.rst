
To MathML
=========

A simple C++ library to convert mathematical expressions written in text format into content MathML.

This library expoects mathematical expressions of the form::

  a = b + 2;

or for an ODE::

  ode(x, t) = 5;

The semi-colon (;) is used to mark the end of an expression.
Mulitple expressions can be added to a single text block::

  a = 6;b = e -3;
