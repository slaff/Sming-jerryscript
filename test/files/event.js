function thirdListener(event, arg2) {
	print('>> Listener #3');
	if (typeof(arg2.value) != "number" || arg2.value != 12.5) {
		return Error("arg2 invalid");
	}
	arg2.value = true;
	return 3.5;
}

function init() {
	addEventListener("EVENT_TEMP", function (event, arg2) {
		print('>> Listener #1');
		print('Event name: ' + event.name + ', value: ' + event.params['temp']);
		if (arg2 !== undefined) {
			return Error("arg2 invalid");
		}
		return "one";
	});

	addEventListener("EVENT_TEMP", function (event, arg2) {
		print('>> Listener #2');
		if (typeof(arg2.value) != "boolean" || arg2.value != true) {
			return Error("arg2 invalid");
		}
		arg2.value = 2;
		return 2;
	});

	addEventListener("EVENT_TEMP", thirdListener);
}

function onload() {
	return "OK, module loaded!";
}

onload();
