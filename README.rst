JerryScript
===========

.. highlight:: bash

A JavaScript Virtual Machine based on JerryScript.

Introduction
------------

This library allows running JavaScript in a sandbox on all architectures supported by Sming. 
The library uses JerryScript as JavaScript Virtual Machine (VM).

.. image:: jerryscript/docs/img/engines_high_level_design.png

The diagram above shows the interactions between the major components of JerryScript: Parser and VM. 
Parser performs translation of input ECMAScript application into the byte-code. 
Prepared bytecode is executed by the Virtual Machine that performs interpretation.
Source:  `Official JerryScript site <https://github.com/jerryscript-project/jerryscript/blob/master/docs/04.INTERNALS.md>`_. 

To save space and be able to run JerryScript on an embedded device Sming compiles this library without a parser.
This means that the JavaScript files have to be compiled before landing on the device.
See the samples below to learn more.

Version
-------

.. toctree::

   JerryScriptUpdate

Configuration variables
-----------------------

.. envvar:: JERRY_ENABLE_DEBUG

   default: 0 (disabled)

   Enable assertions and debug messages in jerryscript library.
   Should be left disabled unless debugging the jerryscript engine itself.


.. envvar:: JERRY_MEM_STATS

   default: 1 (enabled)

   Enable jerryscript heap memory tracking.


.. envvar:: JERRY_ERROR_MESSAGES

   default: 0 (disabled)

   Enable text messages for common errors.
   Consumes ~2K RAM on ESP8266.


.. envvar:: JERRY_COMPACT_PROFILE

   default: 1 (enabled)

   Compact (minimal profile) compilation profile makes the JerryScript library smaller.

   Set to 0 to use es.next profile.


.. envvar:: JERRY_GLOBAL_HEAP_SIZE

   default: 1 (in KB)

   Size of the jerryscript global heap in kilobytes. Increase as required.


.. envvar:: JERRY_SNAPSHOT_TOOL

   Read-only. Path to the snapshot compiler for use by applications.


.. envvar:: APP_JS_SOURCE_DIR

    default: undefined

    Snap files can be created during the build stage by setting this variable in your project's component.mk file::

        APP_JS_SOURCE_DIR := files

    All ``.js`` files will be compiled into ``.snap`` files and written to :envvar:`APP_JS_SNAPDIR`.


.. envvar:: APP_JS_SNAP_DIR

    default: ``out/jerryscript``

    Location to write generated .snap files.


.. envvar:: APP_JS_SNAP_UPDATED

    default: undefined

    Set this if action required when snaps are updated, for example to rebuild SPIFFS image or update application.


.. envvar:: JERRY_WEB_COMPILER

    (read-only)

    Location of pre-compiled web compiler. See ``Advanced-Jsvm`` sample for example usage.


Credits
-------

The initial work on the JerryScript library for Sming was done as part of the `U:Kit project <https://github.com/attachix/ukit>`_. 


API Documentation
-----------------

.. doxygennamespace:: Jerryscript
   :members:
