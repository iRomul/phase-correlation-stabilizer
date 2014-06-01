#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>

#include "image_device.h"
#include "algos.h"
#include "utility.h"
#include "settings.h"

#include <iostream>
#include <time.h>
#include <math.h>

#define CMD_STABILIZER_OFF 1
#define CMD_ADVANCED_OUTPUT 2
#define CMD_RESET_BASE 4

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
	float frame_per_second = vcap.get(CV_CAP_PROP_FPS);
	frame_per_second = frame_per_second < 1 ? 15 : frame_per_second;

	cout << "[VIDEO]" << endl;
	cout << "Source: ";
	if(settings.dev_type == SteadySettings::DEV_CAMERA)
		cout << "CAMERA" << settings.device_id << endl;
	else
		cout << "File (" << settings.path_to_file << ")" << endl;
	if(settings.out_type == SteadySettings::OUT_FILE)
		cout << "Writing to file (" << settings.path_to_out_video << endl;
	cout << "Frame size: " << vsize.width << "x" << vsize.height << endl;
	cout << "FPS: " << frame_per_second << endl;

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
	float frame_ratio;

	Point2d offset(0, 0);
	Point2d prev_offset(0, 0);

	bool reset_base_frame = true; // Флаг сброса базового кадра
	unsigned char flags = 0;

	clock_t t_begin;
	float dur;

	float real_fps = 0;

	// ---- ГЛАВНЫЙ ЦИКЛ ----

	while(true) {		

		// 1 - Получаем кадр с устройства

		t_begin = clock();

		vcap >> current_frame;

		if(current_frame.empty()) break;

		// 2 - Setting base frame

		frame_ratio = (float)current_frame.rows / (float)current_frame.cols;

		if(!(flags&CMD_STABILIZER_OFF)) {

			if(reset_base_frame) {
				base_frame = current_frame.clone();
#ifdef _DEBUG
				cout << "Changing baseframe" << endl;
#endif
				reset_base_frame = false;
				prev_offset.x = 0;
				prev_offset.y = 0;
			}

			// 3 - Calculating offset

			offset = calc_offset(current_frame, base_frame);

			// 3.1 - Фильтруем координаты

			//cout << "LPF: " << settings.camera_lowpass_filter_amount << " POX: " << prev_offset.x << " PX: " << offset.x << endl;

			/*if(
				(
				abs(prev_offset.x - offset.x) > settings.camera_lowpass_filter_amount
				||
				abs(prev_offset.y - offset.y) > settings.camera_lowpass_filter_amount
				)
				&&
				!reset_base_frame
			) {
			
				offset = prev_offset;
				cout << "!" << endl;
			
			}*/

			// 3.2 - Проверяем, не ушло ли изображение слишком далеко от базы

			if(
				(abs(offset.x) > settings.camera_offset_threshold || abs(offset.y) > (settings.camera_offset_threshold * frame_ratio))
				||
				(abs(offset.x) > (current_frame.rows / 2) || abs(offset.y) > (current_frame.cols / 2))
			)
				reset_base_frame = true;

			// 4 - Shifting images

			shift_image(current_frame, show_frame, Point2d(0-offset.x, 0-offset.y));

		} else show_frame = current_frame;

		if(flags&CMD_ADVANCED_OUTPUT) draw_auxilliary_data(show_frame, settings, frame_per_second, real_fps, vsize, offset);

		// - Выводим изображение

		if(settings.out_type == SteadySettings::OUT_GUI) {
			imshow("Original", show_frame);
		} else {
			vwrt << show_frame;
		}

		// PostFX

		prev_offset = offset;

		t_begin = clock() - t_begin;
		dur = (double)t_begin/CLOCKS_PER_SEC;

		/*char mode = (char)waitKey(
			settings.dev_type == SteadySettings::DEV_CAMERA ?
				15 :
				dur > 1 / frame_per_second ? 1 : (frame_per_second - dur) * 1000);*/
		char mode = (char)waitKey(33);
		if(mode == 'e' || mode == 'у') break;
		if(mode == 'q' || mode == 'й') switch_flag(flags, CMD_ADVANCED_OUTPUT);
		if(mode == 's' || mode == 'ы') reset_base_frame = true;
		if(mode == 'w' || mode == 'ц') switch_flag(flags, CMD_STABILIZER_OFF);

		real_fps = !dur ? 0 : 1 / dur;

	} // while

	destroyAllWindows();

	return EXIT_SUCCESS;

}