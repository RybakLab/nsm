function S = jpattern(Nplus)
global N 
global NaP CaL M KCa kCain 
global mInst mpInst pInact qInst
global N_mNaF N_hNaF N_mNaP N_hNaP N_Kdr N_M N_qCaL N_zCaL N_KCa  N_Cain 
S = zeros(Nplus,Nplus);
%% NaF 
if mInst == 0
    S(N_mNaF,N_mNaF) = 1; 
    S(1,N_mNaF) = 1; 
    S(N_mNaF,1) = 1; 
end
S(N_hNaF,N_hNaF) = 1; 
S(1,N_hNaF) = 1; 
S(N_hNaF,1) = 1; 
%% NaP
if NaP > 0   
    if mpInst == 0
        S(N_mNaP,N_mNaP) = 1; 
        S(1,N_mNaP) = 1; 
        S(N_mNaP,1) = 1; 
    end
    if pInact > 0
        S(N_hNaP,N_hNaP) = 1; 
        S(1,N_hNaP) = 1; 
        S(N_hNaP,1) = 1; 
    end
end
%% Kdr
S(N_Kdr,N_Kdr) = 1; 
S(1,N_Kdr) = 1; 
S(N_Kdr,1) = 1; 
%% M
if M > 0
    S(N_M,N_M) = 1; 
    S(1,N_M) = 1; 
    S(N_M,1) = 1; 
end
%% CaL current
if CaL > 0
    if qInst == 0
        S(N_qCaL,N_qCaL) = 1; 
        S(1,N_qCaL) = 1; 
        S(N_qCaL,1) = 1; 
    end
    S(N_zCaL,N_zCaL) = 1; 
    S(1,N_zCaL) = 1; 
    S(N_zCaL,1) = 1; 
end
%% Ca in concentration 
if kCain > 0 && CaL > 0
    S(1,N_Cain) = 1; 
    S(N_Cain,N_zCaL) = 1; 
    S(N_Cain,N_qCaL) = 1; 
    S(N_Cain,N_Cain) = 1;   
end
%% KCa
if KCa > 0 && kCain > 0 && CaL > 0
    S(N_KCa,N_KCa) = 1; 
    S(1,N_KCa) = 1; 
    S(N_KCa,N_Cain) = 1; 
end
%membrane potentials
S(1,1) = 1; 











