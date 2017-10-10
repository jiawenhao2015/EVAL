close all;
clear
clc
Y = load('E:\trainprocess\train\allGroundTruthFile.txt');
X = load('E:\trainprocess\train\allfeatures.txt');
C = load('E:\trainprocess\train\allJointsDisFile.txt');
[y1,y2]=size(Y);
[a1,a2]=size(X);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
lambda1=100;%700  1
lambda2=300;%10   2
beta = 0.00001;%0.01
betamax = 10000000000;
rho = 1.1;%2.0 1.1

lForeArm = 4;%9;%?allGroundTruthFile ?????? ???2???2?
lHand = 5;%11;
rForeArm = 7;%15;
rHand = 8;%17;

F1 = ones(a2,y2);%??m×n?1?
F2 = ones(a2,y2);%??m×n?1?


AOld = ones(a2,y2);
BOld = ones(a2,y2);
ZOld = ones(a2,y2);
max_itera=56;%????1000
out_itera=1;
epsilon = 1e-6;%error value 1e-8
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

while out_itera < max_itera 
    
    ANew = updateA(X,Y,beta,F2,ZOld);    
    BNew = updateB(lambda1,beta,F1,ZOld);
    ZNew = updateZ(lambda2,beta,AOld,BOld,C,X,ZOld,F1,F2,lForeArm,lHand,rForeArm,rHand);    
    
    diffA = norm(ANew - AOld,inf);
    diffB = norm(BNew - BOld,inf);
    diffZ = norm(ZNew - ZOld,inf);
    
    fprintf('iteration:%d-------------diffA = %f-------diffB = %f---------diffZ = %f\n',...
            out_itera,diffA,diffB,diffZ);
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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


saveMatrix('E:\trainprocess\train\Amatrix.txt',ANew);
saveMatrix('E:\trainprocess\train\Bmatrix.txt',BNew);
saveMatrix('E:\trainprocess\train\Zmatrix.txt',ZNew);

disp('!!!!!!!!!!!!!!!!!!!the iteration over!!!!!!!!!!!!!!!!!!!!!!!!!!');





    