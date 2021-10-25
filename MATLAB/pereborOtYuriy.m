clc
clear all;


D = 6000;
global R;
R= D/2;


global res;

global CurNx;
global CurNy;
global CurHx;
global CurHy;
global CurCx;
global CurCy;





%  Nx=5025;
% Ny=5025;
%  Hx=196400;
%  Hy=196400;
%  Cx=69400;
%  Cy=69400; 

 Nx=5320;
 Ny=5320;
 Hx=207400;
 Hy=207400;
 Cx=73400;
 Cy=73400; 

 res =4;
 
 Fmin = 9999999999;
 
 n=7;
 
 F_counter =0;
 
 F_mas  = zeros((2*n+1)^6,0);
 
 for i_Nx = -n:1:n
    i_Nx
      for i_Ny = -n:1:n
           i_Ny
           for i_Cx = -n:1:n
                for i_Cy = -n:1:n
                     for i_Hx = -n:1:n
                          for i_Hy = -n:1:n
   
     CurNx = Nx*(100+i_Nx)*0.01;
     CurNy = Ny*(100+i_Ny)*0.01;
     CurHx = Hx*(100+i_Hx)*0.01;
     CurHy = Hy*(100+i_Hy)*0.01;
     CurCx = Cx*(100+i_Cx)*0.01;
     CurCy = Cy*(100+i_Cy)*0.01;
     
     
     

F = GreatFunctional();
F_counter =F_counter+1;

F_mas(F_counter) = F;



if F < Fmin
    Fmin =F;
 min_Nx= CurNx;
 min_Ny = CurNy;
 min_Hx= CurHx;
 min_Hy= CurHy;
 min_Cx= CurCx;
 min_Cy= CurCy;
 
end

 end
 end
 end
 end
end
 end
     

 function Functional = GreatFunctional()
 
 global R;

Old_Front1= 3865;
Old_Spad1=6185;
Old_Front2=3865;
Old_Spad2=6185;

[Rx,Ry]= CalcDiametrs(Old_Front1,Old_Spad1,Old_Front2,Old_Spad2);

Functional = (Rx-R)^2+(Ry-R)^2;

Old_Front1= 380;
Old_Spad1=2707;
Old_Front2=3942;
Old_Spad2=6108;

[Rx,Ry]= CalcDiametrs(Old_Front1,Old_Spad1,Old_Front2,Old_Spad2);

Functional =  Functional +(Rx-R)^2+(Ry-R)^2;

Old_Front1= 7343;
Old_Spad1=9670;
Old_Front2=3776;
Old_Spad2=6274;

[Rx,Ry]= CalcDiametrs(Old_Front1,Old_Spad1,Old_Front2,Old_Spad2);

Functional =  Functional +(Rx-R)^2+(Ry-R)^2;

Old_Front1= 688;
Old_Spad1=2860;
Old_Front2=688;
Old_Spad2=2860;

[Rx,Ry]= CalcDiametrs(Old_Front1,Old_Spad1,Old_Front2,Old_Spad2);

Functional =  Functional +(Rx-R)^2+(Ry-R)^2;

Old_Front1= 3942;
Old_Spad1=6108;
Old_Front2=380;
Old_Spad2=2707;

[Rx,Ry]= CalcDiametrs(Old_Front1,Old_Spad1,Old_Front2,Old_Spad2);

Functional =  Functional +(Rx-R)^2+(Ry-R)^2;

Old_Front1= 7190;
Old_Spad1=9362;
Old_Front2=25;
Old_Spad2=2530;

[Rx,Ry]= CalcDiametrs(Old_Front1,Old_Spad1,Old_Front2,Old_Spad2);
Functional =  Functional +(Rx-R)^2+(Ry-R)^2;

Old_Front1= 25;
Old_Spad1=2530;
Old_Front2=7190;
Old_Spad2=9362;

[Rx,Ry]= CalcDiametrs(Old_Front1,Old_Spad1,Old_Front2,Old_Spad2);

Functional =  Functional +(Rx-R)^2+(Ry-R)^2;

Old_Front1= 3776;
Old_Spad1=6274;
Old_Front2=7343;
Old_Spad2=9670;

[Rx,Ry]= CalcDiametrs(Old_Front1,Old_Spad1,Old_Front2,Old_Spad2);

Functional =  Functional +(Rx-R)^2+(Ry-R)^2;

Old_Front1= 7520;
Old_Spad1=10025;
Old_Front2=7520;
Old_Spad2=10025;

[Rx,Ry]= CalcDiametrs(Old_Front1,Old_Spad1,Old_Front2,Old_Spad2);

Functional =  Functional +(Rx-R)^2+(Ry-R)^2;

end

function [Rx1,Ry1] = CalcDiametrs(Old_Front1,Old_Spad1,Old_Front2,Old_Spad2)


global CurNx;
global CurNy;
global CurHx;
global CurHy;
global CurCx;
global CurCy;
global res;


      X11 = (Old_Front1-CurNx)*res+CurCx;
      X21 = (Old_Spad1 -CurNx)*res+CurCx;
      Y11 = (Old_Front2-CurNy)*res+CurCy;
      Y21 = ( Old_Spad2-CurNy)*res+CurCy;
 


      X01 =    CurHx*tan(0.5*(atan((X21-CurCx)/CurHx)+atan((X11-CurCx)/CurHx)))+CurCx ;
      Y01 =    CurHy*tan(0.5*(atan((Y21-CurCy)/CurHy)+atan((Y11-CurCy)/CurHy)))+CurCy ;

      Ex01 =(CurCx*CurHy*Y01 + CurHx*CurHy*X01 - CurHy*X01*Y01)/(CurHx*CurHy - CurCx*CurCy - X01*Y01 + CurCy*X01 + CurCx*Y01);
      Ey01 =(CurHx*CurCy*X01 + CurHx*CurHy*Y01 - CurHx*X01*Y01)/(CurHx*CurHy - CurCx*CurCy - X01*Y01 + CurCy*X01 + CurCx*Y01);

   

      Rx1 = sqrt((Ex01-CurCx)*(Ex01-CurCx)+(CurHx-Ey01)*(CurHx-Ey01))*sin(0.5*(+atan((X21-CurCx)/CurHx)-atan((X11-CurCx)/CurHx)));
      Ry1 = sqrt((Ey01-CurCy)*(Ey01-CurCy)+(CurHy-Ex01)*(CurHy-Ex01))*sin(0.5*(+atan((Y21-CurCy)/CurHy)-atan((Y11-CurCy)/CurHy)));
end