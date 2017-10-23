function ret = swapColumn(X)

temp1 = X(:,7:10);% 7 8 9 10
temp2 = X(:,13:16);%13 14 15 16
temp3 = X(:,1:6);
temp4 = X(:,[11 12]);
temp5 = X(:,17:24);
ret = [temp1,temp2,temp3,temp4,temp5];



