function ret = reSwapColumn(X)

temp1to6 = X(:,9:14);% 
temp7to10 = X(:,1:4);%
temp11to12 = X(:,15:16);
temp13to16 = X(:,5:8);
temp17to24 = X(:,17:24);
ret = [temp1to6,temp7to10,temp11to12,temp13to16,temp17to24];
