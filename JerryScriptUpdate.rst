Updating JerryScript
====================

Currently we use Jerryscript version v2.4 from `JerryScript <https://github.com/jerryscript-project/jerryscript>`__.

The code is imported as a submodule and patched to support Web-based javascript compilation (see https://emscripten.org/).

Update
------

In order to update the version of JerryScript two tasks have to be executed:

- In this repository update the submodule version of JerryScript to point to the new version
- Update patch files as necessary

Once that is done we need to build completely from scratch a new docker container.

Dockerfile
----------

Up to now we have good results with creating a pure JavaScript code using the latest sdk.
There is a docker container that creates the needed environment.
It can be built and run with the following commands::

    docker build -f Dockerfile -t jerryscript-ems .
    docker run -it jerryscript-ems

Inside the container a new jerryscript snapshot compiler can be built by calling::

    node /jerryscript/build/bin/jsc.js -o /tmp/test.js.snap /tmp/test.js

Where ``test.js.snap`` is the output file and ``/tmp/test.js`` is an existing JavaScript file.

To update the already precompiled jsc.js in ``web/dev`` you can do the following::

    docker run -v $(pwd)/web/:/web -it jerryscript-ems

And run the following commands inside the container::

    cp -r /jerryscript/build/bin/jsc.* /web/dev

That will require also precompilation of the main.js file.
Inside the container this can be done with the following command::

    node /jerryscript/build/bin/jsc.js -o /web/build/main.js.snap /web/dev/main.js

