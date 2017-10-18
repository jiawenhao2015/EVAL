/*
1.显示eval数据集。
2.根据训练矩阵生成测试结果。
3.统计欧氏距离误差。给出误差厘米数据。
*/

#include <iostream>
#include <set>
#include "highgui.h"
#include "ImageShow.h"
#include "Loading.h"
#include "opencv2/imgproc/imgproc.hpp" 
  

using namespace std;
using namespace cv;


#define HEIGHT 240
#define WIDTH  320

//----------------------------------------------------------------------------------------------------------------
#define  WITH_CONSTRAINT  0

#define		PROJECT_MAT		"E:/trainprocess/train/Amatrix.txt"		//映射矩阵路径
#define		JOINTS_NUM   12     //关节点groundtruth个数
#define		FEATURE_NUM  16    //特征点个数

#define		START		0		//测试起始帧索引
#define		END 		350		//测试结束帧索引


string matrixName;
//-------------------------------------------------------------------------------------------------------------------
// 读取文件
bool ReadFile(string filePath, vector<float>&errorVec)
{
	errorVec.clear();
	ifstream infile(filePath);
	float x;
	if (!infile.is_open())
	{
		cout << "不能打开文件----" << filePath << endl;
		return false;
	}
	while (!infile.eof())
	{
		infile >> x;
		errorVec.push_back(x);
	}
	errorVec.pop_back();
	return true;
}

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

//生成测试结果   输出每一帧的估计的关节点位置。
set<pair<int, int>> wrongAction = { make_pair(2, 0), make_pair(2, 136), make_pair(7, 52),
									make_pair(7, 53), make_pair(7, 54) };

void ComputeJoint(string matrixName)
{
	Mat m = InitMat(matrixName, FEATURE_NUM * 2, JOINTS_NUM * 2);//读取训练得到的模型矩阵 *2 是因为是2维的

	stringstream ss;

	for (int action = 0; action <= 7; action++)
	{
		for (int frame = START; frame <= END; frame++)
		{
			if (wrongAction.count(make_pair(action, frame))) continue;
			if (frame % 10 != 0)continue;

			ss.str("");
			ss << "D:/EVAL20170704/EVAL/depth/" << action << "/" << frame << "/featurePoints.txt";
			
			ifstream isexist(ss.str());
			if (!isexist.is_open())continue;//当前不存在 
			
			Mat featuresMat = InitMat(ss.str(), 1, FEATURE_NUM * 2);

			Mat jointsMatrix(1, JOINTS_NUM * 2, CV_32F);//映射得到的关节点
			jointsMatrix = featuresMat * m;


			ss.str("");
			ss << "D:/EVAL20170704/EVAL/depth/" << action << "/" << frame << "/guessPoints.txt";
			ofstream of(ss.str());
			for (int i = 0; i < JOINTS_NUM; i++)//输出到本地
			{
				of << jointsMatrix.at<float>(0, 2 * i) << " " << jointsMatrix.at<float>(0, 2 * i + 1) << endl;
			}
			of.close();

			cout << ss.str() << endl;
		}
	}

}

