var x;
var exceptionEnabled = false;

function thirdListener(event) {
	print('Third listener is a normal function!');
	print('-------------' + (++x) + '------------------');

	if(!exceptionEnabled) {
		return;
	}

	try{
		alert('>> Forcing out-of-memory fatal engine error <<')
	} catch(e) {
		print("alert() not available!");
	}

	var y = [1, 2, 3, 4];
	for (var x = 0; x < 10000; ++x) {
		y[x] = 0;
	}
}

/**
 * This is a function that will be called once.
 */
function init() {
	x = 0;
	print('Init: X=' + x);

	// This is how we register an event listener in JavaScript
	addEventListener("EVENT_TEMP", function (event) {
		// An event has a name and multiple params.
		// The params have string keys, and values can be of any type.
		print('Event name: ' + event.name + ', value: ' + event.params['temp'] + ', origin: ' + event.params.origin);
	});

	addEventListener("EVENT_TEMP", function (event) {
		// You can register more than one listener to an event
		print('Second listener');
	});

	// You can also run a normal function as callback instead of
	// an anonymous function.
	addEventListener("EVENT_TEMP", thirdListener);

	myPrint("Hello there", 1, 2, 3, Error("abc"), [12, 23]);
}

init();
