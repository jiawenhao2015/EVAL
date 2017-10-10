function minA = updateA(X,Y,beta,F2,Z)
[m,n] = size(X);
I = eye(n);
temp1 = 2 * X' * X + beta * I;
temp2 = 2 * X' * Y + beta * Z + F2;

A = temp1 \ temp2;%A = inv(temp1) * temp2;  Replace inv(A)*b with A\b
minA = A;