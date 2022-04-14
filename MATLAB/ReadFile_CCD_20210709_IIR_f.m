clear all
fid = fopen('2022_04_04__20_05_28', 'rb');  % открытие файла на чтение 
if fid == -1 
    error('File is not opened'); 
end 
  
%%B=0;                % инициализация переменной 
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
N=4;      % Order
Fc=240000;     % Cutoff Frequency
Wc=Fc/(Fs/2);
[b1,a1] = butter(N,Wc);
b=int32(b1*(2^24));
a=int32(a1*(2^24));
fvtool(b,a,'Fs',Fs);
cnt2=len;
k=0:(cnt2-1);
for i=1:cnt2
 B2(1,i)=V(i);
 x_axes(1,i)=0;
 t2(1,i)=(i-1)*dt;
 end
y=filter(b,a,B2); 
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
