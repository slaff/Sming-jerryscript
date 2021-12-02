Basic_Jsvm
==========

.. highlight:: javascript

This sample demonstrates running JavaScript applications in a sandbox inside a microcontroller.
The sample uses the :component:`jerryscript` library for Sming.

To save space and be able to run JerryScript on an embedded device Sming compiles this library without a parser.
This means that the JavaScript files have to be compiled before landing on the device.
This sample demonstrates how this is done automatically for you.

Snapshots
---------

The initial JavaScript code used in this sample is located in ``files/main.js``. 
The code has one global variable ``x`` and two functions ``setup`` and ``loop``.
As shown below::

	var x;

	/**
	 * This is a function that will be called once.
	 */
	function setup() {
		x = 0;
		print ('Setup: X='+x);
	}
	
	/**
	 * This is a function that will be called every half a second
	 */
	function loop() {
		x++;
		print('Loop: X='+x);
	}


Once compiled the source code will be converted into a byte-code that will be executed inside of your microcontroller.
In JerryScript this final byte-code is called ``snapshot`` or just ``snap``. 

Calls
-----

This sample will run first the ``setup`` JavaScript function and then execute in an endless loop the ``loop`` function.
