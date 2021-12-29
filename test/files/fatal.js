function allocateArray(count, value, dbg) {
	// With es.next we can do this:
	//   print(`allocateArray(${count}, ${value})`);
	print("allocateArray(" + count + ", " + value + ")");

	var x = [1, 2, 3, 4];
	for (i = 0; i < count; ++i) {
		dbg.count = i;
		x[i] = value;
		// print("x[" + i + "] = " + x[i]);
	}

	return true;
}

function infiniteLoop() {
	while (true) {
		// Help! Watchdog save me please!
	}
}
