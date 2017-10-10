//����������֮�� ÿһ֡���������� ���� ӳ�����M �õ����ƹؽڵ�λ�� J'  
//��J'��groundtruth���Ƚ� ͳ�������Ϣ  �������
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
#define		PROJECT_MAT		"E:/trainprocess/train/MatrixT.txt"		//ӳ�����·��
#define		FEATURE_FILE		"E:/trainprocess/test/allfeatures.txt"		//����������·��
#define		GROUNDTRUTH_FILE		"E:/trainprocess/test/allGroundTruthFile.txt"		//����GROUNDTRUTH·��

#define		FINAL_ERROR_FILE		"E:/trainprocess/test/finalerror.txt"		//������� ÿһ���ؽڵ��ƽ�����
#define		EVERY_FRAME_ERROR		"E:/trainprocess/test/errorjoints.txt"		//ÿһ֡ ͳ�����

#define		TEST_NUM     63     //����������
#define		JOINTS_NUM   12     //�ؽڵ�groundtruth����
#define		FEATURE_NUM  16    //���������
//-------------------------------------------------------------------------------------------------------------------

//����һ��txt ����һ��m*n�ľ���
Mat InitMat(string matrixPath, int m, int n)
{
	ifstream matrixFile(matrixPath);
	float temp;
	Mat mat(m, n, CV_32F);
	vector<float>xVec;//������������
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
		cout << "���ܴ��ļ���" << matrixPath.c_str() << endl;
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

//ͳ���ܹ�����ʵ�����
void ComputeError()
{	
	Mat m = InitMat(PROJECT_MAT, FEATURE_NUM * 2, JOINTS_NUM * 2);//��ȡѵ���õ���ģ�;��� *2 ����Ϊ��2ά��		
	Mat featuresMat = InitMat(FEATURE_FILE, TEST_NUM, FEATURE_NUM*2);
	Mat gt = InitMat(GROUNDTRUTH_FILE, TEST_NUM, JOINTS_NUM*2);
	Mat jointsMatrix(TEST_NUM, JOINTS_NUM*2, CV_32F);//ӳ��õ��Ĺؽڵ�
	jointsMatrix = featuresMat * m;// test_num * joints_num ӳ��õ��Ĺؽڵ����

	
	Mat jointerror(TEST_NUM, JOINTS_NUM, CV_32F);// ÿһ֡  �������ÿһ���ؽڵ��ƽ�����
	Mat finalerror(1, JOINTS_NUM, CV_32F);//����֡   ������� ÿһ���ؽڵ��ƽ�����
	ofstream finalerrorfile(FINAL_ERROR_FILE);//ͳ��ÿһ���ؽڵ� ͼƬ��x y���ز�	
	ofstream errorpixel(EVERY_FRAME_ERROR);//ͳ��ÿ���ؽڵ����groundtruth ŷʽ���ز�
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

	for (int i = 0; i < JOINTS_NUM; i++)//�����ȷ��� ͳ�� TEST_NUM ֡�ܹ���� ����ƽ��
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
