/*
 * =====================================================================================
 *
 *       Filename:  main_eccv_data.cpp
 *
 *    Description:  plays back depth mocap data for ECCV 2012
 *        Version:  1.0
 *        Created:  11/10/2012
 *         Author:  Varun Ganapathi (varung@gmail.com), 
 *
 *
 *         USAGE:   pass prefix of data files, e.g., seq_a, and it will play back all 
 *                  items in that sequences, i.e., seq_a_0.bin, seq_1.bin, and so on.
 *
 *                  on ubuntu:
 *         COMPILE: g++ main_eccv_data.cpp -lglut
 * =====================================================================================
 */
#include <Windows.h>
#include <vector>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

#ifdef __APPLE__ & __MACH__
#define OSX
#endif

#ifdef OSX
#include <GLUT/glut.h>
#else
// NOTE: you can change this to the standard GL headers if you don't have GLEW
//#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
//#include <GL/glut.h>
#endif

#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace std;
using namespace cv;

int             paused = false;
float           amp_lo = 4000;
float           x_angle = 0,
                y_angle = 0;
int             cur_file=0;
char           *data_set_dir = NULL;
extern float    color_map[][3];
extern int      color_map_size;

 int _fid = -1;


//float intrinsic[] = {
//  2.844376,0.000000,0.000000,0.000000,
//  0.000000,2.327216,0.000000,0.000000,
//  0.000000,0.000000,-1.000020,-1.000000,
//  0.000000,0.000000,-0.200002,0.000000 
//};

float intrinsic[] = {
	1.609876,0.000000,0.000000,0.000000,
	0.000000,2.147216,0.000000,0.000000,
	0.000000,0.000000,-1.000000,-1.000000,
	0.000000,0.000000,-1.000000,0.000000 
};


//////////////////////////////////////////////////////////////////////////
void Qsort(float arr[], int low, int high)//快排
{
	if(low >= high)
	{
		return;
	}
	int first = low;
	int last = high;
	float key = arr[first];/*用字表的第一个记录作为枢轴*/

	while(first < last)
	{
		while(first < last && arr[last] >= key)
		{
			--last;
		}

		arr[first] = arr[last];/*将比第一个小的移到低端*/

		while(first < last && arr[first] <= key)
		{
			++first;
		}

		arr[last] = arr[first];    
		/*将比第一个大的移到高端*/
	}
	arr[first] = key;/*枢轴记录到位*/
	Qsort(arr, low, first-1);
	Qsort(arr, first+1, high);
}

//////////////////////////////////////////////////////////////////////////
int _fileIndex = 0;
bool _isSave = true;//true;
bool _drawn = false;

#include "eccv.h"

int win_h() { return glutGet(GLUT_WINDOW_HEIGHT); }
int win_w() { return glutGet(GLUT_WINDOW_WIDTH); }

void reshape( int w, int h ){
  glutPostRedisplay();
}

inline float clamp( float x, float lo, float hi){
  if(x < lo) return lo;
  if(x > hi) return hi;
  return x;
}

Frame frame;

ifstream points_ifs, markers_ifs, joints_ifs; 

