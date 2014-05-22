#pragma once
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

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