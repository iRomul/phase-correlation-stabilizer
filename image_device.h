#pragma once
#include <opencv2/imgproc/imgproc.hpp>

class ImageDevice {

	cv::Mat image;
	cv::Mat window;
	cv::Size window_size;
	cv::Point2d window_coord;
	bool everything_right;
	//Различные движения окна
	void move_horizontal_right(unsigned range); //Движение вперёд-назад вправо от начальной точки

public:

	//Конструкторы
	ImageDevice();
	ImageDevice(char* , cv::Size); //Конструктор с вызовом open(...) и set_window_size()
	//Базовая настройка
	void open(char* ); //Загрузить изображение по пути
	void set_window_size(cv::Size); //Установить размеры окна
	void set_base_coord(cv::Point2d);
	//Данные об устройстве
	bool is_opened(); //Всё ли хорошо
	ImageDevice& operator>>(cv::Mat&); //Получить текущее окно
	cv::Size set_window_size(); //Получить размеры

};