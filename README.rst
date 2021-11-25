JerryScript
===========

.. highlight:: bash

A JavaScript Virtual Machine based on JerryScript.

Introduction
------------

This library allows running JavaScript in a sandbox on all architectures supported by Sming. 
The library uses JerryScript as JavaScript Virtual Machine (VM).

.. image:: https://github.com/jerryscript-project/jerryscript/raw/master/docs/img/engines_high_level_design.png

The diagram above shows the interactions between the major components of JerryScript: Parser and VM. 
Parser performs translation of input ECMAScript application into the byte-code. 
Prepared bytecode is executed by the Virtual Machine that performs interpretation.
Source:  `Official JerryScript site <https://github.com/jerryscript-project/jerryscript/blob/master/docs/04.INTERNALS.md>`_. 

To save space and be able to run JerryScript on an embedded device Sming compiles this library without a parser.
This means that the JavaScript files have to be compiled before landing on the device.
See the samples below to learn more.

Samples
-------

- :sample:`Basic_Sample` - demonstrates how to use JerryScript and run compiled JavaScript code on a micro-controller.
- :sample:`Advanced_Sample` - demonstrates how to modify and compile JavaScript code on the fly using only your browser and run that code on a micro-controller.  


JerryScript Version
-------------------

Currently we use version v2.2 from JerryScript. If you are interested in updating JerryScript read the :ref:`JerryScriptUpdate` document.


Configuration variables
-----------------------

.. envvar:: APP_JS_SOURCE_DIR

    default: undefined

    Snap files can be created during the build stage by setting this variable in your project's component.mk file::

        APP_JS_SOURCE_DIR := files

    All ``.js`` files will be compiled into ``.snap`` files and written to :envvar:`APP_JS_SNAPDIR`.


.. envvar:: APP_JS_SNAP_DIR

    default: ``out/jerryscript``

    Location to write generated .snap files.
    
    
Credits
-------

The initial work on the JerryScript library for Sming was done as part of the `U:Kit project <https://github.com/attachix/ukit>`_. 
 
