Advanced_Jsvm
=============

.. highligh: JavaScript

![JerryScript Engine Design](https://github.com/jerryscript-project/jerryscript/raw/master/docs/img/engines_high_level_design.png)

The diagram above shows the interactions between the major components of JerryScript: Parser and Virtual Machine (VM). 
Parser performs translation of input ECMAScript application into the byte-code. 
Prepared bytecode is executed by the Virtual Machine that performs interpretation.
`Source: <https://github.com/jerryscript-project/jerryscript/blob/master/docs/04.INTERNALS.md>`_. 

To save space and be able to run JerryScript on an embedded device Sming compiles this library without parser.
This means that the JavaScript files have to be compiled before landing on the device.
This can be done either using a binary or directly from the web browser.

Local compilation
----------------
For the first option you should set the ``APP_JS_SOURCE_DIR`` variable during compilation to point to a directory 
where all JavaScript files that have to be compiled will reside. Example::

	make APP_JS_SOURCE_DIR=snaps

In the example above all files in the snaps/ directory inside of your application will be compiled.
By default the compiled files will be stored in ``out/jerryscript`` directory inside of your application.
If you want to use another target directory this can be done suing the ``APP_JS_SNAP_DIR`` variable.
Example::
	
	make APP_JS_SOURCE_DIR=snaps APP_JS_SNAP_DIR=files/build

The is already implemented in the advanced sample.

Web Browser compilation
-----------------------
Quite often we don't have physical access to our devices. And here comes the true beauty of this sample.
With only network access to the device and a modern browser one can still compile JavaScript files 
and uploading them to the device. No external tools are required. 

The advanced sample starts a web server with a simple page.
In it you will see the initial source code of the ``main.js`` file.

There are two buttons in the HTML page. The first one starts and stops the JavaScript VM. 
If you press ``Run`` you will be able to see the result from the execution in the device terminal.
If you have your device connected to your computer then type the following to see the output::

	make terminal
		
If you want to change the JavaScript code and test it again on your device you have to press ``Stop``.
Do directly in the browser the desired changes and press ``Compile``. If the modifed JavaScript is valid
then it will be compiled directly in your browser and sent to your device. 
Again no external tools are required - just a modern browser. 
After that you can press ``Run`` in the browser and enjoy the modified result in the terminal.

Credits
=======

The initial work on the JerryScript library for Sming was done as part of the `U:Kit project <https://github.com/attachix/ukit>`_. 
 
  
