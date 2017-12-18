function   solveEquation(lambda1,lambda2,beta)

Y = load('E:\trainprocess\train\allGroundTruthFile.txt');
X = load('E:\trainprocess\train\allfeatures.txt');
C = load('E:\trainprocess\train\allJointsDisFile.txt');
[y1,y2]=size(Y);
[a1,a2]=size(X);


betamax = 10000000000;
rho = 1.1;%2.0 1.1

lForeArm = 7;%9;%?allGroundTruthFile  groundtruth lforearm 7 lHand 9  rForeArm 13 rHand 15
lHand = 9;%11;
rForeArm = 13;%15;
rHand = 15;%17;

F1 = ones(a2,y2);%??m×n?1?
F2 = ones(a2,y2);%??m×n?1?


AOld = ones(a2,y2);
BOld = ones(a2,y2);
ZOld = ones(a2,y2);
max_itera = 1000;%????1000
min_itera = 20;%the minum number of iteration
out_itera = 1;
epsilon = 1e-6;%error value 1e-8
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
diffA = 0;
while ( out_itera < min_itera || diffA < 0.000001   )
%while ( out_itera < 5)
    ANew = updateA(X,Y,beta,F2,ZOld);    
    BNew = updateB(lambda1,beta,F1,ZOld);
    ZNew = updateZ(lambda2,beta,AOld,BOld,C,X,ZOld,F1,F2,lForeArm,lHand,rForeArm,rHand);    
    
    diffA = norm(ANew - AOld,inf);
    diffB = norm(BNew - BOld,inf);
    diffZ = norm(ZNew - ZOld,inf);
    
    fprintf('lambda1:%f---lambda2:%f---- iteration:%d-------------diffA = %f-------diffB = %f---------diffZ = %f\n',...
            lambda1,lambda2,out_itera,diffA,diffB,diffZ);
    if max(diffA,max(diffB,diffZ)) < epsilon
        break;
    end
    
    AOld = ANew;
    BOld = BNew;
    ZOld = ZNew;
    F1 = F1 + beta * (ZNew - BNew);
    F2 = F2 + beta * (ZNew - ANew);
    beta = min(betamax,rho * beta);%update ??
    out_itera = out_itera + 1;    


end
%save the matrix
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

matrixPath = sprintf('E:\\trainprocess\\train\\matrix\\%f_%f_%f_A.txt',lambda1,lambda2,beta);
saveMatrix(matrixPath,ANew);
%matrixPath = sprintf('E:\\trainprocess\\train\\matrix\\%f_%f_%f_B.txt',lambda1,lambda2,beta);
%saveMatrix(matrixPath,BNew);
%matrixPath = sprintf('E:\\trainprocess\\train\\matrix\\%f_%f_%f_Z.txt',lambda1,lambda2,beta);
%saveMatrix(matrixPath,ZNew);
