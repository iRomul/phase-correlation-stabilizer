#pragma once
#include <opencv2/imgproc/imgproc.hpp>

#include "settings.h"

using namespace cv;

////		Класс для работы с алгоритмами		////

// Статический стабилизатор, основанный на фазовой корреляции
class Stabilizer {

	Mat base_frame; // Опорный кадр
	Point2f prev_offset; // Предыдущие координаты смещения
	Point2f offset; // Текущие координаты смещения
	SteadySettings* settings; // Настройки программы
	bool state; // Включено/выключено
	float inner_scale; // Масштабирование внутри фазовой корреляции
	float frame_scale; // Масштабирование результирующего кадра
	bool reset_base_frame; // Флаг сброса базового кадра

public:
	Stabilizer();
	void set_settings(SteadySettings*);
	void stabilize(Mat& A);
	void shift_images();
	void calcutae_offset(Mat& A);
	void operator()(Mat& A);

};

////		Алгоритмы вычисления смещений		////

// Вычисление смещения между изображениями методом фазовой корелляции
Point2d calculate_offset_phase(Mat one, Mat two);

// Вычисление смещения между изображениями методом фазовой корелляции с оптимизацией
Point2d calculate_offset_phase_optimized(Mat one, Mat& two);

// Вычисление смещения между изображениями методом фазовой корелляции с оптимизацией и использованием мультимасштабирования
Point2d calculate_offset_phase_optimized_multiscale(Mat one, Mat& two);

////		Алгоритмы сдвигов		////

// Сдвинуть изображение с циклическим обменом строк и стобцов
void move_images_cycle_roll(Mat& one, Point2d offset);

// Сдвинуть изображение с использоваине ROI и вспомогательного изображения
void move_image_roi_alt(Mat& img, Mat& trans, const Point2d& offset);