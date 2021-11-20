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
tempShot = shots(1,:);                                                      %Исходный сигнал
%plot(tempShot);
%surf(shots);
fclose('all')
Fs=10e6;
dt=1/Fs;
N=201;                                                                      % Порядок
Fc=240000;                                                                  % Частота среза
flag='scale';                                                               % флаг семплирования
Beta=6;                                                                     % Параметр, определяющий окно фильтра
win = kaiser(N, Beta);                                                      % Окно фильтра
Wc=2*Fc/Fs;                                                                 
b=fir1(N-1,Wc,'low',win,flag);                                              %Коэффициенты фильтра
fvtool(b,1,'Fs',Fs);                    
cnt2=len;
k=0:(length(tempShot)-1);
y=filter(b,1,tempShot);                                                     %Результирующий сигнал                                                 

plot(k,tempShot,k,y); %Figure3
legend('Нефильтрованный', '200-й порядок');