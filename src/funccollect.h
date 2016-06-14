#ifndef __FUNC_COLLECT_H__
#define __FUNC_COLLECT_H__

#include <algorithm>
#include <stdlib.h>
#include <opencv2/opencv.hpp>

#include <math.h>
#include <time.h>

#include "vector2f.h"

#define OPEN_IMAGE(img,name) \
	do { \
		char buf[256];\
		sprintf(buf, "F:/Image/Src/%s",name);\
		sprintf(buf, "../image_example/%s",name);\
		##img = cvLoadImage(buf); \
	}while(0);
#define SAVE_IMAGE(img,name) \
	do { \
		char buf[256]; \
		sprintf(buf, "F:/Image/Dst/%s",name);\
		sprintf(buf, "../result_example/%s",name);\
		cvSaveImage(buf , ##img); \
	}while(0);


void semi_main(IplImage *img);

#endif