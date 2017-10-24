 clear;
 clc;
lambda1 = 0.001;

while(lambda1 <=1)
    
  for lambda2 = [0.1 0.5 1 5 10]
      matrixPath = sprintf('E:\\trainprocess\\train\\matrix\\%f_%f.txt',lambda1,lambda2);
       
      t = load(matrixPath);      
       
      lam1 = 0.001;
      
      while(lam1<=1)
          for lam2 =[0.1 0.5 1 5 10]
              
               path = sprintf('E:\\trainprocess\\train\\matrix\\%f_%f.txt',lam1,lam2);       
               tt = load(path);
               
               if(lam1<lambda1 || lam2<lambda2 || strcmpi(path,matrixPath))
                  continue;
               end
               item1 = norm(tt-t,'fro');
               fprintf('%f_%f---%f_%f***********%f\n',lambda1,lambda2,lam1,lam2,item1);
          end
          lam1 = lam1*5;
      end
      
      
  end
  lambda1 = lambda1*5;
end

