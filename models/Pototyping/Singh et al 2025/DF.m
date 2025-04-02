function dx = DF(t,x)
global mV12 mk mtau mtauV12 mtauk hV12 hk htau htauV12 htauk
global mpV12 mpk mptau mptauV12 mptauk hpV12 hpk hptau hptauV12 hptauk
global nV12 nk ntau ntauV12 ntauk 
global nMV12 nMk nMtau nMtauV12 nMtauk                                 
global aKCa bKCa lambdaKCa tauKCa 
global qV12 qk qtau zV12 zk ztau
global C Eleak ENa EK ECa 
global Cain0 Caout alfaCa tauCa
global gNaF gNaP gM gKdr gL gCaL gKCa 
global NaP M CaL KCa kCain 
global mInst mpInst pInact qInst 
global N_mNaF N_hNaF N_mNaP N_hNaP N_M N_Kdr N_qCaL N_zCaL N_KCa N_Cain 
dx = 0*x;
%% membrane potentials
v = x(1);
%% leak 
I = gL.*(v-Eleak);
%% NaF 
m_inf = (1+exp(-(v-mV12)/mk)).^(-1);  
if mInst > 0
    m = m_inf; 
else
    m = x(N_mNaF); %m Naf 
    tau_inf = mtau*(cosh((v-mtauV12)/mtauk))^(-1);     
    dx(N_mNaF) =(m_inf-m)/tau_inf;
end
h = x(N_hNaF);     %h NaF
h_inf = (1+exp(-(v-hV12)/hk))^(-1);  
tau_inf = htau*(cosh((v-htauV12)/htauk))^(-1); 
dx(N_hNaF) = (h_inf-h)/tau_inf;
INaF = gNaF*m^3*h*(v-ENa);
I = I+INaF;
%% NaP
if NaP > 0 
    mp_inf = (1+exp(-(v-mpV12)/mpk)).^(-1);   
    if mpInst > 0
        mp = mp_inf;
    else
        mp = x(N_mNaP);        %mp NaP 
        tau_inf = mptau*(cosh((v-mptauV12)/mptauk))^(-1); 
        dx(N_mNaP) = (mp_inf-mp)/tau_inf; 
    end
    if pInact > 0
        hp = x(N_hNaP);        %hp NaP 
        hp_inf = (1+exp(-(v-hpV12)/hpk))^(-1);   
        tau_inf = hptau*(cosh((v-hptauV12)/hptauk))^(-1); 
        dx(N_hNaP) = (hp_inf-hp)/tau_inf;
        INaP = gNaP*mp*hp*(v-ENa); 
    else
        INaP = gNaP*mp*(v-ENa);
    end
    I = I+INaP;
end
%% Kdr
n = x(N_Kdr);       %n Kdr
n_inf = (1+exp(-(v-nV12)/nk))^(-1);  
tau_inf = ntau*(cosh((v-ntauV12)/ntauk))^(-1);  
dx(N_Kdr) = (n_inf-n)/tau_inf;
IKdr = gKdr*n^4*(v-EK);
I = I+IKdr;
%% M
if M > 0
    nM = x(N_M);                                     %mM KM
    nM_inf = (1+exp(-(v-nMV12)/nMk))^(-1);  
    taunM = nMtau*(cosh((v-nMtauV12)/nMtauk))^(-1); 
    dx(N_M) = (nM_inf-nM)/taunM;
    IM = gM*nM*(v-EK);
    I = I+IM;
end
%% CaL current
if CaL > 0
    q_inf = (1+exp(-(v-qV12)/qk))^(-1); 
    if qInst > 0
        q = q_inf; 
    else
        q = x(N_qCaL); %q, z CaL
        dx(N_qCaL) = (q_inf-q)/qtau;
    end
    z = x(N_zCaL); 
    z_inf = (1+exp(-(v-zV12)/zk))^(-1); 
    dx(N_zCaL) = (z_inf-z)/ztau;
    if Caout > 0
        ICaL = gCaL*q*z*(v-ECa);
        I = I+ICaL;
    else
        ICaL = 0;
    end
end
%% KCa
if KCa > 0
    w = x(N_KCa);  
    Cain = x(N_Cain);
    alfa = aKCa*Cain^lambdaKCa;
    a = 1./(alfa+bKCa);
    w_inf = alfa*a;
    tau_inf = tauKCa*1000*a;
    dx(N_KCa) = (w_inf-w)/tau_inf;
    if Caout > 0
        IKCa = gKCa*w^2*(v-EK);
        I = I+IKCa;
    end
end
%% Ca in concentration 
if kCain > 0
    Cain = x(N_Cain);
    ICa = ICaL;
    dx(N_Cain) = -alfaCa*ICa+(Cain0-Cain)/tauCa;
end
%% membrane potentials
dx(1) = -I/C;












