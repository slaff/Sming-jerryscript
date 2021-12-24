var forceFatal = false;

function test(param) {
	callback(param);

	if (!forceFatal) {
		return true;
	}

	try {
		alert('>> Forcing out-of-memory fatal engine error <<')
	} catch (e) {
		print("alert() not available!");
	}

	var y = [1, 2, 3, 4];
	for (var x = 0; x < 10000; ++x) {
		y[x] = 0;
	}
}

function onload() {
	print("OK, module loaded!");
	return true;
}

onload();
