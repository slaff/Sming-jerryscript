Updating JerryScript
====================

highlight:: bash

Currently we use Jerryscript version v2.4 from `JerryScript <https://github.com/jerryscript-project/jerryscript>`__,
imported as a submodule.

The `web-assembly (WASM) <https://webassembly.org/>`__ snapshot compiler uses the `emscripten SDK <https://emscripten.org/>`__.

Update
------

In order to update the version of JerryScript two tasks have to be executed:

- In this repository update the submodule version of JerryScript to point to the new version
- Rebuild the snapshot compiler

The snapshot compiler can be rebuilt manually as follows:

1. Install the emscripten SDK::

    git clone --depth 1 https://github.com/emscripten-core/emsdk.git emsdk
    emsdk/emsdk install latest
    emsdk/emsdk activate latest

2. run ``make`` from the ``emscripten-snapshot-compiler`` directory.

The following three files are generated in ``emscripten-snapshot-compiler/build/bin/{PROFILE}``:

- jsc.js
- jsc.wasm
- jsc.wasm.map

Copy these to the ``jsc`` sub-directory for application use as demonstrated in the ``Advanced_Jsvm`` sample.


Dockerfile
----------

The snapshot compiler can be rebuilt using a Docker container which also includes a WebAssembly toolkit.
It can be built and run with the following commands::

    docker build -f Dockerfile -t jerryscript-ems .
    docker run -it jerryscript-ems

Inside the container a new jerryscript snapshot compiler can be built by calling::

    node /jerryscript/build/bin/minimal/jsc.js -o /tmp/test.js.snap /tmp/test.js

Where ``test.js.snap`` is the output file and ``/tmp/test.js`` is an existing JavaScript file.

To update the ``jsc.*`` files you run the following from the application directory::

    docker run -v $(pwd)/files/web/:/web -it jerryscript-ems

And run the following commands inside the container::

    cp -r /jerryscript/build/bin/minimal/jsc.* /web

The build system handles compilation of the ``main.js`` file,
but if you prefer to use pre-compiled snaps this can also be done inside the container::

    node /jerryscript/build/bin/minimal/jsc.js -o /web/main.js.snap /web/main.js
