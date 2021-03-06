#include <iostream>
#include "highgui.h"
#include<windows.h>
#include "slic.h"
#include "ImageShow.h"
#include "Loading.h"
#include "Dijkstra.h"
#include "Cluster.h"
#include "Kmeans.h"
#include <ctime>
#include "Skeleton.h"
#include<winbase.h>
#include<direct.h>
using namespace std;
using namespace cv;

#define HEIGHT 240
#define WIDTH   320
#define NUM_SUPERPIXEL 2500
//callback function  
void mouseEvent(int evt, int x, int y, int flags, void* param)
{
	IplImage* img, *img1;
	img = (IplImage*)param;
	img1 = cvCloneImage(img);

	char  label[128];
	CvFont font, font2;;
	cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1, 1, 0, 1, 1);
	cvInitFont(&font2, CV_FONT_HERSHEY_PLAIN, 1, 1, 0, 1, 1);

	if (evt == CV_EVENT_LBUTTONDOWN){
		//printf("%d %d\n", x, y);
	}
	if (evt == CV_EVENT_MOUSEMOVE)
	{
		//cout << x << " " << y << endl;
		sprintf(label, " %d, %d", x, y);
		cvPutText(img1, label, cvPoint(x, y), &font, CV_RGB(255, 0, 0));
		cvShowImage("show", img1);
	}
}
////判断文件路径是否存在
bool dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!  

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!  
	return false;    // this is not a directory!  
}
void run(int action, int frame, int model = 0);


int main(int argc, char* argv[])
{
	//for (int model = 1; model <= 1; model++)
	//{
	//	for (int action = 1; action <= 1; action++)
	//	{
	//		for (int frame = 0; frame <= 150; frame++)
	//		{
	//			//run(action,people,frame);
	//			run( model,  action,  frame);
	//		}
	//	}
	//}

	if (argc < 2)
	{
	cout << "需要一个frame起始帧数的参数！！HumanPoseEstimation.exe frame " << endl;
	return -1;
	}
	stringstream ss(argv[1]);
	int begin;
	ss >>begin;
	for (int action = 2; action <= 2;action++)
	{
		for (int frame = begin; frame <= 136; frame++)
		{
			run(action, frame);
		}
	}

	getchar();
	return 0;
}


