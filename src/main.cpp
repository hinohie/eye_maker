#include "funccollect.h"

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <time.h>
#include <string.h>
#include <string>

using namespace cv;

static struct AA {
	string filename = "F:/Image/Src/dopdopdopdop.jpg";
	int t_width = 0;
	int t_height = 0;
	double cursor_size = 5;
	double build_range = 0.05;
};

AA parsing(char *filename)
{
	AA me;
	FILE *fp = fopen(filename, "rt");

	if (fp == NULL){ printf("Load parameter fail : file is not exist\n"); return me; }

	char line[257];
	char command[257];
	char value[257];
	while (fgets(line, 256, fp))
	{
		int i, j, k;
		for (i = 0; line[i]; i++)
		{
			if (line[i] == '/')
				if (line[i + 1] == '/')
				{
					// delete comments
					line[i] = 0;
					break;
				}
		}
		if (sscanf(line, "%s : %s", command, value) != 2) continue;
		for (i = 0; command[i]; i++)if (command[i] >= 'A' && command[i] <= 'Z') command[i] = command[i] - 'A' + 'a';
		if (strcmp(command, "image_filepath") == 0)
		{
			me.filename = value;
		}
		if (strcmp(command, "image_width") == 0)
		{
			me.t_width = 0;
			for (j = 0; value[j]; j++)
			{
				if (value[j] <'0' || value[j] >'9') break;
				me.t_width *= 10;
				me.t_width += value[j] - '0';
			}
		}
		if (strcmp(command, "image_height") == 0)
		{
			me.t_height = 0;
			for (j = 0; value[j]; j++)
			{
				if (value[j] <'0' || value[j] >'9') break;
				me.t_height *= 10;
				me.t_height += value[j] - '0';
			}
		}
		if (strcmp(command, "cursor_size") == 0)
		{
			me.cursor_size = 0;
			int point_size = 0;
			bool point_apear = false;
			for (j = 0; value[j]; j++)
			{
				if (value[j] == '.') { point_apear = true; continue; }
				if (value[j] <'0' || value[j] >'9') break;
				if (point_apear) point_size++;
				me.cursor_size *= 10;
				me.cursor_size += value[j] - '0';
			}
			while (point_size--)me.cursor_size /= 10;
		}
		if (strcmp(command, "build_range") == 0)
		{
			me.build_range = 0;
			int point_size = 0;
			bool point_apear = false;
			for (j = 0; value[j]; j++)
			{
				if (value[j] == '.') { point_apear = true; continue; }
				if (value[j] <'0' || value[j] >'9') break;
				if (point_apear) point_size++;
				me.build_range *= 10;
				me.build_range += value[j] - '0';
			}
			while (point_size--)me.build_range /= 10;
		}

	}
	return me;
}

int main(int argc, char *argv[])
{	
	if (argc <= 1)
	{
		printf("Input parameter please\n");
		return 0;
	}

	AA tt = parsing(argv[1]);

	IplImage *img2;
	img2 = cvLoadImage(tt.filename.c_str());

	CvSize size2 = cvGetSize(img2);
	CvSize size;
	if (tt.t_height == 0 && tt.t_width == 0)
	{
		size = size2;
	}
	else {
		if (tt.t_height == 0)
			size.height = tt.t_width * size2.height / size2.width;
		else
			size.height = tt.t_height;
		if (tt.t_width == 0)
			size.width = tt.t_height * size2.width / size2.height;
		else
			size.width = tt.t_width;

		printf("%d %d\n", size.width, size.height);
	}
	IplImage *img = cvCreateImage(size, 8, 3);
	cvResize(img2, img);
	cvReleaseImage(&img2);
	
	semi_main(img, tt.cursor_size, tt.build_range);
	/*
	cvShowImage("HelloCV", img);

	cvWaitKey();
	*/
	cvReleaseImage(&img);
	return 0;
}