void drawJoints(Mat &photo, vector<int>X, vector<int>Y,int flag)
{	
	for (int i = 0; i < X.size(); i++)
	{
		stringstream ss;
		ss << (i+1);
		if (flag == 1)//groundtruth
		{
			circle(photo, Point(X[i], Y[i]), 2, Scalar(0, 255,0), 1);
			putText(photo, ss.str(), Point(X[i], Y[i]), CV_FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0));
		}
		else if (flag == 0)//测试结果 与groundtruth只是颜色不一样
		{
			circle(photo, Point(X[i], Y[i]), 2, Scalar(0, 0, 255), 1);
			//putText(photo, ss.str(), Point(X[i], Y[i]), CV_FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0));
		}
	}
}
//eval数据集groundtruth
bool readGroundTruth(string filePath, vector<int>&xVec, vector<int>&yVec)
{
	ifstream infile(filePath);
	int temp, temp2;
	float tx,ty,tz;
	
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
//读取 测试生成的关节点文件
bool readGuessFile(string filePath, vector<int>&xVec, vector<int>&yVec)
{
	ifstream infile(filePath);
	float temp, temp2;
	 

	if (!infile.is_open())
	{
		cout << "不能打开文件！" << endl;
		return false;
	}
	while (!infile.eof())
	{
		infile >> temp >> temp2 ;
		xVec.push_back(temp);		
		yVec.push_back(temp2);
	}
	xVec.erase(xVec.end() - 1);
	yVec.erase(yVec.end() - 1);
	return true;
}
//显示eval数据集 以及groundtruth
int showEval()
{
	
	Mat depthData(HEIGHT, WIDTH, CV_64FC3);
	Mat photo(HEIGHT, WIDTH, CV_8UC3);
	
	Mat tmp;
	Mat tmp2;
	ImageShow ShowTool;
	vector<Data> src; 
	Loading Load;

	stringstream ss;
	vector<int>X, Y;
	for (int i = 0; i <= 7;i++)
	{
		for (int j = 0; j <= 500;j++)
		{
			src.clear();
			X.clear();
			Y.clear();
			ss.str("");

			ss << "D:/EVAL20170704/EVAL/joints/joint" << i << "_" << j << ".txt";
			if(!readGroundTruth(ss.str(),X,Y))continue;
			ss.str("");
			ss << "D:/EVAL20170704/EVAL/depth/depth" << i << "_" << j << ".txt";
			
			Load.DownLoad_Info(ss.str(), src, 1);
			photo = ShowTool.getPhoto(src);
			drawJoints(photo,X,Y,1);
			
			//pyrUp(photo, tmp, Size(photo.cols * 2, photo.rows * 2));
			//pyrDown(tmp, tmp2, Size(tmp.cols / 2, tmp.rows / 2));
			
			imshow("eval", photo);
			waitKey(1);	

			
			stringstream spath;
			spath << "D:/EVAL20170704/EVAL/depth/"<< i << "/" << j << ".jpg";
			cout << spath.str() << endl;
			imwrite(spath.str(), photo);
			//imshow("pyrup", tmp);
			//waitKey(1);
			//imshow("pyrdown", tmp2);
			//waitKey(1);

		}
	}
		
	getchar();
	return 0;
}


//根据像素位置，索引到世界坐标位置，计算误差 厘米数 XY是groundtruth myXmyY是测试结果
void ComputeRealError(vector<Data>& src, vector<int>&X, vector<int>&Y, vector<int>&myX, vector<int>&myY, string path,Mat& photo)
{
	Loading Load;
	int n = X.size();
	ofstream of(path);
	pair<int, int>pa;
	
	for (int i = 0; i < n; i++)
	{
		int index1 = Load.Search(X[i],Y[i], src);
		int index2 = Load.Search(myX[i],myY[i], src);
		//映射回去的关节点可能不在人体身上，没在点云里面，找最近的点云里的点
		while (index1 == -1)
		{
			pa = Load.SearchInNeighbor(X[i], Y[i], photo);
			index1 = Load.Search(pa.first, pa.second, src);
		}
		while (index2 == -1)
		{
			pa = Load.SearchInNeighbor(myX[i], myY[i], photo);
			cout << myX[i] << " " << myY[i] << " " << pa.first << " " << pa.second << endl;
			index2 = Load.Search(pa.first, pa.second, src);			
		}
		
		float  dis = pow((src[index1].Tx - src[index2].Tx), 2) + pow((src[index1].Ty - src[index2].Ty), 2) + pow((src[index1].Tz - src[index2].Tz), 2);

		of << sqrt(dis) << " ";
		cout << sqrt(dis) << " ";
	}
	cout << endl;
	of.close();
}


//批量生成测试结果 将测试结果显示出来 保存  并统计误差 bool flag 是否显示图片
void saveTestResults(bool flag)
{
	Mat depthData(HEIGHT, WIDTH, CV_64FC3);
	Mat photo(HEIGHT, WIDTH, CV_8UC3);

	Mat temp(HEIGHT, WIDTH, CV_8UC3);
	
	ImageShow ShowTool;
	vector<Data> src;
	Loading Load;

	stringstream ssgroundtruth;
	stringstream sstest;
	stringstream sspointcloud;
	stringstream sserror;
	vector<int>X, Y;//groundtruth
	vector<int>myX, myY;//测试结果
	for (int action = 0; action <= 7; action++)
	{
		for (int frame = START; frame <= END; frame++)
		{
			if (wrongAction.count(make_pair(action, frame))) continue;
			if (frame % 10 != 0)continue;

			cout << "-------------------"<<action<<"---------------" << frame << "----------------" << endl;
			src.clear();
			X.clear();
			Y.clear();
			myX.clear();
			myY.clear();
			ssgroundtruth.str("");
			sstest.str("");
			sspointcloud.str("");
			sserror.str("");

			ssgroundtruth << "D:/EVAL20170704/EVAL/joints/joint" << action << "_" << frame << ".txt";
			if (!readGroundTruth(ssgroundtruth.str(), X, Y))continue;
			
			sspointcloud << "D:/EVAL20170704/EVAL/depth/depth" << action << "_" << frame << ".txt";
			Load.DownLoad_Info(sspointcloud.str(), src, 1);
			photo = ShowTool.getPhoto(src);
			temp = ShowTool.getPhoto(src);//保存一下原始数据  不能用temp=photo 因为是引用！相当于本身
			 
			sstest << "D:/EVAL20170704/EVAL/depth/" << action << "/" << frame << "/guessPoints.txt";
			if (!readGuessFile(sstest.str(), myX, myY))continue;
			if (flag)
			{
				drawJoints(photo, X, Y, 1);//groundtruth
				drawJoints(photo, myX, myY, 0);//测试结果
				imshow("img", photo);
				waitKey(1);
			}
			

			//计算绝对误差 厘米数 保存到本地
			sserror << "D:/EVAL20170704/EVAL/depth/" << action << "/" << frame << "/abserror.txt";
			ComputeRealError(src, X, Y, myX, myY,sserror.str(),temp);
		}
	}	
}




//euclid error  每一帧的误差已经计算好  现在将他们汇总到一起 求平均
//每一个action为一个单位
void computeFinalError(int action)
{
	vector<vector<float>>error;//所有帧误差
	vector<float>frameError;//单帧误差

	char path[128];
	sprintf_s(path, "E:/trainprocess/action%d_euclid.txt",action);
	ofstream euclidErrorFile(path);//所有测试帧的误差信息

	sprintf_s(path,"E:/trainprocess/action%d_euclidfinal.txt", action);
	ofstream finalEuclidErrorFile(path);//所有帧平均误差

	sprintf_s(path,"E:/trainprocess/action%d_percentage.txt", action);
	ofstream percentage(path);

	string euclidErrorPrefix("D:/EVAL20170704/EVAL/depth/");
	 
	
	
	for (int frame = START; frame <= END; frame++)//读取误差到vector
	{
		if (wrongAction.count(make_pair(action, frame))) continue;
		if (frame % 10 != 0)continue;

		stringstream ss;
		ss << euclidErrorPrefix << action << "/" << frame << "/abserror.txt";
		ifstream isexist(ss.str());
		if (!isexist.is_open())continue;//当前不存在 
		if (!ReadFile(ss.str(), frameError))continue;

		error.push_back(frameError);			
	}	
	for (int i = 0; i < error.size();i++)//将vector输出到文件
	{
		for (int j = 0; j < error[i].size();j++)
		{
			euclidErrorFile << error[i][j] << " ";
		}
		euclidErrorFile << endl;
	}
	euclidErrorFile.close();
	for (int i = 0; i < error[0].size();i++)//按列访问
	{
		float err = 0;
		int cnt = 0;//统计误差小于10cm的 百分比
		for (int j = 0; j < error.size(); j++)
		{
			//if (error[j].size()!=12)cout << j<<" "<<error[j].size() << endl;
			err += error[j][i];
			if (error[j][i] <= 0.1) cnt++;
		}
		
		finalEuclidErrorFile << err << " ";
		percentage << cnt / (float)error.size() << " " << endl;
		cout << err << " ";
	}
	finalEuclidErrorFile.close();
	percentage.close();	
}
//原来的统计误差方法 是分开统计的  
int original()
{
	ComputeJoint(matrixName);//1.首先计算测试关节点位置
	saveTestResults(true);//2.统计每一帧测试关节点误差
	for (int action = 0; action <= 7;action++)
	{
		computeFinalError(action);//3.汇总统计的误差 每一个action为一个单位
	}	 
	//showEval();
	getchar();
	return 0;
}

//将7个序列的所有误差统计到一起
void computAllFinalError(string matrixName)
{
	vector<vector<float>>error;//所有帧误差
	vector<float>frameError;//单帧误差

	char path[128];
	sprintf_s(path, "E:/trainprocess/test/results/%s_actionALLeuclid.txt",matrixName.c_str());
	ofstream euclidErrorFile(path);//所有测试帧的误差信息
	
	sprintf_s(path, "E:/trainprocess/test/results/%s_actionALLeuclidfinal.txt", matrixName.c_str());
	ofstream finalEuclidErrorFile(path);//所有帧平均误差

	sprintf_s(path, "E:/trainprocess/test/results/%s_actionALLpercentage.txt", matrixName.c_str());
	ofstream percentage(path);

	
	string euclidErrorPrefix("D:/EVAL20170704/EVAL/depth/");


	for(int action = 0; action <= 7;action++)
	{
		for (int frame = START; frame <= END; frame++)//读取误差到vector
		{
			if (wrongAction.count(make_pair(action, frame))) continue;
			if (frame % 10 != 0)continue;

			stringstream ss;
			ss << euclidErrorPrefix << action << "/" << frame << "/abserror.txt";
			ifstream isexist(ss.str());
			if (!isexist.is_open())continue;//当前不存在 
			if (!ReadFile(ss.str(), frameError))continue;

			error.push_back(frameError);
		}
	}
	
	for (int i = 0; i < error.size(); i++)//将vector输出到文件
	{
		for (int j = 0; j < error[i].size(); j++)
		{
			euclidErrorFile << error[i][j] << " ";
		}
		euclidErrorFile << endl;
	}
	euclidErrorFile.close();
	for (int i = 0; i < error[0].size(); i++)//按列访问
	{
		float err = 0;
		int cnt = 0;//统计误差小于10cm的 百分比
		for (int j = 0; j < error.size(); j++)
		{
			//if (error[j].size()!=12)cout << j<<" "<<error[j].size() << endl;
			err += error[j][i];
			if (error[j][i] <= 0.1) cnt++;
		}

		finalEuclidErrorFile << err << " ";
		percentage << cnt / (float)error.size() << " " << endl;
		cout << err << " ";
	}
	finalEuclidErrorFile.close();
	percentage.close();


}

//评价不同模型的最终结果 目前是使用误差和最小的
void evaluateError()
{
	set<vector<float>>allframeError;//每一个模型的平均误差结果放到一个set里面

	string prefix("E:/trainprocess/test/results/");
	int cnt = 0;
	for (int lambda1 = 1; lambda1 <= 1000; lambda1 += 50)
	{
		for (int lambda2 = 1; lambda2 <= 1000; lambda2 += 50)
		{
			string t[] = { "0.004114", "0.061159" };
			for (int i = 0; i < 2; i++)
			{
				stringstream ss;
				ss << prefix<< lambda1 << "_" << lambda2 << "_" << t[i] << "_actionALLeuclidfinal.txt";
				string  errorPath = ss.str();
				 
				vector<float >  error;
				if (!ReadFile(errorPath, error))continue;
				cnt++;
				allframeError.insert(error);				
			}
		}
	}
	//显示有效的测试结果数
	cout << "cnt:" << cnt << "----set size:" << allframeError.size() << endl;
	
	/*for (auto it = allframeError.begin(); it != allframeError.end();it++)
	{
	for (int i = 0; i < it->size(); i++)
	{
	cout << (*it)[i]<<" ";
	}
	cout << endl;
	}*/

	//查看有效的映射矩阵的数量

}
//查看回归矩阵 用 set 剔除重复矩阵
void checkMatrix2()
{
	set<vector<float>> allmat;// 

	string prefix("E:/trainprocess/train/matrix/");
	int cnt = 0;
	vector<string>matname;
	int setsize = 0;
	for (int lambda1 = 1; lambda1 <= 1000; lambda1 += 50)
	{
		for (int lambda2 = 1; lambda2 <= 1000; lambda2 += 50)
		{
			string t[] = { "0.004526", "0.061159" };
			for (int i = 0; i < 2; i++)
			{
				stringstream ss;
				ss << prefix << lambda1 << "_" << lambda2 << "_" << t[i] << ".txt";
				string  errorPath = ss.str();

				vector<float >  error;
				if (!ReadFile(errorPath, error))continue;
				cnt++;
				allmat.insert(error);
				int nowsize = allmat.size();
				if (nowsize != setsize)
				{
					setsize = nowsize;
					matname.push_back(errorPath);
				}
			}
		}
	}
	//显示有效的测试结果数
	cout << "cnt:" << cnt << "----set size:" << allmat.size() << endl;
	 
	//查看有效的映射矩阵的数量
	for (int i = 0; i < matname.size();i++)
	{
		cout << matname[i] << endl;
	}
}
//由于矩阵名字并不规则 直接读取文件夹下矩阵文件名 存到vector里  
void checkMatrix(vector<string> & matname)
{
	 ////读list文件 读矩阵
	ifstream infile("E:\\trainprocess\\train\\matrix\\LIST.TXT");
	vector<string>matrxPath;
	if (!infile.is_open())
	{
		cout << "不能打开文件----checkMatrix"  << endl;
		return;
	}
	string t;
	while (!infile.eof())
	{
		infile >> t;
		matrxPath.push_back(t);
	}
	matrxPath.pop_back(), matrxPath.pop_back(), matrxPath.pop_back();//后2个没有用
	//////////---------------------------

	int setsize = 0;
	set<vector<float>> allmat;
	
	for (int i = 0; i < matrxPath.size();i++)
	{
		vector<float >  temp;
		if (!ReadFile("E:\\trainprocess\\train\\matrix\\"+matrxPath[i], temp))continue;	

		allmat.insert(temp);
		int nowsize = allmat.size();
		if (nowsize != setsize)
		{
			setsize = nowsize;
			matname.push_back(matrxPath[i]);
		}
		allmat.insert(temp);
	}
	cout <<"***********"<< allmat.size() << endl;
	 
	
}

int main()
{
	
	vector<string>matname;
	checkMatrix(matname);


	for (string name : matname)
	{
		string t = "E:\\trainprocess\\train\\matrix\\" + name;
		ComputeJoint(t);//1.首先计算测试关节点位置
		saveTestResults(false);//2.统计每一帧测试关节点误差	
		computAllFinalError(name);//3计算所有误差
	}

	for (int lambda1 = 1; lambda1 <= 1000;lambda1+=50)
	{
		for (int lambda2 = 1; lambda2 <= 1000; lambda2 += 50)
		{
			string t[] = { "0.004114" ,"0.061159"};
			for (int i = 0; i < 2;i++)
			{
				stringstream ss;
				ss << "E:/trainprocess/train/matrix/"<< lambda1 << "_" << lambda2 << "_" << t[i] << ".txt";
				matrixName = ss.str();
				cout << matrixName << endl;
				ComputeJoint(matrixName);//1.首先计算测试关节点位置
				saveTestResults(false);//2.统计每一帧测试关节点误差	
				matrixName = to_string(lambda1) + "_" + to_string(lambda2) + "_" + t[i];
				
				computAllFinalError(matrixName);//3计算所有误差
			}
		}
	}

	system("PAUSE");
	return 0;
}

