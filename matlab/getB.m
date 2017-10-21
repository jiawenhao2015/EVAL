function newB = getB(X,lambda2,mu,oldB,Y,A,C,LAMBDA)

[m,n] = size(oldB);
E = eye(n);
pre = X'*X + lambda2*(X'*X) + (mu/2)*E; 

  
B1 = pre \ (X'* Y(:,1) + lambda2 * (X'*X*oldB(:,3)) + (mu/2)*A(:,1) + lambda2*(X'*C(:,1)) - 0.5*LAMBDA);

B2 = pre \ (X'* Y(:,2) + lambda2 * (X'*X*oldB(:,4)) + (mu/2)*A(:,2) + lambda2*(X'*C(:,2)) - 0.5*LAMBDA);

B3 = pre \ (X'* Y(:,3) + lambda2 * (X'*X*oldB(:,1)) + (mu/2)*A(:,3) - lambda2*(X'*C(:,1)) - 0.5*LAMBDA);

B4 = pre \ (X'* Y(:,4) + lambda2 * (X'*X*oldB(:,2)) + (mu/2)*A(:,4) - lambda2*(X'*C(:,2)) - 0.5*LAMBDA);

B5 = pre \ (X'* Y(:,5) + lambda2 * (X'*X*oldB(:,7)) + (mu/2)*A(:,5) + lambda2*(X'*C(:,3)) - 0.5*LAMBDA);

B6 = pre \ (X'* Y(:,6) + lambda2 * (X'*X*oldB(:,8)) + (mu/2)*A(:,6) + lambda2*(X'*C(:,4)) - 0.5*LAMBDA);

B7 = pre \ (X'* Y(:,7) + lambda2 * (X'*X*oldB(:,5)) + (mu/2)*A(:,7) - lambda2*(X'*C(:,3)) - 0.5*LAMBDA);

B8 = pre \ (X'* Y(:,8) + lambda2 * (X'*X*oldB(:,6)) + (mu/2)*A(:,8) - lambda2*(X'*C(:,4)) - 0.5*LAMBDA);

Belse = (X'*X + (mu/2)*E) \ (X'*Y(:,9:n) + (mu/2)*A(:,9:n) - 0.5*LAMBDA);

newB = [B1,B2,B3,B4,B5,B6,B7,B8,Belse];