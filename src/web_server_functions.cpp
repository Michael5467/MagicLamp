#include "web_functions.h"

File fsUploadFile;

void http_server_init()
{
	server.on("/", HTTP_GET, []() {
		if (!handleFileRead("/index.html"))
			server.send(404, "text/plain", "FileNotFound");
	}); //list directory

	server.on("/index.html", HTTP_GET, []() {
		if (!handleFileRead("/index.html"))
			server.send(404, "text/plain", "FileNotFound");
	}); //list directory

	server.on("/settings", HTTP_GET, []() {
		if (!handleFileRead("/settings.html"))
			server.send(404, "text/plain", "FileNotFound");
	});

	server.on("/settings.html", HTTP_GET, []() {
		if (!handleFileRead("/settings.html"))
			server.send(404, "text/plain", "FileNotFound");
	});

	server.on("/info", HTTP_GET, handleFilesystemInformation);
	server.on("/list", HTTP_GET, handleFileList);

	//load editor
	server.on("/edit", HTTP_GET, []() {
		if (!handleFileRead("/edit.html"))
			server.send(404, "text/plain", "FileNotFound");
	});

	//create file
	server.on("/edit", HTTP_PUT, handleFileCreate);

	//delete file
	server.on("/edit", HTTP_DELETE, handleFileDelete);

	//first callback is called after the request has ended with all parsed arguments
	//second callback handles file uploads at that location
	server.on("/edit", HTTP_POST, []() {
		server.send(200, "text/plain", "");
	}, handleFileUpload);

	//called when the url is not defined here
	//use it to load content from LittleFS
	server.onNotFound(handleNotFound);

	server.serveStatic("/font", LittleFS, "/font", "max-age=86400");
	server.serveStatic("/js", LittleFS, "/js", "max-age=86400");
	server.serveStatic("/css", LittleFS, "/css", "max-age=86400");

	//get heap status, analog input value and all GPIO statuses in one json call
	server.on("/all", HTTP_GET, []() {
		String json = "{";
		json += "\"heap\":" + String(ESP.getFreeHeap());
		json += ", \"analog\":" + String(analogRead(A0));
		json += ", \"gpio\":" + String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
		json += "}";
		server.send(200, "text/json", json);
		json = String();
	});
}

void handleNotFound()
{
	DPRINTLN("handleNotFound")
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";
	for (uint8_t i = 0; i < server.args(); i++)
	{
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	}
	server.send(404, "text/plain", message);
}

bool handleFileRead(String path)
{
	//Serial.println("handleFileRead: " + path);
	DPRINTLN("handleFileRead: " + path)
	// if (path.endsWith("/"))
	// 	path += "index.htm";
	String contentType = getContentType(path);
	String pathWithGz = path + ".gz";
	if (LittleFS.exists(pathWithGz) || LittleFS.exists(path))
	{
		if (LittleFS.exists(pathWithGz))
			path += ".gz";
		File file = LittleFS.open(path, "r");
		// size_t sent = server.streamFile(file, contentType);
		server.streamFile(file, contentType);
		file.close();
		return true;
	}
	return false;
}

void handleFilesystemInformation()
{
	DPRINTLN("handleFilesystemInformation")

	FSInfo fs_info;
	LittleFS.info(fs_info);

	String output = "{";
	output += "\"Total space\":\"";
	output += fs_info.totalBytes;
	output += "\",";

	output += "\"Total space used\":\"";
	output += fs_info.usedBytes;
	output += "\",";

	output += "\"Block size\":\"";
	output += fs_info.blockSize;
	output += "\",";

	output += "\"Page size\":\"";
	output += fs_info.pageSize;
	output += "\",";

	output += "\"Max open files\":\"";
	output += fs_info.maxOpenFiles;
	output += "\",";

	output += "\"Max path lenght\":\"";
	output += fs_info.maxPathLength;
	output += "\"";

	output += "}";
	server.send(200, "text/json", output);
}

void handleFileList()
{
	if (!server.hasArg("dir"))
	{
		server.send(500, "text/plain", "BAD ARGS");
		return;
	}

	String path = server.arg("dir");
	// Serial.println("handleFileList: " + path);
	DPRINTLN("handleFileList: " + path)
	Dir dir = LittleFS.openDir(path);
	path = String();

	String output = "[";
	while (dir.next())
	{
		File entry = dir.openFile("r");
		if (output != "[")
			output += ',';
		bool isDir = false;
		output += "{\"type\":\"";
		output += (isDir) ? "dir" : "file";
		output += "\",\"name\":\"";
		// output += String(entry.name()).substring(1);
		output += String(entry.name());
		output += "\"}";
		entry.close();
	}
	output += "]";
	server.send(200, "text/json", output);
}

