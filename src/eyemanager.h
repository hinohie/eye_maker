#ifndef __EYE_MANAGER_H__
#define __EYE_MANAGER_H__

#include <algorithm>
#include <stdlib.h>
#include <opencv2/opencv.hpp>

#include <math.h>
#include <time.h>

#include "vector2f.h"

class EyeManager{
public:
	vector<vector2f> dst;
	vector<vector2f> src;
	vector<CvScalar> src_color;
	vector<vector2f> d;
	vector3f sal_col;
	vector2f dir;

	IplImage *eye;
	vector<vector2d> eye_vec;
	double build_range;

	EyeManager(){}
	EyeManager(IplImage *eye_dst, IplImage *eye_src, IplImage *eye, vector<vector2d> &_eye_vec,IplImage *img, vector2f _dir, double _build_range);

	void build();
	void update_image(IplImage *img,double t);
	vector<int> *ijk;
	bool ijk_init;
};

#endif