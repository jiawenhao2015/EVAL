close all;
clear;
clc;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
for lambda1 = 1 : 1 : 2
    for lambda2 =  1 :  1 : 2
        for beta = 1:10:1
            solveEquation(lambda1,lambda2,beta);
        end 
    end
end

disp('!!!!!!!!!!!!!!!!!!!the iteration over!!!!!!!!!!!!!!!!!!!!!!!!!!');





    