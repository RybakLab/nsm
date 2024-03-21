%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % read current simulation file
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%       
    file_name = strcat(sim_name,'.dat')
    fid = fopen(file_name,'r');                   
    Out  = textscan(fid,'%s','delimiter',sprintf('\n'));
    N_Out = length(Out);
    fclose(fid);