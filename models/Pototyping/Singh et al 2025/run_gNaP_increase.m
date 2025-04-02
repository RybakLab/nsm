clear all;
tic;
global mInst mpInst pInact qInst
global NaF NaP M CaL KCa kCain 
%% MODEL CONFIG
%currents  and concentrations included into neuron model if 1 
%      NaF NaP Kdr KM CaL KCa Cain
%       1   2   3   4   5   6   7  
neu = [ 1   1   1   1   1   1   1  ];
NaF = neu(1); NaP = neu(2); M = neu(4); 
CaL = neu(5); KCa = neu(6); kCain = neu(7); 
mInst = 1;                             % NaF activation is instant if mInst=1
mpInst = 1;                            % NaP activation is instant if mpInst=1 
pInact = 1;                            % no NaP inactivation if pInact=0
qInst = 1;                             % CaL activation is instant if qInst = 1 
num = 1+NaF+CaL+M+kCain+KCa; 
N_DE = 1+num+(1-mInst)*NaF+(1-mpInst+pInact)*NaP+(1-qInst)*CaL; 
param_neuron;
param_currents;
%% SIMULATION PARAM;
time_max = 20000;                      % simulation time
t1 = 10000;                            % show plots starting at t1
spike_thr = -35;                       % thresholt to determine a spike
block = 500;                           % draw plots for each block
Nstep = 10; step = 1/Nstep;            % integration step
%% PARAM VARIATION
par1_name = 'gNaP';
par1 = [0.4 1];
% par1 = [0.4 1];
NP1 = length(par1);

par2_name = 'gKdr';
par2 = [150]; 
% par2 = [150]; 
NP2 = length(par2);

set_sim_description;
figure(1);
for ip1 = 1:NP1
    for ip2 = 1:NP2
        gNaP  = par1(ip1);                     % gNaP      
        gKdr = par2(ip2);                      % Eleak
        % set initial conditions
        x0 = zeros(N_DE,1);
        initialization;        
        nb = -block;
        time = 0:step:time_max;
        options = odeset('JPattern',jpattern(N_DE));
        x = zeros(time_max*Nstep+1,N_DE);
        % ODE solution
        for tb = 0:block:time_max-block
            nb = nb+block;
            par1(ip1)
            par2(ip2)
            tbe = tb+block;
            tib = tb:step:tbe;
            [t,xx] = ode15s(@DF,tib,x0,options);
            x0 = xx(block*Nstep+1,1:N_DE);
            x(nb*Nstep+1:tbe*Nstep+1,1:N_DE) = xx;  
        end
        plot_V;
    end
end    
toc;
