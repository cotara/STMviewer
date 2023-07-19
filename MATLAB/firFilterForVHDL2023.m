clear all
clc
%//////////////////////////////////////////////////////////////////////////
%                       ��������� � �����                                 /
%//////////////////////////////////////////////////////////////////////////
fid = fopen('C:\Ruslan\QT_pr\STMviewer\MATLAB\2021_06_23__15_07_56_CH1', 'rb');  % �������� ����� �� ������ 
if fid == -1 
    error('File is not opened'); 
end 
  
 V = fread(fid,'uint8');  %������ ���� ����
 k=0;                     %������� �����
 j=0;                     %������� ������ � ����
 i=1;
 len=length(V)-3;
 %��������� ���� �� ����
while(i<=len)
    if(V(i)==255 && V(i+1)==0 && V(i+2)==255 && V(i+3)==0)
        k=k+1;
        i=i+4;
        bytesInShot = j;
        j=0;
    else
        j=j+1;
        shots(k+1,j)=V(i);
    end
    i=i+1;
end
%surf(shots);
fclose('all');
%//////////////////////////////////////////////////////////////////////////
%                              ���������� ��������                        /
%//////////////////////////////////////////////////////////////////////////
step=zeros(1,bytesInShot);
for m=1000:bytesInShot
   step(m)=100;
end
%//////////////////////////////////////////////////////////////////////////
%                               ���-������                                /
%//////////////////////////////////////////////////////////////////////////
Fs=3.75e6;
dt=1/Fs;
N=201;                                                                     % Order
Fc=240000;                                                                 % Cutoff Frequency
flag='scale';                                                              % Sampling Flag
Beta=6;                                                                    % Window Parameter
win = kaiser(N, Beta);                                                     % Window
Wc=2*Fc/Fs;
b=int32((2^32)*fir1(N-1,Wc,'low',win,flag));                               %������������ �����
bd=double(b);
dx_mas_counter =1;

dx_mas = zeros(N,1);
kk=20;                                                                       %����� ����
outFIR=zeros(1,bytesInShot);
ticks=zeros(1,bytesInShot);

for j=1:bytesInShot
    dx_mas(dx_mas_counter) = shots(kk,j);  
    dx_mas(dx_mas_counter) = step(j);
    Yx = 0;

    for i = 1:N
       dx_mas_counter_offset =dx_mas_counter+i-1;
       if dx_mas_counter_offset>N
          dx_mas_counter_offset= dx_mas_counter_offset-N;
       end 
       Yx=Yx+bd(i)*dx_mas(dx_mas_counter_offset);      
    end    
    dx_mas_counter=dx_mas_counter+1;
    if dx_mas_counter == N+1
        dx_mas_counter=1;
    end
    Yx=Yx/2^32;
    if(Yx<0)
        Yx=0;
    elseif(Yx>255)
        Yx=255;
    end
    outFIR(j)=Yx;
    if(ticks(j)==0)
        ticks(j+1)=20;
    else
        ticks(j+1)=0;
    end
end

%//////////////////////////////////////////////////////////////////////////
%                              ���-���������                              /
%//////////////////////////////////////////////////////////////////////////
N=4;                                                                        % Order
Fc=70000;                                                                  % Cutoff Frequency
Wc=Fc/(Fs/2);
[b1,a1] = butter(N,Wc);
b=(b1*(2^24));                                                              %������������
b=int32(b);
a=(a1*(2^24));
a=int32(a);                                                                 

for i=1:bytesInShot
 %B3(1,i)=shots(kk,i);                                                       %�������������� ������
 B3(1,i)=step(i);
end
outMatlabBUTER=filter(b1,a1,B3);                                           %���������� �����������  

% ����� ������ 4 �����, ��� ������
%�������� ������������ �� 24 ���� ����� (��������� 8-������ �������� � 32-������)
% shift_Y(1)=bitshift(B3(4),24);
% shift_Y(2)=bitshift(B3(3),24);
% shift_Y(3)=bitshift(B3(2),24);
% shift_Y(4)=bitshift(B3(1),24);

shift_Y(1)=0;
shift_Y(2)=0;
shift_Y(3)=0;
shift_Y(4)=0;

shift_X(1)=0;                                         %�������������� 32 ������ ������ � ��������� ��������� (5 ����)
shift_X(2)=0;
shift_X(3)=0;
shift_X(4)=0;

Yx=0;
% coef=[449,1794,2691,1794,449,-60500812,82109537,-49687023,11308258];
% coef=[6989,27957,41936,27957,6989,-53362260 ,64780092,-35436087,7352869];   
coef=[37495,	149981,	224972,	149981,	37495,-45174516,	48106974,	-23546565,	4436816];   

for i=5:bytesInShot
    %����� ����� �� 24 ����.(��������� 8-������ �������� � 32-������)
    shift_X(5)=shift_X(4);
    shift_X(4)=shift_X(3);
    shift_X(3)=shift_X(2);
    shift_X(2)=shift_X(1);
    shift_X(1)=bitshift(B3(i),24);                                         %�������������� 32 ������ ������ � ��������� ��������� (5 ����)

    mul(1) = shift_X(1)*coef(1);
    add(1) = mul(1);
    mul(2) = shift_X(2)*coef(2);
    add(2) = add(1)+mul(2);
    mul(3) = shift_X(3)*coef(3);
    add(3) = add(2)+mul(3);
    mul(4) = shift_X(4)*coef(4);
    add(4) = add(3)+mul(4);
    mul(5) = shift_X(5)*coef(5);
    add(5) = add(4)+mul(5);
    
    mul(6) = shift_Y(1)*coef(6);
    add(6) = add(5)-mul(6);
    mul(7) = shift_Y(2)*coef(7);
    add(7) = add(6)-mul(7);
    mul(8) = shift_Y(3)*coef(8);
    add(8) = add(7)-mul(8);
    mul(9) = shift_Y(4)*coef(9);
    add(9) = add(8)-mul(9); 
    % ����� 9 ���������
    %��������� ����� � ����� ������� ���������� 48 ���: 24 �� �������������
    %� 24 �� ������ �������������� ������
    y=int64(add(9));                                                       %������ �� ��������� ��� ������� � double. ����������� � int
    DOut=bitshift(y,-24);                                                  %�������� �� 24 ����, ����� ����� ��������� 32 ������ ��� ���������� ��������
    DOut8bit = bitshift(y,-48);                                            %�������� �� 48 ���, ����� �������� 8-������ ����� ��� ����������
    %�������������� 32-������ ������� � ��������� ��������� (4 ����)
    shift_Y(4)=shift_Y(3);
    shift_Y(3)=shift_Y(2);
    shift_Y(2)=shift_Y(1);
    shift_Y(1)=DOut;

    outMyBUTTER(i)= DOut8bit;
    outMyBUTTER32bit(i) = DOut;
end

%������ �����������
for m=1:bytesInShot-102
    %outFIR(m)=outFIR(m+102);
    %outMatlabBUTER(m)=outMatlabBUTER(m+20);
    %outMyBUTTER(m)=outMyBUTTER(m+20);
end
plot(B3);
hold on;
plot(outFIR);
hold on;
plot(outMyBUTTER);
hold on;
plot(outMatlabBUTER);
legend('�������� ������', 'MyFIR','MyButter','MatlabButer');
%hold on;
%plot(step);