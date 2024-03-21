shiftX1 = 0.19; shiftX2 = 0.02; 
shiftY=0.195;
posX0 = 0.02;
switch iexp
    case {1,2}
        model_name = ' intact'; 
        par_min = 0.1;  par_max = 1; 
        Npar1 = 5; Npar2 = 10;
        st_min = 0; st_max = time_max;
        title_name = 'Intact';
        posX = posX0;
        Xmin = 0.5; Xmax = 1;
        col1 = [237 50 55]; col2 = [62 64 149]; col_p = 'k';
        col =  [col1; col2]/256; 
        if iexp == 1
            col_intact = col; col_p_intact = col_p;
        end
    case {3,4}
        model_name = ' spinal';
        par_min = 0.1;   par_max = 1; 
        % Npar1 = 5; Npar2 = 10;
        Npar1 = 6; Npar2 = 10;
        st_min = 0; st_max = time_max;
        title_name = 'Transected';
        posX = posX0+shiftX1;
        if iexp == 3
            Xmin = 0.4; Xmax = 1;
            Npar1 = 4; Npar2 = 10;
        else
            Xmin = 0.5; Xmax = 1;
            Npar1 = 5; Npar2 = 10;
        end
        col1 = [236 38 143]; col2 = [0 0 255]; 
        col =  [col1; col2]/256; col_p = [150 152 154]/256;
         if iexp == 3
            col_spinal = col; col_p_spinal = col_p;
        end       
end
switch iexp
    case {1,3}
        title_exp = 'Tied RGs';
        posY=0.02+shiftY*4;
    case {2,4}
        title_exp = 'Split RGs (Left slow)';
        posY=0.02+shiftY*3; 
end
sizeX = 0.08; sizeY = 0.16; 
Ymax = 1.2;
tlt{1} = 'Left'; tlt{2} = 'Right';
