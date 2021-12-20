Advanced_Jsvm
=============

.. highligh:: bash

To save space and be able to run JerryScript on an embedded device, this library builds without parser support.

This means that the JavaScript files have to be compiled into 'snap' files before landing on the device.

This can be done during normal compilation and flashing, or indirectly via web application.

Manual compilation method
-------------------------

You should have your device connected physically to your computer via USB cable. 

In this case the compilation will be done on your computer and the compiled files will be saved to your device.

In order to configure the JavaScript file compilation one can use :envvar:`APP_JS_SOURCE_DIR` and :envvar:`APP_JS_SOURCE_DIR` variables.

All ``.js`` files in the ``APP_JS_SOURCE_DIR`` directory will be compiled into ``.snap`` files and written to ``APP_JS_SNAPDIR`` directory.
Example::

	make APP_JS_SOURCE_DIR=files/js APP_JS_SNAP_DIR=out/web

In this advanced sample the variables are directly stored in ``component.mk`` file.

The generated .snap files may be flashed or uploaded to your device for execution via any appropriate mechanism.

Web Browser compilation method
------------------------------

Quite often we don't have physical access to our devices. And here comes the true beauty of this sample.
With only network access to the device and a modern browser one can still compile JavaScript files 
and upload them to the device. No external tools are required. 

The advanced sample starts a web server with a simple page.
In it you will see the initial source code of the ``main.js`` file.

There are two buttons in the HTML page. The first one starts and stops the JavaScript VM. 
If you press ``Run`` you will be able to see the result from the execution in the device terminal.
If you have your device connected to your computer then type the following to see the output::

	make terminal
		
If you want to change the JavaScript code and test it again on your device you have to press ``Stop``.
Do directly in the browser the desired changes and press ``Compile``. If the modified JavaScript is valid
then it will be compiled directly in your browser and sent to your device. 
Again no external tools are required - just a modern browser. 
After that you can press ``Run`` in the browser and enjoy the modified result in the terminal.

Credits
-------

The initial work on the JerryScript library for Sming was done as part of the `U:Kit project <https://github.com/attachix/ukit>`_. 
