for iside = 1:2
    ps = (iside-1)*0.45+0.08;
    pos = [ps 0.4 0.38 0.535];
    subplot('Position',pos);
    for ip = 1:2
        ipop = ip+(iside-1)*2;
        bl = NaN(Npar,1);
        pr = NaN(Npar,1);
        if Nb(ipop) > 1
            for ipar = 1:Npar
                bl(ipar) = ave_Blength(ipop,ipar);
                pr(ipar) = ave_Period(ipop,ipar);
            end
        end
        if ip == 2
            plot_curve_vs_param(param,1,Npar,pr,1,Npar,col_p,'-',1,col_p); hold on;
            plot_curve_vs_param(param,1,Npar,bl,1,Npar,col(2,:),'-',1,col(2,:)); hold on;
        else
            plot_curve_vs_param(param,1,Npar,bl,1,Npar,col(1,:),'-',1,col(1,:));            
        end
        axis([Xmin Xmax Ymin Ymax]);
        xticks(Xmin:Xst:Xmax);
        yticks(Ymin:Yst:Ymax);
        set(gca, 'TickDir', 'out'); 
    end
    title(tlt{iside});       
end
