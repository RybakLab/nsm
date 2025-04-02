function [kdr,nap,km,cal,kca] = calc_steady_states(v,ca)
global gKCa                                               %% ...
global nV12 nk ntau ntauV12 ntauk                         %% Potassium rectifier (Kdr)
global nMV12 nMk nMtau nMtauV12 nMtauk                    %% Potassium M (M)
global mpV12 mpk hpV12 hpk hptau hptauV12 hptauk          %% Persistent sodium (NaP)
global qV12 qk zV12 zk ztau                               %% High-threshold calcium (CaL)
global aKCa bKCa lambdaKCa tauKCa                         %% Calcium-dependent potassium (KCa) 
kdr = NaN; nap = NaN; km = NaN; cal = NaN; kca = NaN;
%% NaP
mPinf = 1/(1+exp(-(v-mpV12)/mpk));
hPinf = 1/(1+exp(-(v-hpV12)/hpk));  
tauhP = hptau/cosh((v-hptauV12)/hptauk);
nap   = [mPinf; hPinf; tauhP];
%% Kdr
ninf = 1/(1+exp(-(v-nV12)/nk));
taun = ntau/cosh((v-ntauV12)/ntauk);
kdr = [ninf; taun];
%% M
nMinf = 1/(1+exp(-(v-nMV12)/nMk));
taunM = nMtau/cosh((v-nMtauV12)/nMtauk);
km = [nMinf; taunM];
%% CaL
qinf = 1/(1+exp(-(v-qV12)/qk));
zinf = 1/(1+exp(-(v-zV12)/zk));
tauz = ztau;
cal  = [qinf; zinf; tauz];
%% KCa
if gKCa > 0
    alfa = aKCa*ca^lambdaKCa;
    a = 1/(alfa+bKCa);
    winf = alfa*a;
    tauw = tauKCa*1000*a;
    kca = [winf,tauw];
end
