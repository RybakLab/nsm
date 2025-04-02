if strcmp(par1_name,'Eleak')||strcmp(par2_name,'Eleak')
    str = 'EL-VARIED';
else
    str = strcat('EL=',mat2str(Eleak));
end
strALL = str; 
if strcmp(par1_name,'gL')||strcmp(par2_name,'gL')
    str = ';   gL-VARIED';
else
    str = strcat(';   gL=',mat2str(gL));
end
strALL = strcat(strALL,str); 
if NaF > 0
    if strcmp(par1_name,'gNaF')||strcmp(par2_name,'gNaF')
        str = ';      gNaF-VARIED';
    else
        str = strcat(';      gNaF=',mat2str(gNaF));
    end
    if strcmp(par1_name,'mV12')||strcmp(par2_name,'mV12')
        str = strcat(str,'   mV12-VARIED');
    else
        str = strcat(str,'   mV12=',mat2str(mV12));
    end
    if strcmp(par1_name,'mk')||strcmp(par2_name,'mk')
        str = strcat(str,'   mk-VARIED');
    else
        str = strcat(str,'   mk=',mat2str(mk));
    end
    if strcmp(par1_name,'hV12')||strcmp(par2_name,'hV12')
        str = strcat(str,'   hV12-VARIED');
    else
        str = strcat(str,'   hV12=',mat2str(hV12));
    end
    if strcmp(par1_name,'hk')||strcmp(par2_name,'hVk')
        str = strcat(str,'   hk-VARIED');
    else
        str = strcat(str,'   hk=',mat2str(hk));
    end
    if strcmp(par1_name,'htau')||strcmp(par2_name,'htau')
        str = strcat(str,'   htau-VARIED');
    else
        str = strcat(str,'   htau=',mat2str(htau));
    end
    if strcmp(par1_name,'htauV12')||strcmp(par2_name,'htauV12')
        str = strcat(str,'   htauV12-VARIED');
    else
        str = strcat(str,'  htauV12=',mat2str(htauV12));
    end
    if strcmp(par1_name,'htauk')||strcmp(par2_name,'htauk')
        str = strcat(str,'   htauk-VARIED');
    else
        str = strcat(str,'   htauk=',mat2str(htauk));
    end
    strALL = strcat(strALL,str);
end
if NaP > 0 
    if strcmp(par1_name,'gNaP')||strcmp(par2_name,'gNaP')
        str = ';        gNaP-VARIED';
    else
        str = strcat(';        gNaP=',mat2str(gNaP));
    end
    if strcmp(par1_name,'mpV12')||strcmp(par2_name,'mpV12')
        str = strcat(str,'   mpV12-VARIED');
    else
        str = strcat(str,'   mVp12=',mat2str(mpV12));
    end
    if strcmp(par1_name,'mpk')||strcmp(par2_name,'mpk')
        str = strcat(str,'   mpk-VARIED');
    else
        str = strcat(str,'   mpk=',mat2str(mpk));
    end
    if strcmp(par1_name,'hpV12')||strcmp(par2_name,'hpV12')
        str = strcat(str,'   hpV12-VARIED');
    else
        str = strcat(str,'   hpV12=',mat2str(hV12));
    end
    if strcmp(par1_name,'hpk')||strcmp(par2_name,'hpk')
        str = strcat(str,'  hpk-VARIED');
    else
        str = strcat(str,'   hpk=',mat2str(hpk));
    end
    if strcmp(par1_name,'hptau')||strcmp(par2_name,'hptau')
        str = strcat(str,'   hptau-VARIED');
    else
        str = strcat(str,'   hptau=',mat2str(hptau));
    end
    if strcmp(par1_name,'hptauV12')||strcmp(par2_name,'hptauV12')
        str = strcat(str,'   hptauV12-VARIED');
    else
        str = strcat(str,'   hptauV12=',mat2str(hptauV12));
    end
    if strcmp(par1_name,'hptauk')||strcmp(par2_name,'hptauk')
        str = strcat(str,'   hptauk-VARIED');
    else
        str = strcat(str,'   hptauk=',mat2str(hptauk));
    end
    strALL = strcat(strALL,str);
end
if strcmp(par1_name,'gKdr')||strcmp(par2_name,'gKdr')
    str = ';        gK-VARIED';
else
    str = strcat(';        gKdr=',mat2str(gKdr));
end
if strcmp(par1_name,'nV12')||strcmp(par2_name,'nV12')
    str = strcat(str,'   nV12-VARIED');
else
    str = strcat(str,'   nV12=',mat2str(nV12));
end
if strcmp(par1_name,'nk')||strcmp(par2_name,'nk')
    str = strcat(str,'   nk-VARIED');
else
    str = strcat(str,'   nk=',mat2str(nk));
end
if strcmp(par1_name,'ntau')||strcmp(par2_name,'ntau')
    str = strcat(str,'   ntau-VARIED');
else
    str = strcat(str,'   ntau=',mat2str(ntau));
end
if strcmp(par1_name,'ntauV12')||strcmp(par2_name,'ntauV12')
    str = strcat(str,'   ntauV12-VARIED');
else
    str = strcat(str,'   ntauV12=',mat2str(ntauV12));
end
if strcmp(par1_name,'ntauk')||strcmp(par2_name,'ntauk')
    str = strcat(str,'   ntauk-VARIED');
