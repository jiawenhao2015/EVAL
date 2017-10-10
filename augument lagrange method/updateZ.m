function minZ = updateZ(lambda2,beta,A,B,C,X,Z,F1,F2,lForeArm,lHand,rForeArm,rHand)
H = B - F1/beta;
U = A - F2/beta;
[m,n] = size(X);
I = eye(n);
newZ = Z;
oldZLForeArm = Z(:,lForeArm:lForeArm+1);
oldZLHand = Z(:,lHand:lHand+1);
oldZRForeArm = Z(:,rForeArm:rForeArm+1);
oldZRHand = Z(:,rHand:rHand+1);

left = 2 * lambda2 * X' * X + 2 * beta * I;
temp1 = beta * ( H(:,lForeArm:lForeArm+1) + U(:,lForeArm:lForeArm+1));
temp2 = beta * ( H(:,lHand:lHand+1) + U(:,lHand:lHand+1));
temp3 = beta * ( H(:,rForeArm:rForeArm+1) + U(:,rForeArm:rForeArm+1));
temp4 = beta * ( H(:,rHand:rHand+1) + U(:,rHand:rHand+1));

CLeftLength = C(:,1:2); %leftForeArm - leftHand 
CRightLength = C(:,3:4); %rightForeArm - rightHand

newZLForeArm = left \ (2 * lambda2 * X' *(X * oldZLHand + CLeftLength) + temp1);
newZLHand = left \ (2 * lambda2 * X' *(X * oldZLForeArm - CLeftLength) + temp2);

newZRForeArm = left \ (2 * lambda2 * X' *(X * oldZRHand + CRightLength) + temp3);
newZRHand = left \ (2 * lambda2 * X' *(X*oldZRForeArm - CRightLength) + temp4);

newZ(:,lForeArm:lForeArm+1) = newZLForeArm;
newZ(:,lHand:lHand+1) = newZLHand;
newZ(:,rForeArm:rForeArm+1) = newZRForeArm;
newZ(:,rHand:rHand+1) = newZRHand;

minZ = newZ;
