#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "image_device.h"
#include <iostream>

using namespace std;
using namespace cv;

Point2d calculate_offset(Mat one, Mat two) {

	cvtColor(one, one, CV_BGR2GRAY);
	cvtColor(two, two, CV_BGR2GRAY);
	one.convertTo(one, CV_64FC1);
	two.convertTo(two, CV_64FC1);
	return phaseCorrelate(one, two);

}

int main() {

	//Настраиваем устройство
	char* path_to_image = "C:\\Short\\Projects\\OFBench\\Media\\TestExampleTwo.jpg";
	Size frame_size(600, 600);
	ImageDevice imdev(path_to_image, frame_size);
	Mat current_frame; //Текущий кадр
	Mat reference_frame; //Опорный кадр для штатива
	Mat processed_frame(frame_size, CV_8UC3);

	//Базовые настройки
	int offset_threshold = 10; //Порог смещений для штатива

	//Впомогательные переменные
	bool use_last_frame = false; //Флаг использования предыдущего кадра для штатива
	Point2d offset(0, 0); //Смещения между 2 изображениями
	Rect cutter_frame; //Прямоугольник для ROI в смещенных изображениях
	namedWindow("Ready"); //Окошки
	namedWindow("Steady");
	namedWindow("Ref");

	while(true) {

		imdev >> current_frame; //Получаем изображение с устройства
		imshow("Ready", current_frame); //Выводим изображние в первозданном виде

		//БЛОК 1. Штатив
		//Если по какой-то причине флаг сброшен, то используем новое референсное изображение
		//Причины сброса:
		//	Смещение изображение больше заданного порога
		//	Слишком большое смещение
		if(!use_last_frame) {
			reference_frame = current_frame;
			use_last_frame = true;
		}

		offset = calculate_offset(current_frame, reference_frame); //Подсчитываем смещение между изображениями
		//Если смещение больше допустимой нормы...
		if(abs(offset.x) > offset_threshold || abs(offset.y) > offset_threshold) {

			//...значит штатив сдвинули с места и следующий референсный кадр изменится
			use_last_frame = false;

		} else {

			//В противном случае мы двигаем исходное изображение, предотвращая смещение и voila!
			//Ситуация, когда камера сдвинулась влево вниз
			if(offset.x >= 0 && offset.y >= 0) {
				//Проверяем, не сместилось ли изображение слишком далеко
				if(offset.x > current_frame.cols && offset.y > current_frame.rows)
					use_last_frame = false;
				else {
					processed_frame.adjustROI(offset.y, current_frame.rows, offset.x, current_frame.cols) = current_frame(Rect(0, 0, current_frame.cols-offset.x, current_frame.rows-offset.y));
				}
				
			}

		}
		
		imshow("Steady", processed_frame);
		imshow("Ref", reference_frame);

		char c = (char)waitKey(250);

		if(c == 'e' || c == 'у') break; //Если нажата e, то выходим
		if(c == 'q' || c == 'й') {--offset_threshold; cout << "Threshold " << offset_threshold << endl;}
		if(c == 'w' || c == 'ц') {++offset_threshold; cout << "Threshold " << offset_threshold << endl;}

	}

	return EXIT_SUCCESS;

}