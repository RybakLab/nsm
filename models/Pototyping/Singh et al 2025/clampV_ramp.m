Vapp(1:t_ar(1)) = V0; Vapp(t_ar(Nta-1)+1:t_ar(Nta)) = V0;
time_up = t_ar(2)-t_ar(1); slope_up = (V1-V0)/time_up; t_up = 1:time_up;
Vapp(t_ar(1)+1:t_ar(2)) = V0+t_up*slope_up;
time_down = t_ar(3)-t_ar(2); slope_down = (V0-V1)/time_down; t_down = 1:time_down;
Vapp(t_ar(2)+1:t_ar(3)) = V1+t_down*slope_down;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%    CALCULATION OF STEADY STATE VARIABLES PRIOR AND POST RAMP
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
[kdr,nap,km,cal,kca] = calc_steady_states(V0,Cain0);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%    INITIALIZATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%	
ninf_pre = kdr(1);  %% Kdr    
hPinf_pre = nap(2); %% NaP  
zinf_pre = cal(2);  %% CaL
nMinf_pre = km(1);  %% M
if gKCa > 0         %% KCa
    winf_pre = kca(1);
    Ca = Cain0;
end
Ca = Cain0;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   VOLTAGE CLAMP
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
for ita = t_ar(1)+1:length(timeV)
    % ita
    [kdr,nap,km,cal,kca] = calc_steady_states(Vapp(ita),Ca);
    % IC = gC*(Vapp(ita)-Vapp(ita-1));
    Itotal(ita) = 0; 
    %% Kdr
    n = kdr(1)-(kdr(1)-ninf_pre)*exp(-deltaT/kdr(2));  
    ninf_pre = n;
    IKdr_VC(ita) = gKdr*n^4*(Vapp(ita)-EK);
    Itotal(ita) = Itotal(ita)+IKdr_VC(ita);
    %% M
    nM = km(1)-(km(1)-nMinf_pre)*exp(-deltaT/km(2));  
    nMinf_pre = nM;
    IM_VC(ita) = gM*nM*(Vapp(ita)-EK);
    Itotal(ita) = Itotal(ita)+IM_VC(ita);
    %% NaP
    hP = nap(2)-(nap(2)-hPinf_pre)*exp(-deltaT/nap(3)); 
    hPinf_pre = hP;
    INaP_VC(ita) = gNaP*nap(1)*hP*(Vapp(ita)-ENa); 
    Itotal(ita) = Itotal(ita)+INaP_VC(ita);
    %% Ca
    z = cal(2)-(cal(2)-zinf_pre)*exp(-deltaT/cal(3)); 
    zinf_pre = z;
    ICaL_VC(ita) = gCaL*cal(1)*z*(Vapp(ita)-ECa); 
    Itotal(ita) = Itotal(ita)+ICaL_VC(ita);
    %% KCa
    if gKCa > 0
        Ca_VC(ita) = Ca*exp(-deltaT/tauCa)-alfaCa*ICaL_VC(ita)*tauCa*(1-exp(-deltaT/tauCa));
        Ca = Ca_VC(ita);
        w = kca(1)-(kca(1)-winf_pre)*exp(-deltaT/kca(2)); 
        winf_pre = w;
        IKCa_VC(ita) = gKCa*w^2*(Vapp(ita)-EK); 
        Itotal(ita) = Itotal(ita)+IKCa_VC(ita);
    end   
end
