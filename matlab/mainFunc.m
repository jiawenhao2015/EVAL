function [ret,fro_val] = mainFunc(X,Y,B,C,lambda1,lambda2)

item1 = norm(Y-X*B,'fro');
item2 = norm(B,1);
item3 = norm( X*B(:,1:2) - X*B(:,3:4) - C(:,1:2), 'fro');
item4 = norm( X*B(:,5:6) - X*B(:,7:8) - C(:,3:4), 'fro');

ret = item1*item1 + lambda1*item2 + lambda2*(item3*item3 + item4*item4);
fro_val = item1*item1;