function [Nburst,burstStart,period,burst] = calc_bursting_par(z)
global trsh
istart = 0;
iend = 0;
NZ = length(z);
burstStart(1) = 0;
burstEnd(1) = 0;
for i = 1:NZ
    if i+1 <= NZ
        if z(i) < trsh && z(i+1) >= trsh
            istart = istart+1;
            burstStart(istart) = i;
        end
    end    
end
if istart > 0
    for i = burstStart(1):NZ
        if i+1 <= NZ
            if z(i) >= trsh && z(i+1) < trsh
                iend = iend+1;               
                burstEnd(iend) = i;
            end
        end
    end
end
Nburst = istart;
period = NaN(Nburst-1,1);
burst = NaN(Nburst-1,1);
if Nburst > 1
    for ib = 1:Nburst-1
        period(ib) = (burstStart(ib+1)-burstStart(ib));
        burst(ib) = (burstEnd(ib)-burstStart(ib));
    end
end
period = period*0.1/1000;
burst = burst*0.1/1000;
burstStart = burstStart*0.1/1000;