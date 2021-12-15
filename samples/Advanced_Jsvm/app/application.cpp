#include <SmingCore.h>
#include <MultipartParser.h>
#include <HttpMultipartResource.h>
#include <Data/Stream/FileStream.h>
#include <Jerryscript.h>

#ifndef WIFI_SSID
#define WIFI_SSID "PleaseEnterSSID"
#define WIFI_PWD "PleaseEnterPass"
#endif

namespace
{
JS::Task task;
HttpServer webServer;
DEFINE_FSTR(MAIN_JS_FILE, "main.js.snap");

void startJsvm()
{
	JS::initialise();

	// Load the snapshot file
	if(!JS::Snapshot::loadFromFile(MAIN_JS_FILE)) {
		debug_e("Failed executing the following script: %s", String(MAIN_JS_FILE).c_str());
		return;
	}

	// Now you can initialize your script by calling a setup() JavaScript function
	if(JS::global().runFunction("setup").isError()) {
		debug_e("Failed executing the setup function.");
	}

	// Use the Run/Stop button to suspend/resume the JsVM
}

void onIndex(HttpRequest& request, HttpResponse& response)
{
	if(request.method == HTTP_POST) {
		debug_d("Successful post request...");
	}
	response.sendFile(F("index.html"));
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
		task.suspend();
	} else {
		body = F("{\"status\": \"running\"}");
		task.resume();
	}

	response.headers[HTTP_HEADER_CONTENT_TYPE] = toString(MIME_JSON);
	response.sendString(body);
}

void onFile(HttpRequest& request, HttpResponse& response)
{
	String file = request.uri.getRelativePath();

	if(file[0] == '.') {
		response.code = HTTP_STATUS_FORBIDDEN;
	} else {
		response.setCache(86400, true);
		response.sendFile(file);
	}
}

void onWasm(HttpRequest& request, HttpResponse& response)
{
	onFile(request, response);
	response.headers[HTTP_HEADER_CONTENT_TYPE] = F("application/wasm");
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
		task.suspend();
	}
	response.headers[HTTP_HEADER_CONTENT_TYPE] = toString(MIME_JSON);
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

	auto uploadResource = new HttpMultipartResource(fileUploadMapper, onUpload);
	webServer.paths.set("/update", uploadResource);
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
		Serial.println(_F("Unable to mount the spiffs file system!"));
		return;
	}

	// Station - WiFi client
	WifiStation.enable(true);
	WifiStation.config(_F(WIFI_SSID), _F(WIFI_PWD));

	System.onReady(startWebServer);
}
