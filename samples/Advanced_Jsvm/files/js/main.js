var x;


function thirdListener(event)
{
	print('Third listener');
}

/**
 * This is a function that will be called once.
 */
function setup() {
	x = 0;
	print('Setup: X=' + x);

	addEventListener("EVENT_TEMP", function(event) {
		print('Event name: ' + event.name + ', value: '	+ event.params['temp']);
	});
	
//	addEventListener("EVENT_TEMP", function(event) {
//		print('Second listner');
//	});
//	
//	addEventListener("EVENT_TEMP", thirdListener);
}

/**
 * This is a function that will be called every half a second
 */
function loop() {
	x++;
	print('Loop: X=' + x);
}
