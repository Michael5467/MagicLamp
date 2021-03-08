#include "web_functions.h"

File fsUploadFile;

void http_server_init()
{
	server.on("/", HTTP_GET, []() {
		if (!handleFileRead("/index.html"))
			server.send(404, "text/plain", "FileNotFound");
	});

	server.on("/index.html", HTTP_GET, []() {
		if (!handleFileRead("/index.html"))
			server.send(404, "text/plain", "FileNotFound");
	});

	server.on("/alarm.html", HTTP_GET, []() {
		if (!handleFileRead("/alarm.html"))
			server.send(404, "text/plain", "FileNotFound");
	});

	server.on("/settings", HTTP_GET, []() {
		if (!handleFileRead("/settings.html"))
			server.send(404, "text/plain", "FileNotFound");
	});

	server.on("/settings.html", HTTP_GET, []() {
		if (!handleFileRead("/settings.html"))
			server.send(404, "text/plain", "FileNotFound");
	});

	server.on("/action.html", HTTP_GET,  handleAction);
	server.on("/action.html", HTTP_POST, handleAction);

	server.on("/info", HTTP_GET, handleFilesystemInformation);
	server.on("/list", HTTP_GET, handleFileList);

	// //load editor
	// server.on("/edit", HTTP_GET, []() {
	// 	if (!handleFileRead("/edit.html"))
	// 		server.send(404, "text/plain", "FileNotFound");
	// });

	//load editor
	server.on("/edit", HTTP_GET, handleFileLoad);

	//create file
	server.on("/edit", HTTP_PUT, handleFileCreate);

	//delete file
	server.on("/edit", HTTP_DELETE, handleFileDelete);

	//upload file
	//first callback is called after the request has ended with all parsed arguments
	//second callback handles file uploads at that location
	server.on("/edit", HTTP_POST, []() {
		server.send(200, "text/plain", "");
	}, handleFileSave);

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

void handleAction()
{
	String message = "handleAction!\n\n";
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

	// Change lamp_state...
	message += "";
	if (server.hasArg("CNN"))
	{
		message += ">>> Connection DONE!";
	}
	if (server.hasArg("STS"))
	{
		message += ">>> Status request...";
		String opt = server.arg("STS");
		if (opt == "RST") 
		{
			message += ">>> opt == RST";
			ResetEffectSettings(&lamp_state);
		}
		if (opt == "GET") 
		{
			message += ">>> opt == GET";
			SendLampState(&lamp_state);
		}
		message += ">>> STS done";

	}
	if (server.hasArg("CFG"))
	{
		message += ">>> Config request...\n";

		String opt = server.arg("CFG");
		if (opt == "RST") 
		{
			message += ">>> opt == RST\n";
			readHostName(lamp_config.name, 32);
			message += ">>> restore name to: {";
			message += lamp_config.name;
			message += "}\n";
		}
		if (opt == "SET") 
		{
			DPRINTLNF(">>> opt == SET");
			message += "\n>>> opt == SET";
			if (server.hasArg("NAME"))
			{
				opt = server.arg("NAME");
				message += "\n>>> NAME == {";
				message += opt;
				message += "}";
				DPRINTLNF(">>> name from opt:");
				printString(opt.c_str());
				strcpy(lamp_config.name, opt.c_str());
				DPRINTLNF(">>> readHostName:");
				printString(lamp_config.name);

				// writeHostName(lamp_config.name);
				lamp_config.dirty = true;
				resetLamp();
			}
		}
		if (opt == "GET") 
		{
			DPRINTLNF("opt == GET");
			message += "\n>>> opt == GET";

			printString(lamp_config.name);

			String output = "{";
			output += "\"NAME\":\"";
			output += lamp_config.name;
			output += "\",";

			String DateTimeStr = makeStringDateTime(getCurrentDateTime(lamp_state));
			DPRINTLN(DateTimeStr);

			output += "\"DATE\":\"";
			output += DateTimeStr;
			output += "\"}";
			server.send(200, "text/json", output);
		}
		message += "\n>>> CFG done";
	}
	if (server.hasArg("ALR"))
	{
		message += ">>> Alarm request...\n";
		DPRINTLNF(">>> Alarm request...");

		String opt = server.arg("ALR");
		if (opt == "GET") 
		{
			DPRINTLNF("opt == GET");
			message += "\n>>> opt == GET";

			printConfig(lamp_config, true);

			String output = "{";
			output += "\"ALARMS\": [";
			// output += lamp_config.name;
			for (uint8_t i=0; i < 7; i++)
			{
				output += "{";
				output += "\"ON\":";
				output += (uint8_t)lamp_config.alarm[i].state;
				output += ",";
				output += "\"HOUR\":";
				output += lamp_config.alarm[i].hour;
				output += ",";
				output += "\"MIN\":";
				output += lamp_config.alarm[i].min;
				output += "}";
				if (i < 6)
				{
					output += ", ";
				}
			}
			output += "] }";

			server.send(200, "text/json", output);
		}
		if (opt == "SET") 
		{
			DPRINTLNF("opt == SET");
			message += "\n>>> opt == SET";

			int8_t _dayIndex = -1;
			int8_t _status = -1;
			int8_t _hour = -1;
			int8_t _minute = -1;

			if (server.hasArg("day"))
			{
				DPRINTLNF("arg == day");

				opt = server.arg("day");
				message += "\n>>> day == {";
				message += opt;
				message += "}";

				_dayIndex = opt.toInt();
			}
			if (server.hasArg("time"))
			{
				DPRINTLNF("arg == time");

				opt = server.arg("time");
				message += "\n>>> time == {";
				message += opt;
				message += "}";

				_hour = str2int(opt.c_str(), 2);
				_minute = str2int(opt.c_str()+3, 2);
			}
			if (server.hasArg("hour"))
			{
				DPRINTLNF("arg == hour");

				opt = server.arg("hour");
				message += "\n>>> hour == {";
				message += opt;
				message += "}";

				_hour = opt.toInt();
			}
			if (server.hasArg("min"))
			{
				DPRINTLNF("arg == min");

				opt = server.arg("min");
				message += "\n>>> min == {";
				message += opt;
				message += "}";

				_minute = opt.toInt();
			}
			if (server.hasArg("status"))
			{
				DPRINTLNF("arg == status");

				opt = server.arg("status");
				message += "\n>>> status == {";
				message += opt;
				message += "}";

				_status = opt.toInt();
			}

			if ((_dayIndex != -1) && (_hour != -1) && (_minute != -1))
			{
				DPRINTLN_FULL(_dayIndex);
				DPRINTLN_FULL(_hour);
				DPRINTLN_FULL(_minute);
				DPRINTLNF(" ");

				lamp_config.alarm[_dayIndex].hour = _hour;
				lamp_config.alarm[_dayIndex].min = _minute;
				lamp_config.dirty = true;

				printConfig(lamp_config, true);
			}
			if ((_dayIndex != -1) && (_status != -1))
			{
				DPRINTLN_FULL(_dayIndex);
				DPRINTLN_FULL(_status);
				DPRINTLNF(" ");

				lamp_config.alarm[_dayIndex].state = (alarm_state_t)_status;
				lamp_config.dirty = true;

				printConfig(lamp_config, true);
			}
		}
	}
	if (server.hasArg("PWR"))
	{
		String opt = server.arg("PWR");
		if (opt == "0" || opt == "OFF")
		{
			lamp_state.state = false;
			message += ">>> Power OFF";
		}
		else if (opt == "1" || opt == "ON")
		{
			lamp_state.state = true;
			message += ">>> Power ON";
		}
		changePower(&lamp_state);
	}
	if (server.hasArg("MOD")) 
	{
		String opt = server.arg("MOD");
        setMode(&lamp_state, opt.toInt());
		message += ">>> MOD changed";
	}
	if (server.hasArg("BRI")) 
	{
		String opt = server.arg("BRI");
		uint8_t raw = opt.toInt();
        lamp_state.brightness_raw = raw;
        setBrightness(&lamp_state, (raw<<4)-1);
		message += ">>> BRI changed";
	}
	if (server.hasArg("SPD")) 
	{
		String opt = server.arg("SPD");
		uint8_t raw = opt.toInt();
        lamp_state.speed_raw = raw;
        setEffectsSpeed(&lamp_state, (512-((raw-1)<<4)));
		message += ">>> SPD changed";
	}
	if (server.hasArg("DEN")) 
	{
		String opt = server.arg("DEN");
		uint16_t raw = opt.toInt();
		lamp_state.scale_raw = (uint8_t)raw;
        switch (lamp_state.effect) {
            case (EFF_CODE_SNOW):
            case (EFF_CODE_BALLS):
            case (EFF_CODE_FIRE):
            case (EFF_CODE_SPARKLES):
            case (EFF_CODE_MATRIX):
                break;
            case (EFF_CODE_STARFALL):
                // raw = 5 * raw;
                break;
            case (EFF_CODE_SNAKE):
                break;
            case (EFF_CODE_LAVA):
            case (EFF_CODE_CLOUD):
            case (EFF_CODE_ZEBRA):
            case (EFF_CODE_FOREST):
            case (EFF_CODE_OCEAN):
            case (EFF_CODE_PLASMA):
            case (EFF_CODE_PLUM):
            case (EFF_CODE_RANDOM):
            case (EFF_CODE_RAINBOW):
            case (EFF_CODE_RAINBOW_STRIPE):
            	raw = 10 * raw;
                break;
        }
        lamp_state.scale = raw;
		DPRINTF("Set Density: value=");
		DPRINTLN(raw);
        updateMode(&lamp_state);
		message += ">>> DEN changed";
	}

	server.send(200, "text/plain", message);
}

void SendLampState(the_lamp_state_t *lamp_state)
{
	DPRINTLNF("SendLampState")

	String output = "{";
	if (lamp_state->state)
		output += "\"STATE\":1,";
	else
		output += "\"STATE\":0,";

	output += "\"MOD\":";
	output += lamp_state->effect;
	output += ",";

	output += "\"BRI\":";
	output += lamp_state->brightness_raw;
	output += ",";

	output += "\"SPD\":";
	output += lamp_state->speed_raw;
	output += ",";

	output += "\"DEN\":";
	output += lamp_state->scale_raw;

	output += "}";
	server.send(200, "text/json", output);
}

void handleNotFound()
{
	// If the client requests any URI - send it if it exists
	if (!handleFileRead(server.uri()))
	{
		// otherwise, respond with a 404 (Not Found) error
		DPRINTLNF("handleNotFound")
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
}

bool handleFileRead(const String &path)
{
	DPRINTF("handleFileRead: ")
	DPRINTLN(path)
	String contentType = getContentType(path);
	String pathWithGz = path + ".gz";
	if (LittleFS.exists(path) || LittleFS.exists(pathWithGz))
	{
		File file;
		if (LittleFS.exists(pathWithGz))
		{
			file = LittleFS.open(pathWithGz, "r");
		}
		else
		{
			file = LittleFS.open(path, "r");
		}
		if (file)
		{
			server.streamFile(file, contentType);
			file.close();
			return true;
		}
	}
	return false;
}

void handleFilesystemInformation()
{
	DPRINTLNF("handleFilesystemInformation")

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
	DPRINTLNF("handleFileList: ")
	DPRINTLN(path)
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

void handleFileSave()
{
	HTTPUpload &upload = server.upload();
	if (upload.status == UPLOAD_FILE_START)
	{
		String filename = upload.filename;
		if (!filename.startsWith("/"))
			filename = "/" + filename;
		// Serial.print("handleFileSave Name: ");
		// Serial.println(filename);
		DPRINTF("handleFileSave Name: ")
		DPRINTLN(filename)
		fsUploadFile = LittleFS.open(filename, "w");
		filename = String();
	}
	else if (upload.status == UPLOAD_FILE_WRITE)
	{
		// Serial.print("handleFileSave Data: ");
		// Serial.println(upload.currentSize);
		DPRINTF("handleFileSave Data: ")
		DPRINTLN(upload.currentSize)
		if (fsUploadFile)
			fsUploadFile.write(upload.buf, upload.currentSize);
	}
	else if (upload.status == UPLOAD_FILE_END)
	{
		// Serial.print("handleFileSave Size: ");
		// Serial.println(upload.totalSize);
		DPRINTF("handleFileSave Size: ")
		DPRINTLN(upload.totalSize)
		if (fsUploadFile)
			fsUploadFile.close();
	}
}
void handleFileLoad()
{
	if (!server.hasArg("dir"))
	{
		server.send(500, "text/plain", "BAD ARGS");
		return;
	}
	String path = server.arg("dir");
	DPRINTLNF("handleFileLoad: ");
	DPRINTLN(path);
	if (!handleFileRead(path))
		return server.send(404, "text/plain", "Something went wrong");
	else 
	{
		DPRINTLNF("Debug read file:");
		DPRINTLN(sizeof(lamp_config_s));

		// char buffer[1024];
		// File cfg_file = openConfiguration("r");
		// cfg_file.read((uint8_t *)buffer, sizeof(lamp_config_s));
		// closeConfiguration(cfg_file);
		// printRawConfig((lamp_config_s *)buffer);
	}
	server.send(200, "text/plain", "");
}


void handleFileDelete()
{
	if (!server.hasArg("dir"))
	{
		server.send(500, "text/plain", "BAD ARGS");
		return;
	}
	String path = server.arg("dir");
	DPRINTLNF("handleFileDelete: ")
	DPRINTLN(path)
	if (path == "/")
		return server.send(500, "text/plain", "BAD PATH");
	if (!LittleFS.exists(path))
		return server.send(404, "text/plain", "FileNotFound");
	LittleFS.remove(path);
	server.send(200, "text/plain", "");
}

void handleFileCreate()
{
	if (server.args() == 0)
		return server.send(500, "text/plain", "BAD ARGS");
	String path = server.arg(0);
	DPRINTLNF("handleFileCreate: ")
	DPRINTLN(path)
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
}

const char* getContentType(const String &filename)
{
	if (server.hasArg("download"))
		return "application/octet-stream";
	else if (filename.endsWith(".htm"))
		return "text/html";
	else if (filename.endsWith(".html"))
		return "text/html";
	else if (filename.endsWith(".css"))
		return "text/css";
	else if (filename.endsWith(".js"))
		return "application/javascript";
	else if (filename.endsWith(".json"))
		return "text/json";
	else if (filename.endsWith(".ico"))
		return "image/x-icon";
	else if (filename.endsWith(".svg"))
		return "image/svg+xml";
	else if (filename.endsWith(".zip"))
		return "application/x-zip";
	else if (filename.endsWith(".gz"))
		return "application/x-gzip";
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
	else if (filename.endsWith(".png"))
		return "image/png";
	else if (filename.endsWith(".gif"))
		return "image/gif";
	else if (filename.endsWith(".jpg"))
		return "image/jpeg";
	else if (filename.endsWith(".xml"))
		return "text/xml";
	else if (filename.endsWith(".pdf"))
		return "application/x-pdf";
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
