function y=firFilter
fid = fopen('C:\qt_pr\STMviewer\STMviewer\MATLAB\2021_06_23__15_07_56_CH2', 'rb');  % открытие файла на чтение 
if fid == -1 
    error('File is not opened'); 
end 
  
B=0;                % инициализация переменной 
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
        temp = shots(k,:);
        
    else
        j=j+1;
        shots(k+1,j)=V(i);
    end
    i=i+1;
end
n=length(shots(1,:));
for j=1:n
    matozh(j)=mean(shots(:,j));
    dispersia(j)=var(shots(:,j));
end
verh = matozh+dispersia;
niz = matozh-dispersia;
plot(matozh);
hold on;
plot(verh);  
plot(niz);  

fid = fopen('C:\qt_pr\STMviewer\STMviewer\MATLAB\2021_06_23__15_07_56_CH2_mean', 'wb');  % открытие файла на чтение 
fwrite(fid,matozh);
% for i=1:k 
%    plot(shots(i,:));
%    hold on;
% end
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
    N=200;      % Order
    Fc1  = 120000;   % First Cutoff Frequency
    Fc2  = 240000;   % Second Cutoff Frequency
    Fc=240000;     % Cutoff Frequency
    flag='scale';  % Sampling Flag
    Beta=6;        % Window Parameter
    %
    % % Create the window vector for the design algorithm.
    win = kaiser(N+1, Beta);
    Wc=2*Fc/Fs;
    Wc1=2*Fc1/Fs;
    Wc2=2*Fc2/Fs;
    %
    % % Calculate the coefficients using the FIR1 function.
b2=fir1(N,[Wc1 Wc2],'bandpass',win,flag);
b=fir1(N,Wc,'low',win,flag);

fid = fopen('koeff.txt','w');
fprintf(fid,'%11.10f\n',b);
fclose(fid);

%fvtool(b,1,'Fs',Fs);
cnt2=10000;
k=0:(cnt2-1);
for i=1:cnt2
 B2(1,i)=V(i+200e3);
 t2(1,i)=(i-1)*dt;
end
fid = fopen('CH3_.txt','w');
fwrite(fid,B2); 
fclose(fid);


%y=filter(b,1,B2);
dx_mas_counter =1;
BL=201;
dx_mas = zeros(BL,1);

for j=1:10000
    
    %dx = data(j,2);
    
    dx_mas(dx_mas_counter) = B2(j);   
     
     
     
      Yx = b(1) ;
    
     
     
     for i = 2:BL
     
       dx_mas_counter_offset =dx_mas_counter+i-2;
       if dx_mas_counter_offset>BL
          dx_mas_counter_offset= dx_mas_counter_offset-(BL);
       end
        
       Yx=Yx+b(i)*dx_mas(dx_mas_counter_offset);
      
         
     
     end    
     
      dx_mas_counter=dx_mas_counter+1;
      if dx_mas_counter == BL+1
      
            dx_mas_counter=1;
      end
     y(j) = Yx; 
    
end


y2=filter(b2,1,B2);
figure
plot(V,'Marker','.'); 
figure
plot(k,B2,k,y,k,y2,'Marker','.'); %Figure3