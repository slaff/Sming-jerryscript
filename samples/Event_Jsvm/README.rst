Event_Jsvm
==========

.. highlight:: cpp

This sample demonstrates how to run your JavaScript application as performing some action
in response to an event, input or stimulus. 
Since Sming framework itself has an :doc:`event-driven architecture </information/events>`
such JavaScript applications will be able to use all the advantages that Sming has in terms of CPU, RAM and power usage.

JavaScript Code
---------------

Similar to Sming this sample calls the ``init`` JavaScript function. And in this function
we register the events that we are interested in. As shown below.

.. code:: javascript

	/**
	 * This is a function that will be called once.
	 */
	function init() {
		x = 0;
		print('Init: X=' + x);
	
		// This is how we register an event listener in JavaScript
		addEventListener("EVENT_TEMP", function(event) {
			// An event has a name and multiple params
			// the params have string keys and string values
			print('Event name: ' + event.name + ', value: '	+ event.params['temp']);
		});
		
		// ...
	}

The ``addEventListener`` mimics the browser `addEventListener <https://developer.mozilla.org/en-US/docs/Web/API/EventTarget/addEventListener>`_ function.
It accepts two parameters - a unique event name and a callable function. Similar to the browser function
you can add multiple callable functions to one event.  

C/C++ Code
----------

The ``addEventListener`` function is not present in the standard JerryScript VM. In this sample we register it as a built-in JavaScript function.
Excerpt from the ``application.cpp`` file::

	void startJsvm()
	{
		/*
		 * This is how we register a new function in JavaScript
		 * that will communicate directly with our C/C++ code.
		 */
		jsVm.registerFunction("addEventListener", addEventListener);
		
		// ...


The actual implementation of the ``addEventListener`` C++ function is in the ``vm_functions.cpp`` file. 
There you will find also the ``triggerEvent`` function which is available only in the C/C++ code. 
The latter is used to trigger events from the C/C++ code inside the JavaScript application::
	
	JsEventData params;
	params["temp"]="20";
	triggerEvent("EVENT_TEMP", params);
	
	
The code above simulates a temperature sensor sending the temperature in Celsius (C).