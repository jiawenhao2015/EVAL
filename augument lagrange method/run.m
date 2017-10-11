close all;
clear;
clc;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
for lambda1 = 1 : 50 : 1000
    for lambda2 = 1 : 50: 1000
        for beta = [0.0001,0.01]
            solveEquation(lambda1,lambda2,beta);
        end        
    end
end



disp('!!!!!!!!!!!!!!!!!!!the iteration over!!!!!!!!!!!!!!!!!!!!!!!!!!');





    