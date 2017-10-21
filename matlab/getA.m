function newA = getA(lambda1,mu,LAMBDA,B)
tau = lambda1 / mu;
G = LAMBDA/mu + B;
[m,n] = size(G);
A = zeros(m,n);

for i=1:m
    for j=1:n
        A(i,j) = sign(G(i,j)) * max(abs(G(i,j)) - tau,0);        
    end   
end
newA = A;