tic;
clear all; 
param_currents;
param_neuron;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%    INITIALIZATION FOR VOLTAGE CLAMP
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
nfV = 100;
t_array = [1000; 3500; 6000; 7000]; Nta = length(t_array); 
time_maxV = t_array(Nta); deltaT = 0.1; scaleT = 1/deltaT;
t_ar = scaleT*t_array;
V0 = -80; V1 = -10;
timeV = deltaT:deltaT:time_maxV; NTV = length(timeV);
Vapp = zeros(length(timeV),1);
Itotal = zeros(length(timeV),1);
IKdr_VC = zeros(length(timeV),1);
INaP_VC = zeros(length(timeV),1);
ICaL_VC = zeros(length(timeV),1);
IM_VC = zeros(length(timeV),1);
IKCa_VC = zeros(length(timeV),1);
IminV = -76; ImaxV = 450; ImaxB  = 50;   
VminV = -90; VmaxV = 0;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% PARAMETER VARIATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
figure(2);
for igr = 1:5
    param_groups;
    clampV_ramp;
    plot_group_results;  
end
toc;