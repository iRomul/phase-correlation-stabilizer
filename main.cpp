#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include "image_device.h"
#include "algos.h"
#include "utility.h"
#include "settings.h"

#include <iostream>

using namespace std;
using namespace cv;

typedef Point2d (*Image_offset_calculation)(Mat img1, Mat img2);
typedef Point2d (*Image_offset_calculation_alt)(Mat img1, Mat& img2);

typedef void (*Image_shifting)(Mat& img, const Point2d& offset);
typedef void (*Image_shifting_alt)(Mat& img, Mat& trans, const Point2d& offset);

int main(int argc, int** argv) {

	// ---- НАСТРОЙКИ ----

	SteadySettings settings;
	settings.load();
	if(!settings.verify()) {
		cerr << "[ERROR] Work with the current configuration is not possible" << endl;
		exit(EXIT_FAILURE);
	}
	settings.print();

	// ---- УСТРОЙСТВО ВВОДА ----

	VideoCapture vcap(0);

	if(settings.dev_type == SteadySettings::DEV_CAMERA) {
		vcap.open(settings.device_id);
	} else {
		vcap.open(settings.path_to_video.c_str());
	}

	if(!vcap.isOpened()) {
		cerr << "[ERROR] Unable to open source video" << endl;
		exit(EXIT_FAILURE);
	}

	Size vsize;
	vsize.height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
	vsize.width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
	double frame_per_second = vcap.get(CV_CAP_PROP_FPS);

	// ---- УСТРОЙСТВО ВЫВОДА ----

	VideoWriter vwrt;
	if(settings.out_type == SteadySettings::OUT_FILE) {

		vwrt.open(settings.path_to_out_video, CV_FOURCC('M', 'J', 'P', 'G'), frame_per_second, vsize, true);

		if(!vwrt.isOpened()) {
			cerr << "[ERROR] Unable to create output file" << endl;
			exit(EXIT_FAILURE);
		}

	}

	// ---- АЛГОРИТМЫ ----

	//Image_offset_calculation_alt calc_offset = &calculate_offset_phase_optimized;
	Image_offset_calculation_alt calc_offset = &calculate_offset_phase_optimized_multiscale;
	Image_shifting_alt shift_image = &move_image_roi_alt;

	// ---- ПЕРЕМЕННЫЕ ----

	// Если тип вывода GUI, то необходимо создать окна
	if(settings.out_type == SteadySettings::OUT_GUI) {

		namedWindow("Original");

	}

	Mat current_frame; // Текущий кадр, полученный с устройства
	Mat base_frame; // Базовый кадр, от которого считается смещение
	Mat show_frame; // Преобразованный кадр

	Point2d offset(0, 0);

	bool reset_base_frame = true; // Флаг сброса базового кадра
	int command = 0; // Коды клавиш управления

	// ---- ГЛАВНЫЙ ЦИКЛ ----

	while(true) {

		// 1 - Получаем кадр с устройства

		vcap >> current_frame;

		if(current_frame.empty()) break;

		// 2 - Setting base frame

		if(reset_base_frame) {
			base_frame = current_frame.clone();
			reset_base_frame = false;
		}

		// 3 - Calculating offset

		offset = calc_offset(current_frame, base_frame);

		// 3.1 - Фильтруем координаты

		// 3.2 - Проверяем, не ушло ли изображение слишком далеко от базы

		if(offset.x > settings.camera_offset_threshold || offset.y > settings.camera_offset_threshold) {
#ifdef _DEBUG
			cout << "Changing baseframe" << endl;
#endif
			reset_base_frame = true;
		}

		// 4 - Shifting images

		shift_image(current_frame, show_frame, Point2d(0-offset.x, 0-offset.y));

		// - Выводим изображение

		if(settings.out_type == SteadySettings::OUT_GUI) {

			imshow("Original", show_frame);

		} else {
			vwrt << show_frame;
		}

		char mode = (char)waitKey(1);
		if(mode == 'e' || mode == 'у') break;

	} // while

	destroyAllWindows();

	return EXIT_SUCCESS;

}