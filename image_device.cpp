#include "image_device.h"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

ImageDevice::ImageDevice() : window_size(50, 50), window_coord(0, 0), everything_right(false) {}

ImageDevice::ImageDevice(char* path_to_image, cv::Size sz) {

	open(path_to_image);
	set_window_size(sz);
	window.create(sz, CV_8UC3);
	set_base_coord(cv::Point2d(0, 0));
	everything_right = false;

}

void ImageDevice::open(char* path_to_image) {

	image = cv::imread(path_to_image);

}

void ImageDevice::set_base_coord(cv::Point2d pt) {

	window_coord = pt;

}

void ImageDevice::set_window_size(cv::Size sz) {

	window_size = sz;

}

bool ImageDevice::is_opened() {

	if(!image.data) return false;
	else return true;
	//TODO: Добавить остальные условия проверки
	//Размер окна и пр.

}

cv::Size ImageDevice::set_window_size() {

	return window_size;

}

void ImageDevice::move_horizontal_right(unsigned range) {

	static unsigned position = 0;
	if(position > range)
		position = 0;

	window = image(cv::Rect(window_coord.x+position, window_coord.y, window_size.width, window_size.height));
	++position;

};

ImageDevice& ImageDevice::operator >>(cv::Mat& outer_image) {

	outer_image = window;
	move_horizontal_right(300);
	return *this;

}