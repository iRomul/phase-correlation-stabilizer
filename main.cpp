#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "image_device.h"
#include "algos.h"
#include "utility.h"
#include "settings.h"

#include <iostream>

using namespace std;
using namespace cv;

typedef Point2d (*Image_offset_calculation)(Mat img1, Mat img2);

int main(int argc, int** argv) {

	// ---- SETTINGS ----

	SteadySettings settings;
	settings.load();
	if(!settings.verify()) {
		cerr << "[ERROR] Work with the current configuration is not possible" << endl;
		exit(EXIT_FAILURE);
	}
	settings.print();

	// ---- DEVICE and VIDEO ----

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

	// ---- ALGORITHMS ----

	Image_offset_calculation calc_offset = &calculate_offset_phase;

	// ---- VARIABLES ----

	namedWindow("Original");

#ifdef _DEBUG
	namedWindow("Graph");
#endif // _DEBUG

	Mat current_frame;
	Mat base_frame;
	Mat graph_frame;
	graph_frame = Mat::zeros(vsize, CV_8UC1);
	Point2d offset;
	bool reset_base_frame = true;

	// ---- MAIN CYCLE ----

	while(true) {

		// 1 - Get frame

		vcap >> current_frame;
#ifdef _DEBUG
		flip(current_frame, current_frame, 1);
#endif

		// 2 - Setting base frame

		if(reset_base_frame) {
			base_frame = current_frame.clone();
			reset_base_frame = false;
		}

		// 3 - Calculating offset

		offset = calc_offset(current_frame, base_frame);
#ifdef _DEBUG
		circle(graph_frame, Point2d(offset.x + vsize.width / 2, offset.y + vsize.height / 2), 3, Scalar(255, 255, 255));
#endif		
		// - Putting image to screen

		imshow("Original", current_frame);
#ifdef _DEBUG
		imshow("Graph", graph_frame);
#endif
		char mode = (char)waitKey(50);
		if(mode == 'e' || mode == 'ó') break;

	} // while

	destroyAllWindows();

	return EXIT_SUCCESS;

}