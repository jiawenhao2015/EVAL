clear;
clc;
Y = load('E:\trainprocess\train\allGroundTruthFile.txt');
X = load('E:\trainprocess\train\allfeatures.txt');
C = load('E:\trainprocess\train\allJointsDisFile.txt');

[y1,y2]=size(Y);
[a1,a2]=size(X);
lambda1 = 0.9;
lambda2 = 0;

    Bold = rand(a2,y2);
    Aold = rand(a2,y2);
    LAMBDAold = rand(a2,y2);
    
    mu = 1;
    min_itera  = 30;
    out_itera = 1;
    errorOld = inf;
    swapY = swapColumn(Y);       
    while ( out_itera <= min_itera)

        [errorNewB ,errB_fro] = mainFunc(X,swapY,Bold,C,lambda1,lambda2);
        [errorNewA ,errA_fro]= mainFunc(X,swapY,Aold,C,lambda1,lambda2);
 
%         if(errorNew>=errorOld)
%             break;
%         end

        Bnew = getB(X,lambda2,mu,Bold,swapY,Aold,C,LAMBDAold);
        Anew = getA(lambda1,mu,LAMBDAold,Bold);
        LAMBDAnew = LAMBDAold - mu*(Anew-Bnew);
%         norm(X*Bnew - Y, 'fro')
%         norm(X*Anew - Y, 'fro')
%         sum(abs(Bnew(:)))
%         sum(abs(Anew(:)))
        
        Bold = Bnew;
        Aold = Anew;
        LAMBDAold = LAMBDAnew;
        mu = min(100000, 1.05*mu); 
        errorOldB = errorNewB;
        errorOldA = errorNewA;
        fprintf('%02d---lam1:%.2f---lam2:%.2f         errorB:%.3f*****errorA:%.3f froB:%.3f*****froA:%.3f\n',...
                     out_itera,lambda1,lambda2,errorOldB,errorOldA,errB_fro,errA_fro);
        out_itera = out_itera+1;
    end
    answer = reSwapColumn(Bnew);
    Amatrix =  reSwapColumn(Anew);
    
    
   % matrixPath = sprintf('E:\\trainprocess\\train\\matrix\\B%f_%f.txt',lambda1,lambda2);
   % saveMatrix(matrixPath,answer);
    matrixPath = sprintf('E:\\trainprocess\\train\\matrix\\A%f_%f.txt',lambda1,lambda2);
    saveMatrix(matrixPath,Amatrix);
    fprintf('****************************matrixsaved*********************************\n');
        

