#include "utility.h"
#include <iostream>
#include <iomanip>

using namespace cv;

int mod_custom(int num, int max) {

	int t = num%max;
	return !t ? (!num ? 0 : num/abs(num)*max) : t;

};

int _0(const int x) {
	return x < 0 ? 0 : x;
}

int _0ia(const int x) {
	return x < 0 ? abs(x) : 0;
}

double round(double number) {
	return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
}

void switch_flag(unsigned char& flags, unsigned int flag) {
	flags&flag ? flags^=flag : flags|=flag;
}

void draw_auxilliary_data(Mat& frame, const SteadySettings& settings, const double& fps, const double& real_fps, const Size& video_size, const Point2d& offset) {

	std::stringstream text;

	const short draw_field_width = 250;
	const short draw_field_height = 200;
	const short border = 5;
	int radius = draw_field_height/2 - 50;
	float k = static_cast<float>(radius) / static_cast<float>(settings.camera_offset_threshold);

	rectangle(frame, Rect(5, 5, draw_field_width, draw_field_height), Scalar(0xAA, 0xAA, 0xAA), CV_FILLED);

	if(settings.dev_type == SteadySettings::DEV_CAMERA) {
		text << "CAMERA" << settings.device_id;
		putText(frame, text.str(), Point2d(2*border, 2*border+10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));
	} else
		putText(frame, "FILE", Point2d(2*border, 2*border+10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));

	text.str("");
	text << video_size.width << "x" << video_size.height;
	putText(frame, text.str(), Point2d(2*border, 2*border+30), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));

	text.str("");
	text << "FPS: " << fps;
	putText(frame, text.str(), Point2d(2*border, 2*border+50), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));

	text.str("");
	text << "Real: " << std::setprecision(1) << std::fixed << real_fps;
	if(real_fps < fps) {
		putText(frame, text.str(), Point2d(2*border, 2*border+70), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));
	} else {
		putText(frame, text.str(), Point2d(2*border, 2*border+70), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));
	}	

	circle(frame, Point2d(draw_field_width - radius - border, draw_field_height/2), radius, Scalar(0xFA, 0xCE, 0xAD), 2, CV_AA);
	line(frame, Point2d(draw_field_width - 2*radius - border, draw_field_height/2), Point2d(draw_field_width - border, draw_field_height/2), Scalar(0xFA, 0xCE, 0xAD), 2);
	line(frame, Point2d(draw_field_width - border - radius, draw_field_height/2 + radius), Point2d(draw_field_width - border - radius, draw_field_height/2 - radius), Scalar(0xFA, 0xCE, 0xAD), 2);
	circle(frame, Point2d(offset.x * k + draw_field_width - radius - border, offset.y * k + draw_field_height/2), radius - 30, Scalar(0xFA-15, 0xCE-15, 0xAD-15), 2, CV_AA);

}