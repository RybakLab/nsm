tic
clear all
global trsh m_size
trsh = 0.05; m_size = 6;  
N_exp = 4; 
ini_arrays;
time_max = 100; 
sim_name = 'Cat loco model results';
read_file
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% after parsing the current file of simulation results
% we will have simulation time, number of populations (Npop),  
% and histograms of activity for each population   
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
parse_file
step_time0 = 100000;
for iexp = 1:N_exp
    set_exp_param;
    % iexp==1 - intact tied-belt
    % iexp==2 - intact split-belt
    % iexp==3 - transected tied-belt
    % iexp==4 - transected split-belt   
    param = par_min:0.1:par_max;
    step_time = st_min:10:st_max;
    Npar = length(param);
    ave_Blength = NaN(4,Npar); ave_Period = NaN(4,Npar);
    for ipar = 1:Npar                                                                                                                                                
        step_time1 = step_time(ipar)*10000+1;
        step_time2 = step_time1-1+100000;
        output_step_time(1:100000,1:Npop) = output(step_time0+step_time1:step_time0+step_time2,1:Npop); 
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % after calculating bursting parameters for each simulation file 
        % we will have number of bursts (Nb), timing of burst starts (BS),
        % periods of bursting (P) and burst length (B) for each population 
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        Nb = zeros(Npop,1);
        Bstart = NaN(Npop,100); Blength = NaN(Npop,100); Per = NaN(Npop,100);
        for ip = 1:Npop
           [Nb(ip),BS,P,B] = calc_bursting_par(output_step_time(:,ip));
           for ib = 1:Nb(ip)-1
               Bstart(ip,ib) = BS(ib);
               Blength(ip,ib) = B(ib);
               Per(ip,ib) = P(ib);      
           end
           if Nb(ip) > 1
               Bstart(ip,Nb(ip)) = BS(Nb(ip));          
               ave_Blength(ip,ipar) = mean(B(1:Nb(ip)-1));
               ave_Period(ip,ipar) = mean(P(1:Nb(ip)-1));
           end
        end
    end
    figname = strcat(sim_name,'-P,F&E (',title_name,';',title_exp,')'); 
    figure('Name',figname,'NumberTitle','off')
    Ymin = 0; Ymax = 1.2;
    Xmax = 1;
    if iexp == 2 || iexp == 4
        Xmin = 0.45;
    else
        Xmin = 0.35;
    end
    Xst = 0.1; Yst = 0.2;
    plot_results;
    savefig(strcat(figname,'.fig'));
    if iexp == 3 || iexp == 1
        figname = strcat(sim_name,'-P,F&E (',title_name,';',title_exp,')-large'); 
        figure('Name',figname,'NumberTitle','off')
        Ymin = 0; Ymax = 2.5;
        Xmin = 0; Xmax = 1;
        Xst = 0.2; Yst = 0.5;
        plot_results;
        savefig(strcat(figname,'.fig'));
    end
    step_time0 = step_time0+1100000;
end
toc;