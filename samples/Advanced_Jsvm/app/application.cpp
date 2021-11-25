#include <SmingCore.h>
#include <MultipartParser.h>
#include <HttpMultipartResource.h>
#include <Data/Stream/FileStream.h>
#include <jsvm.h>
#include "JsvmTask.h"

namespace
{
Jsvm jsVm;
JsvmTask jsTask(jsVm);

HttpServer webServer;

constexpr char MAIN_JS_FILE[]{"main.js.snap"};

void startJsvm()
{
	// Load the snapshot file
	if(!jsVm.loadFromFile(MAIN_JS_FILE)) {
		debug_e("Failed executing the following script: %s", MAIN_JS_FILE);
		return;
	}

	// Now you can initilize your script by calling a setup() JavaScript function
	if(!jsVm.runFunction("setup")) {
		debug_e("Failed executing the setup function.");
	}

	// Use the Run/Stop button to suspend/resume the JsVM
}

void onIndex(HttpRequest& request, HttpResponse& response)
{
	if(request.method == HTTP_POST) {
		debug_d("Successful post request...");
	}
	response.sendFile("index.html");
}

void onTask(HttpRequest& request, HttpResponse& response)
{
	auto runTask = request.getQueryParameter("runTask", "0");
	String body = F("{\"status\": \"failed\"}");
	if(response.isSuccess()) {
		body = F("{\"status\": \"ok\"}");
	}

	if(runTask == "0") {
		body = F("{\"status\": \"stopped\"}");
		jsTask.suspend();
	} else {
		body = F("{\"status\": \"running\"}");
		jsTask.resume();
	}

	response.headers[HTTP_HEADER_CONTENT_TYPE] = "application/json";
	response.sendString(body);
}

void onFile(HttpRequest& request, HttpResponse& response)
{
	String file = request.uri.Path;
	if(file[0] == '/')
		file = file.substring(1);

	if(file[0] == '.')
		response.code = HTTP_STATUS_FORBIDDEN;
	else {
		response.setCache(86400, true);
		response.sendFile(file);
	}
}

void onWasm(HttpRequest& request, HttpResponse& response)
{
	onFile(request, response);
	response.headers[HTTP_HEADER_CONTENT_TYPE] = "application/wasm";
}

void fileUploadMapper(HttpFiles& files)
{
	files["mainjs"] = new FileStream(MAIN_JS_FILE, File::CreateNewAlways | File::WriteOnly);
}

int onUpload(HttpServerConnection& connection, HttpRequest& request, HttpResponse& response)
{
	String body = F("{\"status\": \"failed\"}");
	if(response.isSuccess()) {
		body = F("{\"status\": \"ok\"}");
		jsTask.suspend();
	}
	response.headers[HTTP_HEADER_CONTENT_TYPE] = "application/json";
	response.sendString(body);

	startJsvm();

	return 0;
}

void startWebServer()
{
	webServer.setBodyParser(MIME_FORM_MULTIPART, formMultipartParser);
	webServer.listen(80);
	webServer.paths.set("/", onIndex);
	webServer.paths.set("/task", onTask);

	HttpMultipartResource* uploadResouce = new HttpMultipartResource(fileUploadMapper, onUpload);
	webServer.paths.set("/update", uploadResouce);
	webServer.paths.set("/jsc.wasm", onWasm);

	webServer.paths.setDefault(onFile);

	startJsvm();
}

} // namespace

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	// Mount file system, in order to work with files
	if(!spiffs_mount()) {
		Serial.println("Unable to mount the spiffs file system!");
		return;
	}

	System.onReady(startWebServer);
}