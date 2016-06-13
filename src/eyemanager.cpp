#include "eyemanager.h"

EyeManager::EyeManager(IplImage *eye_dst, IplImage *eye_src, IplImage *_eye, vector<vector2d> &_eye_vec,IplImage *img, vector2f _dir)
{
	int i, j;
	int height = cvGetSize(eye_dst).height;
	int width = cvGetSize(eye_dst).width;

	dst.clear();
	src.clear();
	src_color.clear();
	d.clear();
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
		{
			if(cvGet2D(eye_dst,i,j).val[0] > 200)
			{
				dst.push_back(vector2f(i,j));
			}
			if(cvGet2D(eye_src,i,j).val[0] > 200)
			{
				src.push_back(vector2f(i,j));
				src_color.push_back(cvGet2D(img,i,j));
				d.push_back(vector2f());
			}
		}
	dir = _dir;

	eye = cvCreateImage(cvGetSize(_eye), 8, 1);
	cvCopy(_eye, eye);
	for(i=0;i<_eye_vec.size(); i++)
		eye_vec.push_back(_eye_vec[i]);

	build();
	ijk_init = false;
}

typedef pair<double, double> pdd;
#include<deque>
void EyeManager::build()
{
	int i, j;
	vector<pdd> dst_v;
	vector<pair<pdd,int> > src_v;
	double dst_min_x,dst_max_x;
	double src_min_x,src_max_x;

	vector2f pir;
	pir.x = dir.y;
	pir.y = -dir.x;

	for(i=0;i<dst.size();i++)
	{
		double tx,ty;
		ty = dst[i]*dir;
		tx = dst[i]*pir;
		dst_v.push_back(pdd(tx,ty));
		if(i==0){dst_min_x = dst_max_x = tx;}
		else{dst_min_x = min(dst_min_x,tx);
		dst_max_x = max(dst_max_x,tx);
		}
	}
	for(i=0;i<src.size();i++)
	{
		double tx,ty;
		ty = src[i]*dir;
		tx = src[i]*pir;
		src_v.push_back(pair<pdd,int>(pdd(tx,ty), i));
		if(i==0){src_min_x = src_max_x = tx;}
		else{src_min_x = min(src_min_x,tx);
		src_max_x = max(src_max_x,tx);
		}
	}

	for(i=0;i<dst_v.size();i++)
	{
		dst_v[i].first = (dst_v[i].first - dst_min_x) / (dst_max_x - dst_min_x);
	}
	for(i=0;i<src_v.size();i++)
	{
		src_v[i].first.first = (src_v[i].first.first - src_min_x) / (src_max_x - src_min_x);
	}
	sort(dst_v.begin(), dst_v.end());
	sort(src_v.begin(), src_v.end());

	deque<pdd> ddq_max;
	deque<pdd> ddq_min;
	deque<pdd> sdq_max;
	deque<pdd> sdq_min;

	double *ds_min = new double[src.size()];
	double *ds_max = new double[src.size()];
	double *sr_min = new double[src.size()];
	double *sr_max = new double[src.size()];

	double range = 0.05;

	// make min_max range

	for(i=0;i<dst.size();i++)
	{
		if(dst_v[i].first > range) break;
		while(!ddq_max.empty() && ddq_max.back().second < dst_v[i].second) ddq_max.pop_back();
		ddq_max.push_back(dst_v[i]);
		while(!ddq_min.empty() && ddq_min.back().second > dst_v[i].second) ddq_min.pop_back();
		ddq_min.push_back(dst_v[i]);
	}
	int cur_di = i;
	for(i=0;i<src.size();i++)
	{
		if(src_v[i].first.first > range) break;
		while(!sdq_max.empty() && sdq_max.back().second < src_v[i].first.second) sdq_max.pop_back();
		sdq_max.push_back(src_v[i].first);
		while(!sdq_min.empty() && sdq_min.back().second > src_v[i].first.second) sdq_min.pop_back();
		sdq_min.push_back(src_v[i].first);
	}
	int cur_si = i;
	sr_min[0] = sdq_min.front().second;
	sr_max[0] = sdq_max.front().second;
	ds_min[0] = ddq_min.front().second;
	ds_max[0] = ddq_max.front().second;
	int min_count = 3;
	for(i=1;i<src.size();i++)
	{
		while(cur_di < dst.size() && dst_v[cur_di].first < range + src_v[i].first.first)
		{
		while(!ddq_max.empty() && ddq_max.back().second < dst_v[cur_di].second) ddq_max.pop_back();
		ddq_max.push_back(dst_v[cur_di]);
		while(!ddq_min.empty() && ddq_min.back().second > dst_v[cur_di].second) ddq_min.pop_back();
		ddq_min.push_back(dst_v[cur_di]);
		cur_di++;
		}
		while (ddq_max.size()>min_count && ddq_max.front().first < src_v[i].first.first - range) ddq_max.pop_front();
		while (ddq_min.size()>min_count && ddq_min.front().first < src_v[i].first.first - range) ddq_min.pop_front();
		while(cur_si < src.size() && src_v[cur_si].first.first < range + src_v[i].first.first)
		{
		while(!sdq_max.empty() && sdq_max.back().second < src_v[cur_si].first.second) sdq_max.pop_back();
		sdq_max.push_back(src_v[cur_si].first);
		while(!sdq_min.empty() && sdq_min.back().second > src_v[cur_si].first.second) sdq_min.pop_back();
		sdq_min.push_back(src_v[cur_si].first);
		cur_si++;
		}
		while (sdq_max.size()>min_count && sdq_max.front().first < src_v[i].first.first - range) sdq_max.pop_front();
		while (sdq_min.size()>min_count && sdq_min.front().first < src_v[i].first.first - range) sdq_min.pop_front();

		sr_min[i] = sdq_min.front().second;
		sr_max[i] = sdq_max.front().second;
		ds_min[i] = ddq_min.front().second;
		ds_max[i] = ddq_max.front().second;
//		printf("-- %lf %lf  %d %d\n",src_v[i].first, src_v[i].second,cur_di,cur_si);
//		printf("%lf %lf  %lf %lf\n",sr_min[i], sr_max[i], ds_min[i], ds_max[i]);
	}

	for(i=0;i<src.size();i++)
	{
		double tx = src_v[i].first.first;
		double ty = src_v[i].first.second;
		if(sr_max[i] - sr_min[i] < E) ty = 1;
		else ty = (ty - sr_min[i]) / (sr_max[i] - sr_min[i]);

		tx = tx*(dst_max_x - dst_min_x) + dst_min_x;
		ty = ty*(ds_max[i] - ds_min[i]) + ds_min[i];

		vector2f tmp;
		tmp = pir*tx + dir*ty;
		d[src_v[i].second] = tmp - src[src_v[i].second];
	}

//	printf("%lf %lf  %lf %lf\n",dst_min_x, dst_max_x, src_min_x, src_max_x);

}
void EyeManager::update_image(IplImage *img,double t)
{
	int i, j, k;
	IplImage *color = cvCreateImage(cvGetSize(img),8, 3);
	cvSet(color, cvScalar(0, 255, 0));
//	cvCopy(img, color, eye);

	int height = cvGetSize(img).height;
	int width = cvGetSize(img).width;
	/*
	for(i=0;i<src.size();i++)
	{
		vector2f tv = d[i]*t + src[i];
		int i0, j0, i1, j1;
		i0 = (int)tv.x;	i1 = i0 + 1;
		j0 = (int)tv.y;	j1 = j0 + 1;
		if(i0<0)i0=0;if(i0>=height)i0=height-1;
		if(i1<0)i1=0;if(i1>=height)i1=height-1;
		if(j0<0)j0=0;if(j0>=width)j0=width-1;
		if(j1<0)j1=0;if(j1>=width)j1=width-1;

		double pp,pq,qp,qq;
		pp = (tv.x - i0);
		pq = -(tv.x - i1);
		qp = (tv.y - j0);
		qq = -(tv.y - j1);

		vector3f col = vector3f(src_color[i].val[0],src_color[i].val[1],src_color[i].val[2]);
		a[i0][j0] += pq*qq*col;
		a[i0][j1] += pq*qp*col;
		a[i1][j0] += pp*qq*col;
		a[i1][j1] += pp*qp*col;
		b[i0][j0] += 1;
		b[i0][j1] += 1;
		b[i1][j0] += 1;
		b[i1][j1] += 1;
	}
	int cnt=0;
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
		{
			if(b[i][j]>0.5)
				cnt++;
		}
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
		{
			a[i][j] *= (double)cnt/src.size();
			if(b[i][j]>0.5)
			cvSet2D(color,i, j, cvScalar(a[i][j].x, a[i][j].y, a[i][j].z));
		}
	*/
	if(!ijk_init)
	{
		ijk = new vector<int>[eye_vec.size()];
		for(int ii=0;ii<eye_vec.size();ii++)
		{
				i = eye_vec[ii].x;
				j = eye_vec[ii].y;
				for(k=0; k<src.size();k++)
				{
					vector2f e = vector2f(i - src[k].x, j - src[k].y);
					if(abs(e.x*d[k].y - e.y*d[k].x)/(d[k].dist()+0.001) < 0.1 * (e.dist() + 0.001))
						ijk[ii].push_back(k);
				}
		}
		ijk_init = true;
	}

	for(int ii=0;ii<eye_vec.size();ii++)
			{
				i = eye_vec[ii].x;
				j = eye_vec[ii].y;
				bool flag = false;
				double b;
				vector3f a;
				b=0;
				for(int kk=0; kk<ijk[ii].size();kk++)
				{
					k = ijk[ii][kk];
					vector2f e = vector2f(i - src[k].x, j - src[k].y);
					double dt = e*d[k]/d[k].dist();
					if(dt < t * d[k].dist())
					{
					}
					else// if(dt <= d[k].dist())
					{
						{
							flag = true;
							if(dt < (t)*d[k].dist() + 1.5)
							{
								vector2f tv = d[k]*t + src[k];
								vector3f col = vector3f(src_color[k].val[0],src_color[k].val[1],src_color[k].val[2]);
//								printf("%d %d %d  %lf %lf %lf\n",i,j,k, col.x, col.y, col.y);
								tv.x -= i;
								tv.y -= j;
								a += col / (tv.dist() + 0.001); 
								b += 1/(tv.dist()+0.001);
							}
						}
					}
				}
				if(flag)
				{
					cvSet2D(color, i, j, cvGet2D(img,i,j));
				}
				else	// sal
				{
		// 			printf("%lf %lf %lf\n",sal_col.x, sal_col.y, sal_col.z);
					cvSet2D(color, i, j,  cvScalar(sal_col.x,sal_col.y,sal_col.z));
				}
				if(b > E) // nun
				{
					a /= b;
					cvSet2D(color, i, j,  cvScalar(a.x,a.y,a.z));
				}
			}

	cvCopy(color, img, eye);
	cvReleaseImage(&color);
}