%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% analyze data from current file
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%      
str = char(Out{1,1}(5));
Nstr = length(str);
for i = 1:Nstr
    switch str(i)
        case '['
            i0 = i;
        case ','
            i1 = i;
        case ']' 
            i2 = i;
        otherwise
    end
end
Tstart = str2double(str(i0+1:i1-1));
Tmax = str2double(str(i1+1:i2-1));
str = char(Out{1,1}(7));
Nstr = length(str);
for i = 1:Nstr
    switch str(i)
        case '='
            i0 = i;
        case 'm'
            i1 = i;
        otherwise
    end
end
step = str2double(str(i0+2:i1-2));
Ntime = int32(Tmax/step);
Nstart = int32(Tstart/step);
Noutput = Ntime-Nstart;
% output = zeros(Noutput,20);
output = zeros(Noutput,4);
bin0 = 20;  
bin = int32(bin0/step); %recalculated bin (in "integration steps")
% end
ip = 0;
iOut = 7;  
while(1)
    iOut = iOut+1;
    str = char(Out{1,1}(iOut));
    Nstr = length(str);
    switch str(1:3)
        case '<Wa'
            for i = 1:Nstr
                if strcmp(str(i),'m')
                    i0 = i; 
                    break
                end
            end
            ip = ip+1;
            pop_name{ip} = str(i0+1:Nstr-1) 
            it = 0;
            while(1)
                iOut = iOut+1;
                str = char(Out{1,1}(iOut)); 
                if (strcmp(str,'</Wave form>')) 
                     break;
                else             
                    wave = str2double(str);
                    it = it+1;
                    output(it,ip) = wave;
                end
            end
        case '</R'
            break;
    end
end
Npop = ip;
% time = (Tstart+0.1:0.1:Tmax)/1000;
time = (Tstart:0.1:Tmax)/1000;
TS = Tstart/1000;
TM = Tmax/1000;
TL = TM-TS;