void handleFileUpload()
{
	if (server.uri() != "/edit")
		return;
	HTTPUpload &upload = server.upload();
	if (upload.status == UPLOAD_FILE_START)
	{
		String filename = upload.filename;
		if (!filename.startsWith("/"))
			filename = "/" + filename;
		// Serial.print("handleFileUpload Name: ");
		// Serial.println(filename);
		DPRINT("handleFileUpload Name: ")
		DPRINTLN(filename)
		fsUploadFile = LittleFS.open(filename, "w");
		filename = String();
	}
	else if (upload.status == UPLOAD_FILE_WRITE)
	{
		// Serial.print("handleFileUpload Data: ");
		// Serial.println(upload.currentSize);
		DPRINT("handleFileUpload Data: ")
		DPRINTLN(upload.currentSize)
		if (fsUploadFile)
			fsUploadFile.write(upload.buf, upload.currentSize);
	}
	else if (upload.status == UPLOAD_FILE_END)
	{
		// Serial.print("handleFileUpload Size: ");
		// Serial.println(upload.totalSize);
		DPRINT("handleFileUpload Size: ")
		DPRINTLN(upload.totalSize)
		if (fsUploadFile)
			fsUploadFile.close();
	}
}

void handleFileDelete()
{
	if (server.args() == 0)
		return server.send(500, "text/plain", "BAD ARGS");
	String path = server.arg(0);
	// Serial.println("handleFileDelete: " + path);
	DPRINTLN("handleFileDelete: " + path)
	if (path == "/")
		return server.send(500, "text/plain", "BAD PATH");
	if (!LittleFS.exists(path))
		return server.send(404, "text/plain", "FileNotFound");
	LittleFS.remove(path);
	server.send(200, "text/plain", "");
	path = String();
}

void handleFileCreate()
{
	if (server.args() == 0)
		return server.send(500, "text/plain", "BAD ARGS");
	String path = server.arg(0);
	// Serial.println("handleFileCreate: " + path);
	DPRINTLN("handleFileCreate: " + path)
	if (path == "/")
		return server.send(500, "text/plain", "BAD PATH");
	if (LittleFS.exists(path))
		return server.send(500, "text/plain", "FILE EXISTS");
	File file = LittleFS.open(path, "w");
	if (file)
		file.close();
	else
		return server.send(500, "text/plain", "CREATE FAILED");
	server.send(200, "text/plain", "");
	path = String();
}

String getContentType(String filename)
{
	if (server.hasArg("download"))
		return "application/octet-stream";
	else if (filename.endsWith(".htm"))
		return "text/html";
	else if (filename.endsWith(".html"))
		return "text/html";
	else if (filename.endsWith(".css"))
		return "text/css";
	else if (filename.endsWith(".svg"))
		return "image/svg+xml";
	else if (filename.endsWith(".woff"))
		return "application/font-woff";
	else if (filename.endsWith(".woff2"))
		return "application/font-woff2";
	else if (filename.endsWith(".ttf"))
		return "application/x-font-truetype";
	else if (filename.endsWith(".eot"))
		return "application/vnd.ms-fontobject";
	else if (filename.endsWith(".otf"))
		return "application/x-font-opentype";
	else if (filename.endsWith(".js"))
		return "application/javascript";
	else if (filename.endsWith(".png"))
		return "image/png";
	else if (filename.endsWith(".gif"))
		return "image/gif";
	else if (filename.endsWith(".jpg"))
		return "image/jpeg";
	else if (filename.endsWith(".ico"))
		return "image/x-icon";
	else if (filename.endsWith(".xml"))
		return "text/xml";
	else if (filename.endsWith(".pdf"))
		return "application/x-pdf";
	else if (filename.endsWith(".zip"))
		return "application/x-zip";
	else if (filename.endsWith(".json"))
		return "text/html";
	else if (filename.endsWith(".gz"))
		return "application/x-gzip";
	return "text/plain";
}

String formatBytes(size_t bytes)
{
	if (bytes < 1024)
		return String(bytes) + "B";
	else if (bytes < (1024 * 1024))
		return String(bytes / 1024.0) + "KB";
	else if (bytes < (1024 * 1024 * 1024))
		return String(bytes / 1024.0 / 1024.0) + "MB";
	else
		return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
}
