#define _CRT_SECURE_NO_WARNINGS

#include "settings.h"
#include <windows.h>

using namespace std;

SteadySettings::SteadySettings() :
#ifdef _DEBUG
	path_to_file("C:\\Short\\cfg.ini"),
#else
	path_to_file("cfg.ini"),
#endif
	camera_offset_threshold(500),
	camera_lowpass_filter_amount(10),
	frame_skipping_threshold(5),
	frame_scale(1),
	device_id(0),
	dev_type(DEV_CAMERA) {

};

void SteadySettings::load() {

	cout << "[Setting] Loading settings from " << path_to_file << "\n";
	
	unsigned short const buffer_lenght = 64;
	char buffer[buffer_lenght];

	char* sec_input_type = "INPUT";
	char* sec_alg_settings = "CONTROL";
	
	GetPrivateProfileStringA(
		sec_input_type,
		"Type",
		NULL,
		buffer,
		buffer_lenght,
		path_to_file.c_str()
		);

	//If file
	if(buffer) {

		if(!strcmp(buffer, "File")) {
		
			dev_type = DEV_FILE;

			GetPrivateProfileStringA(
				sec_input_type,
				"Path",
				NULL,
				buffer,
				buffer_lenght,
				path_to_file.c_str()
				);

			if(buffer) {

				path_to_video = buffer;

			} else {

				path_to_video.clear();

			} // if
	
		} else {

			dev_type = DEV_CAMERA;

			GetPrivateProfileStringA(
				sec_input_type,
				"DeviceID",
				NULL,
				buffer,
				buffer_lenght,
				path_to_file.c_str()
				);

			if(buffer) {

				device_id = atoi(buffer);

			} else {

				device_id = 0;

			} // if

		} // if file else camera

	} // if

	GetPrivateProfileStringA(
		sec_alg_settings,
		"Offset_threshold",
		NULL,
		buffer,
		buffer_lenght,
		path_to_file.c_str()
		);

	if(buffer)
		camera_offset_threshold = atoi(buffer);

	GetPrivateProfileStringA(
		sec_alg_settings,
		"Lowpass_filter",
		NULL,
		buffer,
		buffer_lenght,
		path_to_file.c_str()
		);

	if(buffer)
		camera_lowpass_filter_amount = atoi(buffer);

	GetPrivateProfileStringA(
		sec_alg_settings,
		"Frame_skipping_threshold",
		NULL,
		buffer,
		buffer_lenght,
		path_to_file.c_str()
		);

	if(buffer)
		frame_skipping_threshold = atoi(buffer);

	GetPrivateProfileStringA(
		sec_alg_settings,
		"Frame_scale",
		NULL,
		buffer,
		buffer_lenght,
		path_to_file.c_str()
		);

	if(buffer)
		frame_scale = atoi(buffer);

}

void SteadySettings::save() {

	unsigned short const buffer_lenght = 64;
	char buffer[buffer_lenght];

	char* sec_input_type = "INPUT";
	char* sec_alg_settings = "CONTROL";

	WritePrivateProfileStringA(
		sec_input_type,
		"Type",
		dev_type == DEV_FILE ? "File" : "Camera",
		path_to_file.c_str()
		);

	if (dev_type == DEV_FILE && path_to_file.length()) {
	
		WritePrivateProfileStringA(
			sec_input_type,
			"Path",
			path_to_video.c_str(),
			path_to_file.c_str()
			);
	
	} // if

	WritePrivateProfileStringA(
		sec_alg_settings,
		"Offset_threshold",
		_itoa(camera_offset_threshold, buffer, 10),
		path_to_file.c_str()
		);

	WritePrivateProfileStringA(
		sec_alg_settings,
		"Lowpass_filter",
		_itoa(camera_lowpass_filter_amount, buffer,  10),
		path_to_file.c_str()
		);

	WritePrivateProfileStringA(
		sec_alg_settings,
		"Frame_skipping_threshold",
		_itoa(frame_skipping_threshold, buffer, 10),
		path_to_file.c_str()
		);

	WritePrivateProfileStringA(
		sec_alg_settings,
		"Frame_scale",
		_itoa(frame_scale, buffer, 10),
		path_to_file.c_str()
		);

}

void SteadySettings::print() {

	cout << "Settings" << endl;
	cout << "Capture device: [" << ((dev_type == DEV_FILE) ? "File" : "Camera") << "]" << endl;
	dev_type == DEV_FILE ? (cout << "Path to file: [" << path_to_video << "]" << endl) : cout << "CAMERA" << endl;;
	cout << "Frame scale: [" << frame_scale << "]" << endl;
	cout << "Offset threshold: [" << camera_offset_threshold << "]" << endl;
	cout << "Lowpass filter amount: [" << camera_lowpass_filter_amount << "]" << endl;
	cout << "Frame skipping threshold: [" << frame_skipping_threshold << "]" << endl;

}

bool SteadySettings::verify() {

	bool ok = true;

	if(frame_scale < 1) {
		cerr << "Scale can\'t be less than 1" << endl;
		ok = false;
	}

	if(camera_offset_threshold < 50) {
		cerr << "Offset thresold too low!" << endl;
		ok = false;
	}

	if(camera_lowpass_filter_amount < 1) {
		cerr << "Lowpass filter amound can\'t be less than 0" << cerr;
		ok = false;
	}

	// TODO: add skipping threshold

	if(dev_type == DEV_FILE) {
		if(path_to_video.empty()) {
			cerr << "Path to video is not set" << endl;
			ok = false;
		}
	} else {
		if(device_id < 0) {
			cerr << "Device ID must be more or eq than 0" << endl;
			ok = false;
		}
	}

	return ok;

}