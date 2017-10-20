Y = load('E:\trainprocess\train\allGroundTruthFile.txt');
X = load('E:\trainprocess\train\allfeatures.txt');
C = load('E:\trainprocess\train\allJointsDisFile.txt');
[y1,y2]=size(Y);
[a1,a2]=size(X);

BOld = ones(a2,y2);

lambda1 = 1;
lambda2 = 1;
mu = 1;