void draw( Frame& f ) {

	//////////////////////////////////////////////////////////////////////////
	float maxZ = -10.0f;
	float minZ = 0.0f;
	float delatZ = 0.6f;//0.2f;
	int lowR = 3;//4
	int highR = 3;//4

	for(int i=0; i<f.njoints; ++i) {
		float tmpZ = f.joints[i].z;
		if (tmpZ!=0)
		{
			if (tmpZ>maxZ)
			{
				maxZ = tmpZ;
			}else if (tmpZ<minZ)
			{
				minZ = tmpZ;
			}
		}
	}

	for(int i=0; i<f.nmarkers; ++i) {
		float tmpZ = f.markers[i].z;
		if (tmpZ!=0)
		{
			if (tmpZ>maxZ)
			{
				maxZ = tmpZ;
			}else if (tmpZ<minZ)
			{
				minZ = tmpZ;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float bandZMid[R] = {0};

	for (int i=0;i<R;i++)
	{
		float tmpZ[C]={0};
		int countZ = 0;

		for (int j=0;j<C;j++)
		{
			if (frame.points[i*C+j][2])
			{
				tmpZ[j] = frame.points[i*C+j][2];
				countZ++;
			}
			
		}

		Qsort(tmpZ,0,countZ);
		bandZMid[i] = tmpZ[countZ/2];
	}



	//for (int i=0;i<NPOINTS;i++)
	//{
	//	if (frame.points[i][2]!=0)
	//	{
	//		bandZs[i/C] = frame.points[i][2];
	//		//outfile<<i/C<<" "<<i%C<<" "<<frame.points[i][0]<<" "<<frame.points[i][1]<<" "<<-frame.points[i][2]<<endl;		  
	//	}
	//}


	//////////////////////////////////////////////////////////////////////////
	
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glLoadMatrixf(intrinsic);

  // draw sensor
  glEnable(GL_DEPTH_TEST);
  glPointSize(1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glTranslatef(0,0,-2);
  glRotatef(y_angle, 0, 1.0, 0);
  glRotatef(x_angle, 1.0, 0, 0);
  glTranslatef(0,0,2);

  glColor3f( .7, .3, .3 );
  glBegin(GL_POINTS);

  bool flag[NPOINTS] = {false};
  for (int i=0; i<NPOINTS; ++i) {
	  if (f.points[i][2]<minZ-delatZ || f.points[i][2]>maxZ+delatZ
		  //|| i%(C)!=80
		  )
		  //|| (f.points[i][2]<))
		  //if (f.points[i][2]<-3.5 || f.points[i][2]>-3)
	  {
		  continue;
	  }
	  
	  float maxBandZ = -100;
	  float minBandZ = 0;
	  for (int j=0;j<lowR+highR;j++)
	  {
		  float tmpBandZ = bandZMid[i/C-lowR+j];
		  if (tmpBandZ!=0)
		  {
			  if (maxBandZ < tmpBandZ)//may out array bound
			  {
				  maxBandZ = tmpBandZ;
			  }

			  if (minBandZ > tmpBandZ)//may out array bound
			  {
				  minBandZ = tmpBandZ;
			  }
		  }
	  }
	  
	  if (i/C>170 && ((f.points[i][2]>=minBandZ && f.points[i][2]<=maxBandZ) || f.points[i][2]<-3.82))
	  {
		  continue;
	  }

	  //if (i/C>180 && f.points[i][2]>bandZMid[i/C-2] && f.points[i][2]<bandZMid[i/C+4])
	  //{
		 // continue;
	  //}

	  
	  flag[i] = true;
  }

  int bbox = 5;
  int deltaCeil = 8;
  for (int i=0; i<NPOINTS; ++i)
  {
	  if (flag[i] && i/C>180)
	  {
		  int countCeil = 0;
		  for (int j=0;j<bbox;j++)
		  {
			  if (j==bbox/2)//同一行不检测
			  {
				  continue;
			  }
		  
			  for (int k=0;k<bbox;k++)
			  {
				  int index = i + (j-bbox/2)*C + (k-bbox/2);

				  if (index>=0 && index<NPOINTS)
				  {
					  if (flag[index])
					  {
						  countCeil++;
					  }
				  
				  } 
			  }
		  }

		  if (countCeil<deltaCeil)
		  {
			  flag[i] = false;
		  }
	  }
  }

  for (int i=0; i<NPOINTS; ++i)
  {
	  float x1=i%C-C/2;
	  float y1=i/C-R/2;
	  y1 = y1*C/R;
	  if(x1*x1+y1*y1>C*C/4+50 
		  || i%C<45 
		  || i%C>win_w()-30 
		  || (i/C>win_h()-70 && i%C<70) 
		  || (i/C<70 && i%C<70)
		  )
	  {
		  flag[i] = false;
	  }

	  //if ((i%C>30 && i%C<win_w()-30)
		 // || (i%C<40 && i/C>=80 && i/C<=180)
		 // )
	  //{
	  //}else
	  //{
		 // flag[i] = false;
	  //}


	  if (flag[i])
	  {
		  int index = clamp(-(f.points[i][2] + 3.0), 0.0, 1.0 ) * (color_map_size-1);
		  glColor3fv( color_map[ index ] );
		  //glColor3fv(color_map[ 1 ]);
		  glVertex3fv(f.points[i]);  
	  }

	  
  }

  glEnd();


  // draw markers
  glDisable(GL_DEPTH_TEST);
  for(int i=0; i<f.nmarkers; ++i) {
    glPushMatrix();
    glTranslatef( f.markers[i].x, f.markers[i].y, f.markers[i].z );
    glColor3f(0,0,1);
    glutWireSphere(.01, 10,10); 

    glRasterPos3f( 0, 0, 0 );
#ifndef OSX
    glutBitmapString( GLUT_BITMAP_HELVETICA_18, (const unsigned char*)f.markers[i].name ); // requires openglut or freeglut
#endif
    glPopMatrix();
  }
  
  for(int i=0; i<f.njoints; ++i) {
    glPushMatrix();
    glTranslatef( f.joints[i].x, f.joints[i].y, f.joints[i].z );
    glColor3f(1,0,0);
    glutWireSphere(.01, 10,10); 

    glColor3f( 0, 0, 0 );
    glRasterPos3f( 0, 0, 0 );
    char buf[255];
    sprintf(buf, "%d", i);
#ifndef OSX
    glutBitmapString( GLUT_BITMAP_HELVETICA_18, (const unsigned char*)buf ); // requires openglut or freeglut
#endif
	  glPopMatrix();
  }

  //////////////////////////////////////////////////////////////////////////
  if (!_drawn && _isSave)
  {
	  //points
	  cv::Mat depthMat;
	  depthMat.create(R,C,CV_32FC1);

	  char fileName[1024] = {0};
	  sprintf(fileName,"depth/%s%d_%d.txt","org",_fid,_fileIndex);
	 // ofstream outfile(fileName);
	  for (int i=0; i<NPOINTS; ++i)
	  {
		  if (flag[i])
		  {
			  //if ((i%C>30 && i%C<win_w()-30)
				 // || (i%C<40 && i/C>=80 && i/C<=180)
				 // )
			  {
				  //outfile<<i%C<<" "<<i/C<<" "<<f.points[i][0]<<" "<<f.points[i][1]<<" "<<-f.points[i][2]<<endl;
				  depthMat.at<float>(i/C,i%C) = -f.points[i][2];
			  }
		  }
	  }

	  ////////////////
//	  cv::imshow("test",depthMat);
	
	  //cv::Mat bwImg; 
	  //vector<vector<cv::Point>> contours ; 
	  //// 二值化图像  
	  //cv::threshold(depthMat, bwImg, 0.0, 255.0, CV_THRESH_BINARY | CV_THRESH_OTSU);  

	  //cv::imshow("binary image", bwImg);  
	  //cv::waitKey();  

	  //// 查找轮廓，对应连通域  
	  //cv::findContours(bwImg,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);  

	  //// 寻找最大连通域  
	  //double maxArea = 0;  
	  //vector<cv::Point> maxContour;  
	  //for(size_t i = 0; i < contours.size(); i++)  
	  //{  
		 // double area = cv::contourArea(contours[i]);  
		 // if (area > maxArea)  
		 // {  
			//  maxArea = area;  
			//  maxContour = contours[i];  
		 // }  
	  //}  

	  //// 将轮廓转为矩形框  
	  //cv::Rect maxRect = cv::boundingRect(maxContour);  

	  //// 显示连通域  
	  //cv::Mat result1, result2;  

	  //bwImg.copyTo(result1);  
	  //bwImg.copyTo(result2);  

	  //for (size_t i = 0; i < contours.size(); i++)  
	  //{  
		 // cv::Rect r = cv::boundingRect(contours[i]);  
		 // cv::rectangle(result1, r, cv::Scalar(255));  
	  //}  
	  //cv::imshow("all regions", result1) ;  

	  //cv::rectangle(result2, maxRect, cv::Scalar(255));  
	  //cv::imshow("largest region", result2) ;  
	  //////////////////////////////////////////////////////////////////////////
	  //joints
	  GLdouble winx=0,winy=0,winz=0;
	  GLdouble modelMatrix[16];
	  GLdouble projMatrix[16];
	  GLint viewPoint[4];

	  glGetIntegerv(GL_VIEWPORT, viewPoint);
	  glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	  glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);

	  cv::Mat  imgMat;
	  imgMat.create(win_h(),win_w(),CV_32FC1);

	  sprintf(fileName,"depth/%s%d_%d.txt","depth",_fid,_fileIndex);
	  ofstream outfile3(fileName);
	  for (int i=0; i<NPOINTS; ++i)
	  {
		  if (flag[i])
		  {
			  gluProject(f.points[i][0],f.points[i][1],f.points[i][2],modelMatrix,projMatrix,viewPoint,&winx,&winy,&winz);

			  if (winx>20 && winx<win_w()-20)
			  {
				  imgMat.at<float>(win_h()-winy,winx) = -f.points[i][2];
				  outfile3<<(int)winx<<" "<<(int)winy<<" "<<f.points[i][0]<<" "<<f.points[i][1]<<" "<<-f.points[i][2]<<endl;
			  }
		  }
	  }
	  outfile3.close();

	  //char fileName[1024] = {0};
	  sprintf(fileName,"joints/%s%d_%d.txt","joint",_fid,_fileIndex);
	  ofstream outfile2(fileName);
	  for (int i=0;i<32;i++)
	  {
		  if (frame.joints[i].z!=0)
		  {
			  gluProject(frame.joints[i].x,frame.joints[i].y,frame.joints[i].z,modelMatrix,projMatrix,viewPoint,&winx,&winy,&winz);


			  //imgMat.at<float>(winy,winx) = -frame.joints[i].z;
			  //if (i==1)
			  cv::circle(imgMat,cv::Point(winx,win_h()-winy),10,cv::Scalar(128,128,128));
			  outfile2<<(int)winx<<" "<<(int)winy<<" "<<frame.joints[i].x<<" "<<frame.joints[i].y<<" "<<-frame.joints[i].z<<endl;
		  }
	  }
	  outfile2.close();


	  sprintf(fileName,"joints/%s%d_%d.txt","marker",_fid,_fileIndex);
	  ofstream outfile4(fileName);
	  for (int i=0;i<32;i++)
	  {
		  if (frame.markers[i].z!=0)
		  {
			  gluProject(frame.markers[i].x,frame.markers[i].y,frame.markers[i].z,modelMatrix,projMatrix,viewPoint,&winx,&winy,&winz);


			  //imgMat.at<float>(winy,winx) = -frame.joints[i].z;
			  //cv::circle(imgMat,cv::Point(winx,win_h()-winy),10,cv::Scalar(128,128,128));
			  outfile4<<(int)winx<<" "<<(int)winy<<" "<<frame.markers[i].x<<" "<<frame.markers[i].y<<" "<<-frame.markers[i].z<<" "<<frame.markers[i].name<<endl;
		  }
	  }
	  outfile4.close();


	  _fileIndex++;
	  _drawn = true;

//	  cv::imshow("img",imgMat);
  }

}

bool read_frame() {
  points_ifs.read( (char*)&frame, sizeof(frame) );

  //////////////////////////////////////////////////////////////////////////
  //if (_isSave)
  //{
	 // char fileName[1024] = {0};
	 // sprintf(fileName,"depth/%s_%d.txt","o_depth",_fileIndex);
	 // ofstream outfile(fileName);
	 // for (int i=0;i<NPOINTS;i++)
	 // {
		//  if (frame.points[i][2]!=0)
		//  {
		//	  outfile<<i/C<<" "<<i%C<<" "<<frame.points[i][0]<<" "<<frame.points[i][1]<<" "<<-frame.points[i][2]<<endl;		  
		//  }
	 // }
	 // outfile.close();

  //}
	_drawn = false;
  //////////////////////////////////////////////////////////////////////////
  

  // open the next one
  if(!points_ifs) {
    points_ifs.clear();
    return false;
  }
  return true;
}

void next_file() {
  //static int id = -1;
  ++_fid;
  char buf[1024];
  sprintf(buf, "%s_%d.bin", data_set_dir, _fid);
  printf("trying to open: %s\n",buf);
  points_ifs.clear();
  if(points_ifs.is_open()) points_ifs.close();
  
  points_ifs.open(buf, ifstream::binary);
  _fileIndex=0;
  if(!points_ifs) {
    printf("ERROR: exiting. could not open: %s\n", buf);
    exit(-1);
  }
  read_frame();
}

void idle() {
  if(!paused) {
    if(!read_frame()) {  next_file(); }
  }
  glutPostRedisplay(); 
}

void rewind_one() {
  points_ifs.clear();
  long off = sizeof(Frame); off *= -2;
  points_ifs.seekg( off, ifstream::cur );
  if(!points_ifs){
    points_ifs.clear();
    points_ifs.seekg(0, ios_base::beg);
  }
  if(!read_frame()){
    printf("failed to read file\n");
  }
}

void key(unsigned char k, int x, int y) {
  switch(k) {
    case 'q':
    case 27: 
      //glutLeaveMainLoop(); 
      exit(0);
      break;
    case 'f':
      if(!read_frame()) next_file();
      break;
    case 'b':
      rewind_one();
      break;

	case 'p': paused = !paused; break;
	case 's': _isSave = !_isSave; break;
    case 'x': x_angle += .5; break;
    case 'X': x_angle -= .5; break;
    case 'y': y_angle += .5; break;
    case 'Y': y_angle -= .5; break;
  }
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 
  glViewport(0,0,win_w(),win_h());
  draw(frame);
  glutSwapBuffers();
}

int main2(int argc, char* argv[]) 
{
  // INIT
  glutInit(&argc,argv);
  glutInitDisplayMode(  GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
  glutCreateWindow("TOF MOCAP PLAYER-Varun Ganapathi"); 

  // IF YOU HAVE GLEW
  // glewInit();

  if(argc < 2){
    printf("please specify file prefix for the sequences containing Stanford TOF MOCAP data (e.g., 'seq_a' to play 'seq_a_0.bin', 'seq_a_1.bin', etc.\n");
    return -1;
  }
  data_set_dir = argv[1];
  printf("Key commands: \n");
  printf("q : quit \n");
  printf("p : pause \n");
  printf("f : next frame\n");
  printf("b : previous frame\n");
  printf("xXyY: rotate about axis\n");

  //glutReshapeWindow(512, 512);
  glutReshapeWindow(C, R);
  glutDisplayFunc(display);
  //glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(key);
  glutIdleFunc(idle);
  glClearColor(1,1,1,1);
  next_file();
  glutMainLoop();

  system("PAUSE");
  return 0;
}

float color_map[][3] =  {
  0 ,       0 ,  0.5625,
  0 ,       0 ,  0.6250,
  0 ,       0 ,  0.6875,
  0 ,       0 ,  0.7500,
  0 ,       0 ,  0.8125,
  0 ,       0 ,  0.8750,
  0 ,       0 ,  0.9375,
  0 ,       0 ,  1.0000,
  0 ,  0.0625 ,  1.0000,
  0 ,  0.1250 ,  1.0000,
  0 ,  0.1875 ,  1.0000,
  0 ,  0.2500 ,  1.0000,
  0 ,  0.3125 ,  1.0000,
  0 ,  0.3750 ,  1.0000,
  0 ,  0.4375 ,  1.0000,
  0 ,  0.5000 ,  1.0000,
  0 ,  0.5625 ,  1.0000,
  0 ,  0.6250 ,  1.0000,
  0 ,  0.6875 ,  1.0000,
  0 ,  0.7500 ,  1.0000,
  0 ,  0.8125 ,  1.0000,
  0 ,  0.8750 ,  1.0000,
  0 ,  0.9375 ,  1.0000,
  0 ,  1.0000 ,  1.0000,
  0.0625 ,  1.0000 ,  0.9375,
  0.1250 ,  1.0000 ,  0.8750,
  0.1875 ,  1.0000 ,  0.8125,
  0.2500 ,  1.0000 ,  0.7500,
  0.3125 ,  1.0000 ,  0.6875,
  0.3750 ,  1.0000 ,  0.6250,
  0.4375 ,  1.0000 ,  0.5625,
  0.5000 ,  1.0000 ,  0.5000,
  0.5625 ,  1.0000 ,  0.4375,
  0.6250 ,  1.0000 ,  0.3750,
  0.6875 ,  1.0000 ,  0.3125,
  0.7500 ,  1.0000 ,  0.2500,
  0.8125 ,  1.0000 ,  0.1875,
  0.8750 ,  1.0000 ,  0.1250,
  0.9375 ,  1.0000 ,  0.0625,
  1.0000 ,  1.0000 ,       0,
  1.0000 ,  0.9375 ,       0,
  1.0000 ,  0.8750 ,       0,
  1.0000 ,  0.8125 ,       0,
  1.0000 ,  0.7500 ,       0,
  1.0000 ,  0.6875 ,       0,
  1.0000 ,  0.6250 ,       0,
  1.0000 ,  0.5625 ,       0,
  1.0000 ,  0.5000 ,       0,
  1.0000 ,  0.4375 ,       0,
  1.0000 ,  0.3750 ,       0,
  1.0000 ,  0.3125 ,       0,
  1.0000 ,  0.2500 ,       0,
  1.0000 ,  0.1875 ,       0,
  1.0000 ,  0.1250 ,       0,
  1.0000 ,  0.0625 ,       0,
  1.0000 ,       0 ,       0,
  0.9375 ,       0 ,       0,
  0.8750 ,       0 ,       0,
  0.8125 ,       0 ,       0,
  0.7500 ,       0 ,       0,
  0.6875 ,       0 ,       0,
  0.6250 ,       0 ,       0,
  0.5625 ,       0 ,       0,
  0.5000 ,       0 ,       0
};

int color_map_size = sizeof(color_map)/( sizeof(color_map[0]));
