clear;
clc;
Y = load('E:\trainprocess\train\allGroundTruthFile.txt');
X = load('E:\trainprocess\train\allfeatures.txt');
C = load('E:\trainprocess\train\allJointsDisFile.txt');
[y1,y2]=size(Y);
[a1,a2]=size(X);

lambda1 = 0.001;


while(lambda1 <=1)   
  for lambda2 =[0.1 0.5 1 5 10]
    Bold = rand(a2,y2);
    Aold = rand(a2,y2);
    LAMBDAold = rand(a2,y2);

    
    mu = 1;
    min_itera  = 1000;
    out_itera = 1;
    errorOld = inf;
    swapY = swapColumn(Y);       
    while ( out_itera <= min_itera)

        errorNew = mainFunc(X,swapY,Bold,C,lambda1,lambda2);
        Bnew = getB(X,lambda2,mu,Bold,swapY,Aold,C,LAMBDAold);
        Anew = getA(lambda1,mu,LAMBDAold,Bold);
        LAMBDAnew = LAMBDAold - mu*(Anew-Bnew);

        if(errorNew>=errorOld)
            break;
        end

        Bold = Bnew;
        Aold = Anew;
        LAMBDAold = LAMBDAnew;
        mu = min(100000,1.05*mu); 
        errorOld = errorNew;

        fprintf('lam1:%f---lam2:%f---out_itera:%d---error:%f\n',lambda1,lambda2,out_itera,errorOld);
        out_itera = out_itera+1;     
    end

    answer = reSwapColumn(Bnew);

    matrixPath = sprintf('E:\\trainprocess\\train\\matrix\\%f_%f.txt',lambda1,lambda2);
    saveMatrix(matrixPath,answer);
    fprintf('****************************matrixsaved*********************************\n');
        
  end
  
  lambda1 = lambda1*5;
end