void run(int action, int frame,int model)
{	
	char inputPath[128];
    char outputPath[128];
	
	
	sprintf_s(inputPath, "D:/EVAL20170704/EVAL/depth/depth%d_%d.txt", action, frame);
	ifstream isexist(inputPath);
	if (!isexist.is_open())return;//当前不存在
	cout << inputPath << endl;
	
	

	sprintf_s(outputPath, "D:/EVAL20170704/EVAL/depth/%d/%d",  action, frame);

	string prefix = outputPath;
	

	if (!dirExists(outputPath))////路径不存在创建
	{
		if (_mkdir(outputPath) == 0)
		{
			sprintf_s(outputPath, "%s/1", prefix, model, action, frame);
			_mkdir(outputPath);
			sprintf_s(outputPath, "%s/2", prefix, model, action, frame);
			_mkdir(outputPath);
			sprintf_s(outputPath, "%s/2", prefix, model, action, frame);
			_mkdir(outputPath);
			sprintf_s(outputPath, "%s/4", prefix, model, action, frame);
			_mkdir(outputPath);
			sprintf_s(outputPath, "%s/5", prefix, model, action, frame);
			_mkdir(outputPath);			
			cout << "success to mkdir" << endl;
		}
		else
		{
			cout << "fail to mkdir!!!!" << endl;
			getchar();
		}
	}
	sprintf_s(outputPath, "D:/EVAL20170704/EVAL/depth/%d/%d", action, frame, model);

	char tempPath[128];
	DWORD dwLast;
	DWORD dwCurrent;
	Mat  slicResult;//存放超像素处理结果图 有种子点
	Mat  slicBody;//显示人体超像素点
	Mat  slicseeds;//用来保存超像素种子点
	SLIC slic;
	slic.outputPath = outputPath;

	Mat photo(HEIGHT, WIDTH, CV_8UC3);
	Mat depthData(HEIGHT, WIDTH, CV_64FC3);

	const int downSampling = 1;//downsampling=1
	const int clusterNumber = 3; //定义类内聚类个数
	CvPoint2D32f gujiadian[5][clusterNumber];//存储,左右手和左右脚加头，聚类个几何中心点3
	int gujiadian2[5][clusterNumber];
	ImageShow ShowTool;
	int CenterX, CenterY;
	vector<Data> src;
	vector<Data> originalsrc;//保留初始点云数据
	Loading Load;

	//将稠密点云数据读入向量src中
	Load.DownLoad_Info(inputPath, src, downSampling);
	ShowTool.ShowPhoto(photo, src, depthData);//保存点云数据到depthData     显示图片
	originalsrc = src;
	//超像素分割
	
	clock_t clock_begin, clock_end;
	clock_begin = clock();
	dwLast = GetTickCount();
	slic.GenerateSuperpixels(photo, NUM_SUPERPIXEL, depthData);
	if (photo.channels() == 3)
		slicResult = slic.GetImgWithContours(cv::Scalar(0, 0, 255), slicBody);
	else
		slicResult = slic.GetImgWithContours(cv::Scalar(128), slicBody);
	cv::imshow("slic", slicResult);
	waitKey(1);
	slicseeds = slicBody;
	
	sprintf_s(tempPath, "%s/slicResult.jpg", outputPath);
	imwrite(tempPath, slicResult);
	dwCurrent = GetTickCount();
	cout << "It took " << (dwCurrent - dwLast) << " ms!" << endl;
	clock_end = clock();
	printf("time elapsed: %f (ms), for img size: %dx%d\n", (float)(clock_end - clock_begin) / CLOCKS_PER_SEC * 1000, slicResult.rows, slicResult.cols);
	
	//更新src数据  此时src中的数据是超像素处理后的点云
	src.clear();
	sprintf_s(tempPath, "%s/slic_body.txt", outputPath);
	Load.Load_Info(tempPath, src, 1);//downsampling=1
	double step = sqrt((WIDTH*HEIGHT) / NUM_SUPERPIXEL);//超像素步长 用来寻找中心点	

	Load.Generate_Center_Point(src, CenterX, CenterY, (int)step*2, slicBody);//超像素的中心点很有可能不在slic_body.txt中  要处理!!!!!	
	//////20170705 step*2  否则 centerx centery可能出问题
	
	circle(slicBody, Point(CenterX, CenterY), 3, Scalar(0, 0, 255), -1);

	Dijkstra dijkstra(src.size(), CenterX, CenterY, 1, (int)step);
	dijkstra.Make_Matrix(src);

	int v0 = Load.Search(CenterX, CenterY, src);	//这个感觉没必要待优化 因为slicBody里肯定有

	//先使用师兄旧方法测试 
	int dingdian = v0;
	for (int i = 1; i <= 5; i++)
	{
		dijkstra.MakeDJ(v0, true, src);
		dijkstra.Vertex[i][0] = dijkstra.Xx;
		dijkstra.Vertex[i][1] = dijkstra.Yy;
		dingdian = Load.Search(dijkstra.Vertex[i][0], dijkstra.Vertex[i][1], src);
		//dijkstra.WritePath(i, v0, src, dingdian);

		circle(slicBody, Point(dijkstra.Xx, dijkstra.Yy), 4, Scalar(0, 0, 255), -1);//种子点显示
		imshow("show", slicBody);
	}
	cvNamedWindow("show");
	IplImage iplimg = IplImage(slicBody);
	cvSetMouseCallback("show", mouseEvent, (void*)&iplimg);
	cvShowImage("show", &iplimg);
	cvWaitKey(1);

	sprintf_s(tempPath, "%s/肢端点.jpg", outputPath);
	imwrite(tempPath, slicBody);
	//jwh 20170320 添加 测试 ***********************************************待优化

	sprintf_s(tempPath, "%s/zhiduandian.txt", outputPath);
	ofstream of(tempPath);
	for (int jishu = 1; jishu < 6; jishu++)
	{
		of << dijkstra.Vertex[jishu][0] << " " << dijkstra.Vertex[jishu][1] << endl;
	}
	of.close();

	dijkstra.Make_Matrix(src);
	//	train.Adjust();  师兄的程序被我注释了
	Cluster CL;
	CL.outputPath = outputPath;
	CL.Chek(dijkstra.Vertex);

	CL.pixelBelongToSeedX = slic.pixelBelongToSeedsX;
	CL.pixelBelongToSeedY = slic.pixelBelongToSeedsY;
	CL.label = slic.label;
	for (int j = 0; j < 6; j++)
	{
		dijkstra.MakeDJ(Load.Search(dijkstra.Vertex[j][0], dijkstra.Vertex[j][1], src), false, src);
		dijkstra.Reflash(j);
	}
	
	dijkstra._Check(dijkstra.Vertex);
	cout << dijkstra.lefthand << " " << dijkstra.leftfoot << " " << dijkstra.righthand << " " << dijkstra.rightfoot << endl;

	//CL.Classify(dijkstra.Dist,dijkstra.count,src);/////也是用到了dijkstra的测地距离
	CL.DisClassify(dijkstra.Dist, dijkstra.count, src, v0);//20170320注释的 分类不准确 头部聚类在一起

	vector<CvPoint2D32f> tuples[6];

	//显示这些点的时候 class .txt 由于在cluster.cpp里写入的时候是app形式写入的 
	
	ShowTool.ShowPhoto(photo, src);
	sprintf_s(tempPath, "%s/Class0.txt", outputPath);
	ShowTool.ShowCluster(photo, 255, 255, 0, tempPath, tuples[0]);
	sprintf_s(tempPath, "%s/Class1.txt", outputPath);
	ShowTool.ShowCluster(photo, 0, 255, 0, tempPath, tuples[1]);
	sprintf_s(tempPath, "%s/Class2.txt", outputPath);
	ShowTool.ShowCluster(photo, 255, 0, 0, tempPath, tuples[2]);
	sprintf_s(tempPath, "%s/Class3.txt", outputPath);
	ShowTool.ShowCluster(photo, 0, 0, 255, tempPath, tuples[3]);
	sprintf_s(tempPath, "%s/Class4.txt", outputPath);
	ShowTool.ShowCluster(photo, 255, 0, 255, tempPath, tuples[4]);
	sprintf_s(tempPath, "%s/Class5.txt", outputPath);
	ShowTool.ShowCluster(photo, 0, 255, 255, tempPath, tuples[5]);
	sprintf_s(tempPath, "%s/13.jpg", outputPath);
	imwrite(tempPath, photo);

	Kmeans k(clusterNumber, downSampling);
	k.outputPath = outputPath;
	clock_begin = clock();
	for (int nn = 1; nn < 6; nn++)//tuples1234为四肢5为头部
	{
		k.K_solveSuperPixel(tuples[nn], nn, gujiadian[nn - 1], clusterNumber, true);///////kmeans聚类
	}
	clock_end = clock();
	printf("time elapsed: %f (ms), for img size: %dx%d\n", (float)(clock_end - clock_begin) / CLOCKS_PER_SEC * 1000, slicResult.rows, slicResult.cols);

	for (int kkk = 0; kkk < 5; kkk++)
	{
		for (int lll = 0; lll < clusterNumber; lll++)
		{//gujiadian2保存的是特征点在src数组里标号 可能不在src里用离他距离最近的点的标号代替
			//gujiadian2[kkk][lll] = Load.Search(gujiadian[kkk][lll].x, gujiadian[kkk][lll].y, src);//这里应该用原始数据 因为聚类点不一定能在超像素结果里			
			gujiadian2[kkk][lll] = Load.SearchInSlic(gujiadian[kkk][lll].x, gujiadian[kkk][lll].y, slicseeds);
			circle(photo, Point(gujiadian[kkk][lll].x, gujiadian[kkk][lll].y), 5, Scalar(0, 0, 0), -1);
		    imshow("show", photo);
			waitKey(1);
		}
		cout << endl;
	}
	sprintf_s(tempPath, "%s/14.jpg", outputPath);
	imwrite(tempPath, photo);
	

	CvPoint2D32f Skeleton_point[16];
	Skeleton_point[0].x = CenterX;
	Skeleton_point[0].y = CenterY;

	Skeleton abc;
	int vvv = Load.Search(dijkstra.Vertex[0][0], dijkstra.Vertex[0][1], src);
	 
	abc.ProcessingSkeleton(photo, dijkstra.Dist, gujiadian, gujiadian2, dijkstra.Vertex, vvv, dijkstra.lefthand, dijkstra.leftfoot, dijkstra.righthand, dijkstra.rightfoot, Skeleton_point, clusterNumber);//$$$$$$$$$$$$$$$

	//将程序获取的21个点按照如下顺序存入文件中
	//几何中心点，头部，图中左部肩，脖子，图中右部肩，图中左部手，图中右部手，图中左部腿，图中右部腿
	sprintf_s(tempPath, "%s/featurePoints.txt", outputPath);
	ofstream Cwriter(tempPath);//存放聚类的特征点数据
	 
	for (int jishu = 0; jishu < 16; jishu++)
	{
		Cwriter << Skeleton_point[jishu].x << " " << Skeleton_point[jishu].y << endl;
		cout << Skeleton_point[jishu].x << " " << Skeleton_point[jishu].y << endl;

		circle(slicResult, Point(Skeleton_point[jishu].x, Skeleton_point[jishu].y), 5, Scalar(255, 0, 0), -1);
		char str[10];////////////////////////////////////////////////测试待删除
		sprintf_s(str, "%d", jishu);
		putText(slicResult, str, Point(Skeleton_point[jishu].x, Skeleton_point[jishu].y), CV_FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
	}
	 
	Cwriter.close();
	sprintf_s(tempPath, "%s/000.jpg", outputPath);
	imwrite(tempPath, slicResult);
	
}