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
Source: `Official JerryScript site <https://github.com/jerryscript-project/jerryscript/blob/master/docs/04.INTERNALS.md>`_.

To save space and be able to run JerryScript on an embedded device Sming compiles this library without a parser.
This means that the JavaScript files have to be compiled before landing on the device.
See the samples below to learn more.

Console messages
----------------

Two external functions are made available for scripts to use:

- ``print`` for normal console output (shown in AQUA)
- ``alert`` for messages in debug mode (show in RED)


VM integrity
------------

The Jerryscript VM may encounter situations where it cannot continue execution, such as memory allocation failure
or invalid bytecode.

Fatal errors cause engine execution to halt. It must be "re-booted" (in a virtual sense).
This requires application co-operation as scripts may need to be re-loaded, external functions re-registered, etc.

If intending to run unverified javascript it is especially important to guard all
calls into Jerryscript using the library exception handler:

.. code-block:: c++

   JS_TRY()
   {
      // Call into jerryscript engine
   }
   JS_CATCH()
   {
      // Engine has failed
   }

JS_TRY:
   Place code here which sets up jerryscript values, etc. then calls into Jerryscript.
   Data is allocated on local stack or in jerryscript engine.

   DO NOT perform any system heap allocation here, such as creating or modifying `String`) objects.
   If required, instantiate those above.

JS_CATCH:
   Jerryscript ust be re-initialised before any further calls.

   - De-allocate any globally held jerryscript values
   - Call JS::cleanup()
   - Call JS::init() and any other necessary initialisation

   It is recommended to provide a single function for both
   initial jerryscript initialisation and re-initialisation.

See the ``Event_Jsvm`` sample for a demonstratation.

Note that the VM uses own static heap (sized by :envvar:`JERRY_GLOBAL_HEAP_SIZE`) so main system heap is unaffected.

When :envvar:`JERRY_ENABLE_DEBUG` is enabled it may not be possible to recover because VM objects may be left with
invalid reference counts, for example, which will cause :cpp:func:`Jerryscript::cleanup` to fail.
Applications should generally be built with this setting disabled (the default).


Watchdog
--------

Sming is a single-threaded framework so requires co-operation to perform :doc:`multi-tasking </information/multitasking>`.
This means javascript calls should be brief so as not to disrupt the system.

If a javascript call takes too long then the system watchdog will timeout and reset the system.
This is generally not helpful, so a separate watchdog is implemented for the Jerryscript virtual machine.

By default, the watchdog is disabled. To enable it, add a call during initialisation:

.. code-block:: c++

   // Enable the watchdog with a 100ms timeout (0 to disable it)
   JS::Watchdog::setPeriod(100);

Now, if a call takes more than 100ms then a fatal error will be thrown as discussed above.

The setting can be changed at any time and is unaffected by jerryscript engine resets.


External Contexts
-----------------

By default, a single, global VM context is created in which all code is loaded and executed.
If multiple javascript applications are running then if one fails it will take out all the others.

Enabling the :envvar:`JERRY_EXTERNAL_CONTEXT` setting allows use of the :cpp:class:`JS::Context`
class to run multiple isolated instances. See the :sample:`Basic_Context` example for details.


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


.. envvar:: JERRY_PARSER

   default: 0 (disabled)

   Enable to build library with javascript parser enabled.
   Required for use of :cpp:func:`Jerryscript::eval` function.


.. envvar:: JERRY_EXTERNAL_CONTEXT

   default: 0 (disabled)

   Enable this setting to make use of :cpp:class:`JS::Context` to run multiple isolated instances.


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

.. toctree::
   :glob:

   *
