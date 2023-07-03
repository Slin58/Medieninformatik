
%a)
rot = [0.904304, 0.191048, -0.381753; -0.076213, 0.952152, 0.295970; 0.420031, -0.238552, 0.875595];
[ev, ew] = eig(rot);

rotAxis = ev(:, 3);
degRad = acos((ew(1,1)+ew(2,2))/2);

deg = degRad * 180/3.1415;

%b)
p = [2;5;1];
pQuat = quaternion(0,  2, 5, 1);
q = quaternion(cos(degRad/2), -sin(degRad/2)*rotAxis(1,1), -sin(degRad/2)*rotAxis(2,1), -sin(degRad/2)*rotAxis(3,1));
rotatedQuat = quatmultiply(quatmultiply(q, pQuat), q');
[real, qi, qj, qk] = parts(rotatedQuat);
newPQuat = [qi; qj; qk];

newPRot = rot*p;