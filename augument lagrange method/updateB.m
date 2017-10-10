function minB = updateB(lambda1,beta,F1,Z)
tau = lambda1 / beta;
G = F1/beta + Z;
[m,n] = size(G);
B = zeros(m,n);

for i=1:m
    for j=1:n
        B(i,j) = sign(G(i,j)) * max(abs(G(i,j)) - tau,0);        
    end   
end
minB = B;