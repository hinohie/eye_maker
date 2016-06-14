#include "funccollect.h"

#include "eyemanager.h"

class GCApplication
{
public:
	double thickness;
	double pre_thickness;
	double build_range;
	double pre_build_range;

	void reset();
	void clear();
	void process();
	void setImageAndWinName(IplImage *_image, string _winName, double _pre_thickness, double _build_range);
    void showImage();
    void mouseClick( int event, int x, int y, int flags, void* param );
	void maskAdd(int si,int sj);
	int mask_type;

	IplImage *image;
	IplImage *pre_image;
	IplImage *mask;
	IplImage *new_mask;
	IplImage *alpha_mask;
    const string* winName;

	void phase_up();
	void do_flow();
	int stage;
	double tt;
	IplImage *eye;
	IplImage *eye_dst;
	IplImage *eye_src;
	vector2f dir;

	vector<vector2d> eye_vec;
	vector<EyeManager *> manager;
	vector3f find_near_sal();

	int height,width;

	GCApplication(){
		image = NULL;
		mask = NULL;
	}
};

void GCApplication::clear()
{
	cvDestroyWindow(winName->c_str());
	manager.clear();
}
void GCApplication::reset()
{
	int i, j;
	cvCopy(pre_image, image);
	cvSet(mask, cvScalar(0));
	cvSet(new_mask, cvScalar(0));
	mask_type = -1;
	thickness = pre_thickness;
	build_range = pre_build_range;

	cvSet(eye, cvScalar(0));
	cvSet(eye_src, cvScalar(0));
	cvSet(eye_dst, cvScalar(0));

	stage = 0;
	tt=0;
}

void GCApplication::setImageAndWinName( IplImage *_image, string _winName , double _pre_thickness, double _build_range )
{
    if( _image == NULL || _winName.empty() )
        return;
	pre_image = cvCreateImage(cvGetSize(_image),8, 3);
	image = cvCreateImage(cvGetSize(_image),8, 3);
	mask = cvCreateImage(cvGetSize(_image),8,1);
	new_mask = cvCreateImage(cvGetSize(_image),8,1);
	alpha_mask = cvCreateImage(cvGetSize(_image),8,1);

	cvCopy(_image,pre_image);
	cvSet(mask, cvScalar(0));
	cvSet(new_mask, cvScalar(0));
	cvSet(alpha_mask, cvScalar(255));

	height = image->height;
	width = image->width;

	pre_thickness = _pre_thickness;
	pre_build_range = _build_range;
	
	eye = cvCreateImage(cvGetSize(_image),8,1);
	eye_dst = cvCreateImage(cvGetSize(_image),8,1);
	eye_src = cvCreateImage(cvGetSize(_image),8,1);

	cvSet(eye, cvScalar(0));
	cvSet(eye_src, cvScalar(0));
	cvSet(eye_dst, cvScalar(0));


	int i, j;
	for(i=0; i<image->height; i+=2)
		for(j=0;j<image->width; j+=2)
		{
			cvSet2D(alpha_mask,i,j,cvScalar(0));
		}

    winName = new string(_winName);
    reset();
}

