//生成特征点之后 每一帧特征点坐标 乘上 映射矩阵M 得到估计关节点位置 J'  
//将J'与groundtruth做比较 统计误差信息  像素误差
#include<iostream>
#include <stdio.h>
#include<vector>
#include <fstream>
#include <string>
#include<math.h>
#include "highgui.h"
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------
#define		PROJECT_MAT		"E:/trainprocess/train/MatrixT.txt"		//映射矩阵路径
#define		FEATURE_FILE		"E:/trainprocess/test/allfeatures.txt"		//测试特征点路径
#define		GROUNDTRUTH_FILE		"E:/trainprocess/test/allGroundTruthFile.txt"		//测试GROUNDTRUTH路径

#define		FINAL_ERROR_FILE		"E:/trainprocess/test/finalerror.txt"		//最终误差 每一个关节点的平均误差
#define		EVERY_FRAME_ERROR		"E:/trainprocess/test/errorjoints.txt"		//每一帧 统计误差

#define		TEST_NUM     63     //测试样本数
#define		JOINTS_NUM   12     //关节点groundtruth个数
#define		FEATURE_NUM  16    //特征点个数
//-------------------------------------------------------------------------------------------------------------------

//读入一个txt 返回一个m*n的矩阵
Mat InitMat(string matrixPath, int m, int n)
{
	ifstream matrixFile(matrixPath);
	float temp;
	Mat mat(m, n, CV_32F);
	vector<float>xVec;//保存所有坐标
	if (matrixFile.is_open())
	{
		while (!matrixFile.eof())
		{
			matrixFile >> temp;
			xVec.push_back(temp);
		}
	}
	else
	{
		cout << "不能打开文件！" << matrixPath.c_str() << endl;
	}
	xVec.erase(xVec.end() - 1);
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			mat.at<float>(i, j) = xVec[i * n + j];
		}
	}
	return mat;
}

//统计总共测试实验误差
void ComputeError()
{	
	Mat m = InitMat(PROJECT_MAT, FEATURE_NUM * 2, JOINTS_NUM * 2);//读取训练得到的模型矩阵 *2 是因为是2维的		
	Mat featuresMat = InitMat(FEATURE_FILE, TEST_NUM, FEATURE_NUM*2);
	Mat gt = InitMat(GROUNDTRUTH_FILE, TEST_NUM, JOINTS_NUM*2);
	Mat jointsMatrix(TEST_NUM, JOINTS_NUM*2, CV_32F);//映射得到的关节点
	jointsMatrix = featuresMat * m;// test_num * joints_num 映射得到的关节点矩阵

	
	Mat jointerror(TEST_NUM, JOINTS_NUM, CV_32F);// 每一帧  最终误差每一个关节点的平均误差
	Mat finalerror(1, JOINTS_NUM, CV_32F);//所有帧   最终误差 每一个关节点的平均误差
	ofstream finalerrorfile(FINAL_ERROR_FILE);//统计每一个关节点 图片上x y像素差	
	ofstream errorpixel(EVERY_FRAME_ERROR);//统计每个关节点距离groundtruth 欧式像素差
	for (int i = 0; i < TEST_NUM; i++)
	{
		for (int j = 0; j < JOINTS_NUM; j++)
		{
			float x =  jointsMatrix.at<float>(i, j * 2) - gt.at<float>(i, j * 2);
			float y =  jointsMatrix.at<float>(i, j * 2 + 1) - gt.at<float>(i, j * 2 + 1);
			
			errorpixel << sqrt(x*x + y*y) << " ";
			jointerror.at<float>(i, j) = sqrt(x*x + y*y);
		}
		
		errorpixel << " " << endl;
		cout << i << endl;
	}

	for (int i = 0; i < JOINTS_NUM; i++)//列优先访问 统计 TEST_NUM 帧总共误差 并求平均
	{
		float e = 0;
		for (int j = 0; j < TEST_NUM; j++)
		{
			e += jointerror.at<float>(j, i);
		}
		finalerror.at<float>(0, i) = e / TEST_NUM;
		finalerrorfile << finalerror.at<float>(0, i) << " ";
	}
	finalerrorfile.close();	
	errorpixel.close();
}



int main()
{
	ComputeError();
	getchar();
	return 0;
}
