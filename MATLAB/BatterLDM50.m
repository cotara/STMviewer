clear all
fid = fopen('C:\qt_pr\STMviewer\STMviewer\MATLAB\2022_04_05__20_15_23', 'rb');  % открытие файла на чтение 
if fid == -1 
    error('File is not opened'); 
end 
  
%%B=0;              % инициализация переменной 
cnt=1;              % инициализация счетчика 
 V = fread(fid,'uint8');  %считывание одного
 k=0;                     %счетчик шотов
 j=0;                     %счетчик байтов в шоте
 i=1;
 ch=0;					%Счетчик каналов
 len=length(V)-3;
while(i<=len)
    if(V(i)==255 && V(i+1)==0 && V(i+2)==255 && V(i+3)==0)
        k=k+1;
        i=i+4;
        j=0;
    elseif(V(i)==254 && V(i+1)==0 && V(i+2)==254 && V(i+3)==0)
        i=i+4; 
        k=0;
		ch=ch+1;
    else
        j=j+1;
        shots(j,k+1,ch+1)=V(i);
        i=i+1;
    end
    
end



ch1 = shots(:,:,1);

ch2 = shots(:,:,2);

ch3 = shots(:,:,3);

ch4 = shots(:,:,4);

hold on;


plot(ch1(:,13))
%plot(ch3(:,5))
fclose('all')

%БИХ БУТЕРБРОД
Fs=10e6;
dt=1/Fs;
N=4;            % Order
Fc=250000;     % Cutoff Frequency
Wc=Fc/(Fs/2);
[b1,a1] = butter(N,Wc);
b=int32(b1*(2^24));
a=int32(a1*(2^24));
ah = dec2hex(a);
bh = dec2hex(b);
y=filter(b,a,ch1(:,1)); 
fir1
for i=10:length(y)
    y(i-9)=y(i);
end

plot(y); %Figure3

