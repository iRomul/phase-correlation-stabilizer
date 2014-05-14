#pragma once
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

////		Алгоритмы вычисления смещений		////

// Вычисление смещения между изображениями методом фазовой корелляции
Point2d calculate_offset_phase(Mat one, Mat two);

////		Алгоритмы сдвигов		////

// Сдвинуть изображение с циклическим обменом строк и стобцов
void move_images_cycle_roll(Mat& one, Point2d offset);