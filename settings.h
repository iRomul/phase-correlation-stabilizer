#pragma once

#include <string>
#include <iostream>
#include <windows.h>

struct SteadySettings {

	// Типы
	enum device_type {DEV_FILE, DEV_CAMERA};
	enum output_type {OUT_GUI, OUT_FILE};

	// Методы
	SteadySettings();
	void load();
	void print();
	bool verify();

	// Поля
	std::string path_to_file;
	int frame_scale;
	int device_id;
	int camera_offset_threshold;
	int camera_lowpass_filter_amount;
	int frame_skipping_threshold;
	device_type dev_type;
	output_type out_type;
	std::string path_to_video;
	std::string path_to_out_video;

};