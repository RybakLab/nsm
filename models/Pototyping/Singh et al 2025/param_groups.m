gKdr =  4;
hptau_nb = 3100; hptau_adult = 22000;
switch igr
    case 1
        % group I
        pos1 = 1; 
        gM = 0.801; 
        gNaP = 0.595;
        gCaL = 0.4; 
        gKCa = 0;
        hptau = hptau_nb; 
        IM_exp = 56.08;
        PICa_exp  = 39.52;
        PICd_exp  = 1.04;
    case 2
        % group II
        pos1 = 6; 
        gM = 0.801; 
        gNaP = 0.389;
        gCaL = 0.27;
        gKCa = 0; 
        hptau = hptau_nb;
        IM_exp = 56.08;
        PICa_exp  = 23.92;
        PICd_exp  = 0.52;
    case 3
        % group III
        pos1 = 11; 
        gM = 0.608; 
        gNaP = 0.397;
        gCaL = 0.5;
        gKCa = 0.95;
        hptau = hptau_adult;
        IM_exp = 42.51;
        PICa_exp  = 29.64;
        PICd_exp  = 0.78;
    case 4
        % group IV
        pos1 = 16; 
        gM = 2.452; 
        gNaP = 0.547;
        gCaL = 0.5;
        gKCa = 0.945; 
        hptau = hptau_adult;
        IM_exp = 171.99;
        PICa_exp  = 29.64;
        PICd_exp  = 0.78;        
    case 5
        % group V
        pos1 = 21; 
        gM = 0.801; 
        gNaP = 0.603;
        gCaL = 0.5;
        gKCa = 0.4; 
        hptau = hptau_adult;
        IM_exp = 56.08;
        PICa_exp  = 45.24;
        PICd_exp  = 17.68;
    otherwise
       disp('other value');
end