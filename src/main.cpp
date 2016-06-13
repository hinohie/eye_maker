#include "funccollect.h"

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <time.h>

using namespace cv;

//char file_name[CHAR_MAX]="BT8SOVuCYAAhxhz.jpg";
//char file_name[CHAR_MAX] = "dopdopdopdop.jpg";
char file_name[CHAR_MAX] = "IMG_20140809_214633.jpg";

int main(int argc, char *argv[])
{	
	IplImage *img2;
	OPEN_IMAGE(img2, file_name);

	CvSize size2 = cvGetSize(img2);
	IplImage *img = cvCreateImage(cvSize(700, 700 * size2.height / size2.width), 8, 3);
	cvResize(img2, img);
	cvReleaseImage(&img2);
	
	semi_main(img);
	/*
	cvShowImage("HelloCV", img);

	cvWaitKey();
	*/
	cvReleaseImage(&img);
	return 0;
}
