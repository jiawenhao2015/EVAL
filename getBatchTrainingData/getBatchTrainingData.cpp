/*��������ѵ������
����֡���ɵ��������������ϵ�һ���ļ��� features.txt
�����еĶ�Ӧ�Ĺؽڵ��������ϵ�һ���ļ��� groundtruth.txt*/
#include<iostream>
#include <stdio.h>
#include<vector>
#include <fstream>
#include <string>
#include <set>
#include "highgui.h"
using namespace std;
using namespace cv;


#define HEIGHT 240
#define WIDTH  320


int actionStart, actionEnd;
int frameStart, frameEnd;
int index;//��������ѵ������Եı��� index=5 ˵��ÿ5��ȡһ������

bool readGroundTruth(string filePath, vector<int>&xVec, vector<int>&yVec)
{
	ifstream infile(filePath);
	int temp, temp2;
	float tx, ty, tz;

	if (!infile.is_open())
	{
		cout << "���ܴ��ļ���" << endl;
		return false;
	}
	while (!infile.eof())
	{
		infile >> temp >> temp2 >> tx >> ty >> tz;
		xVec.push_back(temp);
		temp2 = HEIGHT - temp2;
		yVec.push_back(temp2);
	}
	xVec.erase(xVec.end() - 1);
	yVec.erase(yVec.end() - 1);
	return true;
}
//��ÿһ֡�������������ļ� ���ϵ�����һ���ļ���
//pathΪ�����ļ�·��  allFeatureFileΪ�������ϵ�һ������յ������ļ�
void integrateFeaturesFile(string path,ofstream& allFeatureFile)
{
	ifstream featuresFile(path);
	int x, y;
	vector<int>xVec;//���������������x����
	vector<int>yVec;
	if (featuresFile.is_open())
	{
		while (!featuresFile.eof())
		{
			featuresFile >> x >> y;
			xVec.push_back(x);
			yVec.push_back(y);
		}
	}		
	else
	{
		cout << "���ܴ��ļ���" << path.c_str()<<endl;
	}
	//�����һ�����һ�� ɾ�����һ��Ԫ��
	xVec.erase(xVec.end()-1);
	yVec.erase(yVec.end()-1);
	//ÿһ�д���һ֡��������
	for (int i = 0; i < xVec.size(); i++)
	{
		allFeatureFile << xVec[i] << " " << yVec[i] << " ";
	}
	allFeatureFile << endl;
}

//��ÿһ֡�Ĺؽڵ������ļ� ���ϵ�����һ���ļ���
//pathΪ�����ļ�·��  allGroundTruthFileΪ�������ϵ�һ������յĹؽ��ļ� 
void integrateGroundTruthFile(string path, ofstream& allGroundTruthFile)
{
	cout << path << endl;	 
	
	vector<int>xVec;//���������������x����
	vector<int>yVec;
	if (!readGroundTruth(path, xVec, yVec))return;	
	 
	//ÿһ�д���һ֡�Ĺؽڵ�
	for (int i = 0; i < xVec.size(); i++)
	{
		allGroundTruthFile << xVec[i] << " "<<yVec[i]<<" ";
	}
	allGroundTruthFile << endl;	
}


//��ȡÿһ֡�Ĺؽڵ������ļ� ����첲�⵽�ֲ���λ��
//pathΪ�����ļ�·��  allGroundTruthFileΪ�������ϵ�һ������յĹؽ��ļ� 
void integrateDistanceFile(string path, ofstream& allJointsDistanceFile)
{
	cout << path << endl;

	vector<int>xVec;//���������������x����
	vector<int>yVec;
	if (!readGroundTruth(path, xVec, yVec))return;

	allJointsDistanceFile << xVec[3] - xVec[4] << " " << yVec[3] - yVec[4]<<" "  
						  << xVec[6] - xVec[7] << " " << yVec[6] - yVec[7] << " "<< endl;	 //��0��ʼ
}
//������� �� action 2 frame 136 ������  
//
set<pair<int, int>> wrongAction = { make_pair(2, 0), make_pair(2, 136),
									make_pair(7, 52), make_pair(7, 53), make_pair(7, 54) };

void genTrainData()
{
	ofstream allFeatureFile("E:/trainprocess/train/allfeatures.txt");
	string featurePathPrefix("D:/EVAL20170704/EVAL/depth/");
	char featurePath[128];

	ofstream allGroundTruthFile("E:/trainprocess/train/allGroundTruthFile.txt");
	string groundTruthPathPrefix("D:/EVAL20170704/EVAL/joints/");
	char groundTruthPath[128];
	
	
	//�ؽڵ�֮������groundtruth
	//ofstream allJointsDistanceFile("E:/trainprocess/allJointsDisFile.txt");

	
	for (int action = actionStart; action <= actionEnd; action++)//
	{
		 //ÿ10֡ ����һ���������� ����ѵ��(frame % 10 == 0)
		for (int frame = frameStart; frame <= frameEnd; frame++)
		{	
			if (wrongAction.count(make_pair(action,frame))) continue;

			if (frame % index == 0)continue;
			sprintf_s(featurePath,  "%d/%d/featurePoints.txt", action, frame);
			sprintf_s(groundTruthPath, "joint%d_%d.txt", action, frame);
				
				
			ifstream isexist(featurePathPrefix + featurePath);
			if (!isexist.is_open())continue;//��ǰ������ 
			ifstream isexist1(groundTruthPathPrefix + groundTruthPath);
			if (!isexist1.is_open())continue;//��ǰ������ 

			
			integrateFeaturesFile(featurePathPrefix + featurePath, allFeatureFile);
			integrateGroundTruthFile(groundTruthPathPrefix + groundTruthPath,allGroundTruthFile);
			cout << featurePath << endl;
			cout << groundTruthPath << endl;
		}
	}	

	allFeatureFile.close();
	allGroundTruthFile.close();	 	
}
//���ɸ첲�⵽�ֵľ���Լ��
void genDistanceData()
{//�ؽڵ�֮������groundtruth
	ofstream allJointsDistanceFile("E:/trainprocess/train/allJointsDisFile.txt");
		
	string groundTruthPathPrefix("D:/EVAL20170704/EVAL/joints/");
	char groundTruthPath[128];
	
	for (int action = actionStart; action <= actionEnd; action++)//
	{
		//ÿ10֡ ����һ���������� ����ѵ��(frame % 10 == 0)
		for (int frame = frameStart; frame <= frameEnd; frame++)
		{
			if (wrongAction.count(make_pair(action, frame))) continue;
			if (frame % index == 0)continue;
			
			sprintf_s(groundTruthPath, "joint%d_%d.txt", action, frame);
			ifstream isexist1(groundTruthPathPrefix + groundTruthPath);
			if (!isexist1.is_open())continue;//��ǰ������ 			
			
			integrateDistanceFile(groundTruthPathPrefix + groundTruthPath, allJointsDistanceFile);			
			cout << groundTruthPath << endl;
		}
	}
	allJointsDistanceFile.close();
}


int main()
{
	actionStart = 2,actionEnd = 2;
	frameStart = 3, frameEnd = 450;
	index = 5;


	genTrainData();
	genDistanceData();
	getchar();
	return 0;
}