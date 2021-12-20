var x;

/**
 * This is a function that will be called once.
 */
function setup() {
	x = 0;
	print ('Setup: X='+x);

	// If return is not provided, result will be 'undefined'
	// return true;
}

/**
 * This is a function that will be called every half a second
 */
function loop() {
	x++;
	print('Loop: X='+x);
}

/**
 * You can call setup() directly after loading, if you prefer.
 */
// setup();