else
    str = strcat(str,'   ntauk=',mat2str(ntauk));
end
strALL = strcat(strALL,str); 
if M > 0
    if strcmp(par1_name,'gM')||strcmp(par2_name,'gM')
        str1 = 'gM-VARIED';
    else
        str1 = strcat('gM=',mat2str(gM));
    end
    if strcmp(par1_name,'nMV12')||strcmp(par2_name,'nMV12')
        str1 = strcat(str1,'   nMV12-VARIED');
    else
        str1 = strcat(str1,'   nMV12=',mat2str(nMV12));
    end
    if strcmp(par1_name,'nMk')||strcmp(par2_name,'nMk')
        str1 = strcat(str1,'   nMk-VARIED');
    else
        str1 = strcat(str1,'   nMk=',mat2str(nMk));
    end
    if strcmp(par1_name,'nMtau')||strcmp(par2_name,'nMtau')
        str1 = strcat(str1,'   nMtau-VARIED');
    else
        str1 = strcat(str1,'   nMtau=',mat2str(nMtau));
    end
    if strcmp(par1_name,'nMtauV12')||strcmp(par2_name,'nMtauV12')
        str1 = strcat(str1,'   nMtauV12-VARIED');
    else
        str1 = strcat(str1,'   nMtauV12=',mat2str(nMtauV12));
    end
    if strcmp(par1_name,'nMtauk')||strcmp(par2_name,'nMtauk')
        str1 = strcat(str1,'   nMtauk-VARIED');
    else
        str1 = strcat(str1,'   nMtauk=',mat2str(nMtauk));
    end
    strALL1 = str1;
end
if CaL > 0 
    if strcmp(par1_name,'gCaL')||strcmp(par2_name,'gCaL')
        str1 = ';        gCaL-VARIED';
    else
        str1 = strcat(';        gCaL=',mat2str(gCaL));
    end
    if strcmp(par1_name,'qV12')||strcmp(par2_name,'qV12')
        str1 = strcat(str1,'   qV12-VARIED');
    else
        str1 = strcat(str1,'   qV12=',mat2str(qV12));
    end
    if strcmp(par1_name,'qk')||strcmp(par2_name,'qk')
        str1 = strcat(str1,'   qk-VARIED');
    else
        str1 = strcat(str1,'   qk=',mat2str(qk));
    end
    if strcmp(par1_name,'zV12')||strcmp(par2_name,'zV12')
        str1 = strcat(str1,'   zV12-VARIED');
    else
        str1 = strcat(str1,'   zV12=',mat2str(zV12));
    end
    if strcmp(par1_name,'zk')||strcmp(par2_name,'zk')
        str1 = strcat(str1,'   zk-VARIED');
    else
        str1 = strcat(str1,'   zk=',mat2str(zk));
    end
    if strcmp(par1_name,'ztau')||strcmp(par2_name,'ztau')
        str1 = strcat(str1,'   ztau-VARIED');
    else
        str1 = strcat(str1,'   ztau=',mat2str(ztau));
    end
    strALL1 = strcat(strALL1,str1);
    strAll1_lentgh = length(strALL1);
end
if KCa > 0 
    if strcmp(par1_name,'KCa')||strcmp(par2_name,'KCa')
        str1 = ';       KCa-VARIED';
    else
        str1 = strcat(';       KCa=',mat2str(gKCa));
    end
    if strcmp(par1_name,'aKCa')||strcmp(par2_name,'aKCa')
        str1 = strcat(str1,'   aKCa-VARIED');
    else
        str1 = strcat(str1,'   aKCa=',mat2str(aKCa));
    end
    if strcmp(par1_name,'bKCa')||strcmp(par2_name,'bKCa')
        str1 = strcat(str1,'   bKCa-VARIED');
    else
        str1 = strcat(str1,'   bKCa=',mat2str(bKCa));
    end
    if strcmp(par1_name,'lambdaKCa')||strcmp(par2_name,'lambdaKCa')
        str1 = strcat(str1,'   lKCa-VARIED');
    else
        str1 = strcat(str1,'   lKCa=',mat2str(lambdaKCa));
    end
    if strcmp(par1_name,'tauKCa')||strcmp(par2_name,'tauKCa')
        str1 = strcat(str1,'   tauKCa-VARIED');
    else
        str1 = strcat(str1,'   tauKCa=',mat2str(tauKCa));
    end
    strALL1 = strcat(strALL1,str1);
    strAll1_lentgh = length(strALL1);
end
if kCain > 0 
    if strcmp(par1_name,'Cain0')||strcmp(par2_name,'Cain0')
        str1 = ';        Cain0-VARIED';
    else
        str1 = strcat(';        Cain0=',mat2str(Cain0));
    end
    if strcmp(par1_name,'alfaCa')||strcmp(par2_name,'alfaCa')
        str1 = strcat(str1,'   alfaCa-VARIED');
    else
        str1 = strcat(str1,'   alfaCa=',mat2str(alfaCa));
    end
    if strcmp(par1_name,'tauCa')||strcmp(par2_name,'tauCa')
        str1 = strcat(str1,'   tauCa-VARIED');
    else
        str1 = strcat(str1,'   tauCa=',mat2str(tauCa));
    end
    strALL1 = strcat(strALL1,str1);
    strAll1_lentgh = length(strALL1);
end
