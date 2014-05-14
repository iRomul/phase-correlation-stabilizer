#include "algos.h"
#include "utility.h"

Point2d calculate_offset_phase(Mat one, Mat two) {

	cvtColor(one, one, CV_BGR2GRAY);
	cvtColor(two, two, CV_BGR2GRAY);
	one.convertTo(one, CV_64F);
	two.convertTo(two, CV_64F);
	return phaseCorrelate(one, two);

}

void move_images_cycle_roll(Mat& one, Point2d offset) {

	for(register int i = 0; i < one.rows; i++) {

		for(register int j = 0; j < one.cols; j++) {

			one.at<Vec3b>(i, j) = one.at<Vec3b>(mod_custom(i+offset.y, one.rows-1), mod_custom(j+offset.x, one.cols-1));

		}

	}

}