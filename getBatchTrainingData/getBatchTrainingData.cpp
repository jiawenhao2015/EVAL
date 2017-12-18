/*批量生成训练数据
将逐帧生成的特征点数据整合到一个文件里 features.txt
将所有的对应的关节点数据整合到一个文件里 groundtruth.txt*/
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
int index;//用来控制训练与测试的比例 index=5 说明每5个取一个测试

bool readGroundTruth(string filePath, vector<int>&xVec, vector<int>&yVec)
{
	ifstream infile(filePath);
	int temp, temp2;
	float tx, ty, tz;

	if (!infile.is_open())
	{
		cout << "不能打开文件！" << endl;
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
//将每一帧的特征点坐标文件 整合到最终一个文件里
//path为输入文件路径  allFeatureFile为最终整合到一起的最终的特征文件
void integrateFeaturesFile(string path,ofstream& allFeatureFile)
{
	ifstream featuresFile(path);
	int x, y;
	vector<int>xVec;//保存所有特征点的x坐标
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
		cout << "不能打开文件！" << path.c_str()<<endl;
	}
	//多读了一遍最后一行 删除最后一个元素
	xVec.erase(xVec.end()-1);
	yVec.erase(yVec.end()-1);
	//每一行代表一帧的特征点
	for (int i = 0; i < xVec.size(); i++)
	{
		allFeatureFile << xVec[i] << " " << yVec[i] << " ";
	}
	allFeatureFile << endl;
}

//将每一帧的关节点坐标文件 整合到最终一个文件里
//path为输入文件路径  allGroundTruthFile为最终整合到一起的最终的关节文件 
void integrateGroundTruthFile(string path, ofstream& allGroundTruthFile)
{
	cout << path << endl;	 
	
	vector<int>xVec;//保存所有特征点的x坐标
	vector<int>yVec;
	if (!readGroundTruth(path, xVec, yVec))return;	
	 
	//每一行代表一帧的关节点
	for (int i = 0; i < xVec.size(); i++)
	{
		allGroundTruthFile << xVec[i] << " "<<yVec[i]<<" ";
	}
	allGroundTruthFile << endl;	
}


//读取每一帧的关节点坐标文件 计算胳膊肘到手部的位置
//path为输入文件路径  allGroundTruthFile为最终整合到一起的最终的关节文件 
void integrateDistanceFile(string path, ofstream& allJointsDistanceFile)
{
	cout << path << endl;

	vector<int>xVec;//保存所有特征点的x坐标
	vector<int>yVec;
	if (!readGroundTruth(path, xVec, yVec))return;

	allJointsDistanceFile << xVec[3] - xVec[4] << " " << yVec[3] - yVec[4]<<" "  
						  << xVec[6] - xVec[7] << " " << yVec[6] - yVec[7] << " "<< endl;	 //从0开始
}
//有问题的 是 action 2 frame 136 特征点  
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
	
	
	//关节点之间距离的groundtruth
	//ofstream allJointsDistanceFile("E:/trainprocess/allJointsDisFile.txt");

	
	for (int action = actionStart; action <= actionEnd; action++)//
	{
		 //每10帧 留出一个人做测试 其余训练(frame % 10 == 0)
		for (int frame = frameStart; frame <= frameEnd; frame++)
		{	
			if (wrongAction.count(make_pair(action,frame))) continue;

			if (frame % index == 0)continue;
			sprintf_s(featurePath,  "%d/%d/featurePoints.txt", action, frame);
			sprintf_s(groundTruthPath, "joint%d_%d.txt", action, frame);
				
				
			ifstream isexist(featurePathPrefix + featurePath);
			if (!isexist.is_open())continue;//当前不存在 
			ifstream isexist1(groundTruthPathPrefix + groundTruthPath);
			if (!isexist1.is_open())continue;//当前不存在 

			
			integrateFeaturesFile(featurePathPrefix + featurePath, allFeatureFile);
			integrateGroundTruthFile(groundTruthPathPrefix + groundTruthPath,allGroundTruthFile);
			cout << featurePath << endl;
			cout << groundTruthPath << endl;
		}
	}	

	allFeatureFile.close();
	allGroundTruthFile.close();	 	
}
//生成胳膊肘到手的距离约束
void genDistanceData()
{//关节点之间距离的groundtruth
	ofstream allJointsDistanceFile("E:/trainprocess/train/allJointsDisFile.txt");
		
	string groundTruthPathPrefix("D:/EVAL20170704/EVAL/joints/");
	char groundTruthPath[128];
	
	for (int action = actionStart; action <= actionEnd; action++)//
	{
		//每10帧 留出一个人做测试 其余训练(frame % 10 == 0)
		for (int frame = frameStart; frame <= frameEnd; frame++)
		{
			if (wrongAction.count(make_pair(action, frame))) continue;
			if (frame % index == 0)continue;
			
			sprintf_s(groundTruthPath, "joint%d_%d.txt", action, frame);
			ifstream isexist1(groundTruthPathPrefix + groundTruthPath);
			if (!isexist1.is_open())continue;//当前不存在 			
			
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