var x;

function thirdListener(event) {
	print('Third listener is a normal function!');
	print('-------------' + (++x) + '------------------');
}

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
		print('Event name: ' + event.name + ', value: ' + event.params['temp']);
	});

	addEventListener("EVENT_TEMP", function(event) {
		// You can register more than one listener to an event
		print('Second listener');
	});

	// You can also run a normal function as callback instead of
	// an anonymous function.
	addEventListener("EVENT_TEMP", thirdListener);
}