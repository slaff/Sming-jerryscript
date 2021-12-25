Basic Context
=============

.. highlight:: cpp

Jerryscript provides the optional External context feature to allow applications to provide an external buffer
or isolated engine contexts. The heap size is configured at runtime and enables loading of multiple javascript applications.

This sample demonstrates how to use the Jerryscript :cpp:class:`JS::Context` class to implement containerised applications.
