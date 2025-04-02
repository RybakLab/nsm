global N_mNaF N_hNaF N_mNaP N_hNaP N_M N_Kdr N_qCaL N_zCaL N_KCa N_Cain  
%% v membrane potentials 
x0(1) = -58.5;   
%% NaF
j = 1;
if mInst == 0                          
    j = j+1;
    N_mNaF = j;
    x0(N_mNaF) = 0.02;             
end
j = j+1;                             
N_hNaF = j;
x0(N_hNaF) = 0.26;  
%% NaP
if NaP > 0
    if mpInst == 0    %mp NaP 
        j = j+1;
        N_mNaP = j;
        x0(N_mNaP) = 0.02;  
    end
    if pInact > 0    %hp NaP
        j = j+1;
        N_hNaP = j;
        x0(N_hNaP) = 0.52;  
    end
end
%% Kdr
j = j+1;                             
N_Kdr = j;
x0(N_Kdr) = 0.008;
%% KM
if M > 0
    j = j+1;                             
    N_M = j;
    x0(N_M) = 0.008;
end
%% CaL
if CaL > 0 
    if qInst == 0
        j = j+1;      
        N_qCaL = j;
        x0(N_qCaL) = 0.003;
    end
    j = j+1;      
    N_zCaL = j;
    x0(N_zCaL) = 0.85;
end
%% Cain 
if kCain > 0  
    j = j+1;      
    N_Cain = j;
    x0(N_Cain) = Cain0;
end
%% KCa
if KCa > 0  
    j = j+1;      
    N_KCa = j;
    x0(N_KCa) = 0.15;
end
% jDE = (j+1)*N;