void GCApplication::showImage()
{
    if( image==NULL || winName->empty() )
        return;
	IplImage *img = cvCreateImage(cvGetSize(image),8, 3);
	IplImage *color = cvCreateImage(cvGetSize(image),8, 3);
	IplImage *real_mask = cvCreateImage(cvGetSize(image),8,1);
	cvCopy(image,img);
	if(stage == 1)
	{
		cvSet(real_mask, cvScalar(0));
		cvCopy(eye,real_mask);
		cvSet(color,cvScalar(100,100,100));
		cvCopy(color,img,alpha_mask);
		cvCopy(image,img,real_mask);
	}
	else if(stage == 2)
	{
		cvSet(real_mask, cvScalar(0));
		cvCopy(eye,real_mask);
		cvSet(color,cvScalar(100,100,100));
		cvCopy(color,img,alpha_mask);
		cvCopy(image,img,real_mask);

		cvSet(real_mask,cvScalar(0));
		cvCopy(eye_dst, real_mask, alpha_mask);
		cvSet(color, cvScalar(0,255,0));
		cvCopy(color,img,real_mask);
	}
	else if(stage == 3)
	{
		cvSet(real_mask, cvScalar(0));
		cvCopy(eye,real_mask);
		cvSet(color,cvScalar(100,100,100));
		cvCopy(color,img,alpha_mask);
		cvCopy(image,img,real_mask);

		cvSet(real_mask,cvScalar(0));
		cvCopy(eye_dst, real_mask, alpha_mask);
		cvSet(color, cvScalar(0,255,0));
		cvCopy(color,img,real_mask);

		cvSet(real_mask,cvScalar(0));
		cvCopy(eye_src, real_mask, alpha_mask);
		cvSet(color, cvScalar(255,0,255));
		cvCopy(color,img,real_mask);
	}
	else if(stage == 4)
	{
		cvSet(real_mask,cvScalar(0));
		cvCopy(eye_dst, real_mask, alpha_mask);
		cvSet(color, cvScalar(0,255,0));
		cvCopy(color,img,real_mask);

		cvSet(real_mask,cvScalar(0));
		cvCopy(eye_src, real_mask, alpha_mask);
		cvSet(color, cvScalar(255,0,255));
		cvCopy(color,img,real_mask);
	}
	else if(stage == 5)
	{
		double nt = 0;
		nt = abs(sin(tt));
		if(nt > 0.8) nt = 1;
		else if(nt < 0.6) nt = 0;
		else nt = (nt - 0.6)*5;
		/*
		while(tt > 4)tt-=4;
		nt = 1.0 / 4 * (tt-2) * ( tt-2);
		nt = (nt < 0.9 && nt > 0.7)?1:0;
		*/
		for(int i=0; i<manager.size(); i++)
			manager[i]->update_image(img,nt);
	}
	if(stage < 5)
	{
		cvSet(real_mask, cvScalar(0));
		cvCopy(mask,real_mask,alpha_mask);
		cvSet(color,cvScalar(100,99,228));
		cvCopy(color,img,real_mask);

		cvSet(real_mask, cvScalar(0));
		cvCopy(new_mask,real_mask,alpha_mask);
		cvSet(color,cvScalar(228,99,100));
		cvCopy(color,img,real_mask);
	}
    cvShowImage(winName->c_str(),img);
	cvReleaseImage(&color);
	cvReleaseImage(&real_mask);
	cvReleaseImage(&img);
}
void GCApplication::maskAdd(int si, int sj)
{
	int i, j;
	for(i=-thickness; i<=thickness; i++)
	{
		for(j=-thickness; j<=thickness; j++)
		{
			if(si +i >=0 && si + i < mask->height)
				if(sj + j >= 0 && sj + j < mask->width)
			if(i*i + j*j <= thickness*thickness)
			{
				cvSet2D(new_mask, si+i, sj+j, cvScalar(mask_type * 255));
				if(mask_type == 0)
				{
				cvSet2D(mask, si+i, sj+j, cvScalar(mask_type * 255));
				}
			}
		}
	}
}
static vector2d st3_st;
static vector2d st3_ed;
void GCApplication::mouseClick( int even, int x, int y, int flags, void* me)
{
	switch( even )
    {
    case CV_EVENT_LBUTTONDOWN: // set mask(y,x) = 255
        {
			if(stage == 3)
			{
				mask_type = 1;
				st3_st = vector2d(y,x);
			}
			else
			{
				mask_type = 1;
				maskAdd(y,x);
			}
			showImage();
		}
		break;
	case CV_EVENT_RBUTTONDOWN:	// set mask(y,x) = 0
        {
			mask_type = 0;
			maskAdd(y,x);
			showImage();
		}
		break;
    case CV_EVENT_LBUTTONUP:
		{
			maskAdd(y,x);
			mask_type = -1;
			showImage();
		}
		break;
    case CV_EVENT_RBUTTONUP:
		{
			maskAdd(y,x);
			mask_type = -1;
			showImage();
		}
		break;

		
    case CV_EVENT_MOUSEMOVE:
		if(mask_type >= 0)
		{
			if(stage== 3)
			{
				st3_ed = vector2d(y,x);
				cvSet(new_mask,cvScalar(0));
				maskAdd(st3_st.x,st3_st.y);
				maskAdd(st3_ed.x,st3_ed.y);
			}
			else
			{
				maskAdd(y,x);
			}
			showImage();
		}
		break;
	}
}
void GCApplication::process()
{
	if(stage == 0)
	{
	int i, j, k;
	vector<pii> queue;
	int q,r;
	int sq;
	q=r=0;
	IplImage *tmp_mask = cvCreateImage(cvGetSize(image),8,1);
		queue.clear();
		q=r=0;
	cvSet(tmp_mask,cvScalar(255));
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
			if(cvGet2D(new_mask,i,j).val[0] < 200)
			{
				queue.push_back(pii(i,j));
				r++;
				goto ASDFASDFASDF;
			}
ASDFASDFASDF:
		q=0;
	int dx[4] = {0,1,0,-1};
	int dy[4] = {1,0,-1,0};
	sq = r;
	while(q<r)
	{
		int qi, qj;
		int ti, tj;
		qi = queue[q].first;
		qj = queue[q].second;

		q++;
		CvScalar ff = cvGet2D(image,qi,qj);

		for(i=0;i<4;i++)
		{
			ti = qi+dx[i];
			tj = qj+dy[i];
			if(ti>=0 && ti<height)
				if(tj>=0 && tj<width)
					if(cvGet2D(tmp_mask,ti,tj).val[0] > 200 && cvGet2D(mask,ti,tj).val[0] < 200  && cvGet2D(new_mask,ti,tj).val[0] < 200)
					{
						CvScalar gg = cvGet2D(image,ti,tj);
						double xx = 0;
						xx += (ff.val[0] - gg.val[0])*(ff.val[0] - gg.val[0]);
						xx += (ff.val[1] - gg.val[1])*(ff.val[1] - gg.val[1]);
						xx += (ff.val[2] - gg.val[2])*(ff.val[2] - gg.val[2]);
						queue.push_back(pii(ti,tj));
						cvSet2D(tmp_mask,ti,tj,cvScalar(0));
						r++;
					}
		}
	}
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
		{
			CvScalar ff = cvGet2D(tmp_mask,i,j);
			CvScalar gg = cvGet2D(mask,i,j);
			gg.val[0] += ff.val[0];if(gg.val[0] > 255)gg.val[0] = 255;
			gg.val[1] += ff.val[1];if(gg.val[1] > 255)gg.val[1] = 255;
			gg.val[2] += ff.val[2];if(gg.val[2] > 255)gg.val[2] = 255;
			
			cvSet2D(mask,i,j,gg);
		}
	cvSet(new_mask,cvScalar(0));
	cvReleaseImage(&tmp_mask);
	}
	else if(stage == 1)
	{
		int i, j;
		for(i=0;i<height;i++)
			for(j=0;j<width;j++)
			{
				CvScalar ff = cvGet2D(new_mask,i,j);
				CvScalar gg = cvGet2D(mask,i,j);
				if(cvGet2D(eye,i,j).val[0] > 200)
				{
				gg.val[0] += ff.val[0];if(gg.val[0] > 255)gg.val[0] = 255;
				gg.val[1] += ff.val[1];if(gg.val[1] > 255)gg.val[1] = 255;
				gg.val[2] += ff.val[2];if(gg.val[2] > 255)gg.val[2] = 255;
				}
				else
					gg = cvScalar(0,0,0);
				cvSet2D(mask,i,j,gg);
			}
		cvSet(new_mask,cvScalar(0));
	}
	else if(stage == 2)
	{
		/*
	int i, j, k;
	vector<pii> queue;
	int q,r;
	int sq;
	q=r=0;
	double THRESHOLD = 20;
	IplImage *tmp_mask = cvCreateImage(cvGetSize(image),8,1);
	do{
		queue.clear();
		q=r=0;
	cvCopy(new_mask,tmp_mask);
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
		{
			if(cvGet2D(tmp_mask, i, j).val[0] > 200 && cvGet2D(eye,i,j).val[0] > 200)
			{
				queue.push_back(pii(i,j));
				r++;
			}
		}
	int dx[4] = {0,1,0,-1};
	int dy[4] = {1,0,-1,0};
	sq = r;
	while(q<r)
	{
		int qi, qj;
		int ti, tj;
		qi = queue[q].first;
		qj = queue[q].second;

		q++;
		CvScalar ff = cvGet2D(image,qi,qj);

		for(i=0;i<4;i++)
		{
			ti = qi+dx[i];
			tj = qj+dy[i];
			if(ti>=0 && ti<height)
				if(tj>=0 && tj<width)
					if(cvGet2D(tmp_mask,ti,tj).val[0] < 200 && cvGet2D(mask,ti,tj).val[0] < 200 && cvGet2D(eye,ti,tj).val[0] > 200)
					{
						CvScalar gg = cvGet2D(image,ti,tj);
						double xx = 0;
						xx += (ff.val[0] - gg.val[0])*(ff.val[0] - gg.val[0]);
						xx += (ff.val[1] - gg.val[1])*(ff.val[1] - gg.val[1]);
						xx += (ff.val[2] - gg.val[2])*(ff.val[2] - gg.val[2]);
						if(xx < THRESHOLD)
						{
							queue.push_back(pii(ti,tj));
							cvSet2D(tmp_mask,ti,tj,cvScalar(255));
							r++;
						}
					}
		}
	}
	THRESHOLD -= 1;
	printf("%d %d  %lf\n",sq,r,THRESHOLD);
	} while(sq > 0 && r / sq > 1);
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
		{
			CvScalar ff = cvGet2D(tmp_mask,i,j);
			CvScalar gg = cvGet2D(mask,i,j);
			if(cvGet2D(eye,i,j).val[0] > 200)
			{
				gg.val[0] += ff.val[0];if(gg.val[0] > 255)gg.val[0] = 255;
				gg.val[1] += ff.val[1];if(gg.val[1] > 255)gg.val[1] = 255;
				gg.val[2] += ff.val[2];if(gg.val[2] > 255)gg.val[2] = 255;
			}
			else
				gg = cvScalar(0,0,0);
			
			cvSet2D(mask,i,j,gg);
		}
	cvSet(new_mask,cvScalar(0));
	cvReleaseImage(&tmp_mask);
	*/
		
		int i, j;
		for(i=0;i<height;i++)
			for(j=0;j<width;j++)
			{
				CvScalar ff = cvGet2D(new_mask,i,j);
				CvScalar gg = cvGet2D(mask,i,j);
				if(cvGet2D(eye,i,j).val[0] > 200)
				{
				gg.val[0] += ff.val[0];if(gg.val[0] > 255)gg.val[0] = 255;
				gg.val[1] += ff.val[1];if(gg.val[1] > 255)gg.val[1] = 255;
				gg.val[2] += ff.val[2];if(gg.val[2] > 255)gg.val[2] = 255;
				}
				else
					gg = cvScalar(0,0,0);
				cvSet2D(mask,i,j,gg);
			}
		cvSet(new_mask,cvScalar(0));
	}
}
#include<map>
vector3f GCApplication::find_near_sal()
{
	int i, j;
	map<int, int> h;
	h.clear();
	for(int ii=0; ii<eye_vec.size(); ii++)
		{
			i = eye_vec[ii].x;
			j = eye_vec[ii].y;
			int ti, tj;
			for(ti=i-10; ti<=i+10; ti++)
			if(ti>=0 && ti<height)
			for(tj=j-10;tj<=j+10;tj++)
			if(tj>=0 && tj<width)
			if((ti-i)*(ti-i) + (tj-j)*(tj-j) <= 100)
			{
				if(cvGet2D(eye,ti,tj).val[0] < 200)
				{
					int rr,gg,bb, xx;
					CvScalar ff = cvGet2D(image,ti,tj);
					rr = (int)ff.val[0];
					gg = (int)ff.val[1];
					bb = (int)ff.val[2];
					if(rr<0)rr=0;if(rr>=256)rr=255;
					if(gg<0)gg=0;if(gg>=256)gg=255;
					if(bb<0)bb=0;if(bb>=256)bb=255;
					xx = (rr<<16) + (gg<<8) + bb;
					if(h.find(xx) == h.end())
					{
						h[xx] = 1;
					}
					else
					{
						h[xx]++;
					}
				}
			}
		}
	int mm = 0;
	int mx = 0;
	for(map<int, int>::iterator it = h.begin(); it != h.end(); it++)
	{
		int tm,tx;
		tx = it->first;
		tm = it->second;
		int rr,gg,bb;
		rr = tx>>16;
		gg = (tx>>8)%256;
		bb = tx%256;
		for(int ri = rr - 5; ri <= rr+5; ri++)
		if(ri >= 0 && ri < 256)
		for(int gi = gg - 5; gi <= gg+5; gi++)
		if(gi >= 0 && gi < 256)
		if((ri-rr)*(ri-rr) + (gi-gg)*(gi-gg) <= 25)
		for(int bi = bb - 5; bi <= bb+5; bi++)
		if(bi >= 0 && bi < 256)
		if((ri-rr)*(ri-rr) + (gi-gg)*(gi-gg) + (bi-bb)*(bi-bb) <= 25)
		{
			int qx = (ri<<16)+(gi<<8)+bi;
			if(h.find(qx) != h.end())
			{
				tm += h[qx];
			}
		}

		
		if(tm > mm)
		{
			mm = tm;
			mx = tx;
		}
	}
	return vector3f(mx>>16, (mx>>8)%256, mx%256);
}
void GCApplication::phase_up()
{
	printf("%d\n", stage);
	switch(stage)
	{
	case 0:
		cvCopy(mask,eye);
		cvSet(mask,cvScalar(0));
		eye_vec.clear();
		for(int i=0; i<height; i++)
			for(int j=0;j<width; j++)
			{
				if(cvGet2D(eye,i,j).val[0]>200)
					eye_vec.push_back(vector2d(i,j));
			}
		break;
	case 1:
		cvCopy(mask, eye_dst, eye);
		cvSet(mask,cvScalar(0));
		break;
	case 2:
		cvCopy(mask, eye_src, eye);
		cvSet(mask,cvScalar(0));
		break;
	case 3:
		dir = vector2f(st3_ed.x-st3_st.x,st3_ed.y-st3_st.y);
		dir.normalize();
		printf("%lf %lf\n",dir.x,dir.y);
		cvSet(new_mask,cvScalar(0));
		{
		EyeManager *curm = new EyeManager(eye_dst, eye_src,eye,eye_vec,image,dir,build_range);
		curm->sal_col = find_near_sal();
		manager.push_back(curm);
		}
		break;
	case 4:
		stage=-1;
		break;
	}
	stage++;
}
void GCApplication::do_flow()
{
	stage = 5;
}
static void onMouse( int even, int x, int y, int flags, void* param )
{
    ((GCApplication *)param)->mouseClick( even, x, y, flags, param );
}
void semi_main(IplImage *img, double cursor_size, double build_range)
{
	GCApplication gca;
	string win_name = "Active";
	cvNamedWindow(win_name.c_str(), CV_WINDOW_AUTOSIZE);

    cvSetMouseCallback( win_name.c_str(), onMouse, &gca );
	gca.setImageAndWinName(img, win_name, cursor_size, build_range);

	gca.showImage();

	bool work = true;
	int rank = 0;
L:
	gca.tt = 0;
	while(work)
    {
        int c = cvWaitKey(0);
        switch( (char) c )
        {
        case '\x1b':
           printf("exit!\n");
			work = false;
			break;
        case 'r':
            gca.reset();
            gca.showImage();
            break;
        case 'n':
			gca.process();
			rank = 0;
            gca.showImage();
            break;
		case 'a':
			gca.phase_up();
			gca.showImage();
			break;
		case 'g':
			gca.do_flow();
			break;
		case '-':
			gca.stage--;
            gca.showImage();
			break;
        }
		if(gca.stage == 5)
			break;
    }
	double dt = 0.1;
	while(work)
	{
        int c = cvWaitKey(30);
        switch( (char) c )
        {
        case '\x1b':
           printf("exit!\n");
			work = false;
			break;
		case '-':
			gca.stage=0;
            gca.showImage();
			cvSet(gca.eye,cvScalar(0));
			cvSet(gca.eye_src,cvScalar(0));
			cvSet(gca.eye_dst,cvScalar(0));
			goto L;
			break;
		case ' ':
			if(dt > E)
				dt = 0;
			else 
				dt = 0.1;
			break;
		}
		gca.showImage();
		gca.tt += dt;
	}
	gca.clear();
	return;

}