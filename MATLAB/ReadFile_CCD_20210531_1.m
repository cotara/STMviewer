clear all
fid = fopen('C:\qt_pr\STMviewer\STMviewer\MATLAB\2021_06_22__20_45_44_CH2.log', 'rb');  % открытие файла на чтение 
if fid == -1 
    error('File is not opened'); 
end 
  
%%B=0;              % инициализация переменной 
cnt=1;              % инициализация счетчика 
 V = fread(fid,'uint8');  %считывание одного
 k=0;                     %счетчик шотов
 j=0;                     %счетчик байтов в шоте
 i=1;
 len=length(V)-3;
while(i<=len)
    if(V(i)==255 && V(i+1)==0 && V(i+2)==255 && V(i+3)==0)
        k=k+1;
        i=i+4;
        j=0;
    else
        j=j+1;
        shots(k+1,j)=V(i);
    end
    i=i+1;
end
%surf(shots);
fclose('all')
Fs=10e6;
dt=1/Fs;
% The following code was used to design the filter coefficients:
    % % FIR Window Lowpass filter designed using the FIR1 function.
    %
    % % All frequency values are in Hz.
    % Fs = 10000000;  % Sampling Frequency
    %
    N=201;      % Order
    %%Fc1  = 120000;   % First Cutoff Frequency
    %%Fc2  = 240000;   % Second Cutoff Frequency
    Fc=240000;     % Cutoff Frequency
    flag='scale';  % Sampling Flag
    Beta=6;        % Window Parameter
    %
    % % Create the window vector for the design algorithm.
    win = kaiser(N, Beta);
    Wc=2*Fc/Fs;
    %%Wc1=2*Fc1/Fs;
    %%Wc2=2*Fc2/Fs;
    %
    % % Calculate the coefficients using the FIR1 function.
%%b2=fir1(N,[Wc1 Wc2],'bandpass',win,flag);
b=int32((2^32)*fir1(N-1,Wc,'low',win,flag));
fvtool(b,1,'Fs',Fs);
cnt2=len;
k=0:(cnt2-1);
for i=1:cnt2
 B2(1,i)=V(i);
 x_axes(1,i)=0;
 t2(1,i)=(i-1)*dt;
end
fid = fopen('Ckoeff.txt','w');
fprintf(fid,'%d\n',b);
fclose(fid);
y=filter(b,1,B2)/(2^32);
%%y2=filter(b2,1,B2);
figure
plot(V,'Marker','.'); 
figure
plot(k,B2,k,y,k,x_axes,'Marker','.'); %Figure3
%Sx=abs(fft(B2));
%Sy=abs(fft(y));
%dF=Fs/cnt2;
%f = Fs*(0:(cnt2-1))/(cnt2);
%figure
%plot(f,Sx,f,Sy,'Marker','.'); %Figure2
