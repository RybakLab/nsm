lwidth = 1; 
txt0 = 4600; txt1 = 100; txt2 = IminV+33; txt3 = txt2-17;
%% PLOT CONTROL
PIC = min(Itotal(1:t_array(2)*10));
PICdesc = min(Itotal(t_array(2)*10+1:length(Itotal)));
subplot(5,5,pos1);
plot(timeV,IKdr_VC,'g','LineWidth',lwidth);
hold on;
plot(timeV,IM_VC,'Color',[102,153,102]/256,'LineWidth',lwidth);
hold on;
plot(timeV,ICaL_VC,'Color',[85,162,251]/256,'LineWidth',lwidth);
hold on;
plot(timeV,INaP_VC,'Color',[254,162,66]/256,'LineWidth',lwidth);
hold on;
if gKCa > 0
    plot(timeV,IKCa_VC,'m','LineWidth',1.);
    hold on;
end
plot(timeV,Itotal,'k','LineWidth',lwidth);
ylb = strcat('Group:',num2str(igr));
ylabel(ylb, 'FontSize',10,'FontWeight','Bold');
tlt = strcat('gNaP=',num2str(gNaP),', hPT=',num2str(hptau));
tlt = strcat(tlt,'; gCaL =',num2str(gCaL),'; gM =',num2str(gM), '; gKCa =',num2str(gKCa));
title(tlt,'FontSize',8,'FontWeight','bold');
xlabel(' ');
text(txt0,ImaxV-30,strcat('PICa=',num2str(PIC,'%.2f')),'FontSize',10,'FontWeight','bold');
text(txt1,ImaxV-30,strcat('PICaexp=',num2str(PICa_exp,'%.2f')),'FontSize',10,'FontWeight','bold','Color','b');
text(txt0,ImaxV-110,strcat('PICd=',num2str(PICdesc,'%.2f')),'FontSize',10,'FontWeight','bold');
text(txt1,ImaxV-110,strcat('PICdexp=',num2str(PICd_exp,'%.2f')),'FontSize',10,'FontWeight','bold','Color','b');
if gM > 0
    text(txt0,ImaxV-190,strcat('IM=',num2str(max(IM_VC),'%.2f')),'FontSize',10,'FontWeight','bold');
    text(txt1,ImaxV-190,strcat('IMexp=',num2str(IM_exp,'%.2f')),'FontSize',10,'FontWeight','bold','Color','b');
end
if pos1 == 21
    legend ('IKdr','IKM','ICaL','INaP','IKCa','total');
end
axis([0 time_maxV IminV ImaxV])
hold on;
%% PLOT BLOCKADE
tltB = {'INaP blocked','ICaL blocked','IM blocked','IKCa blocked'};
posB = [1,1,1,11];
for ib = 1:4
    switch ib
        case 1
            IB = Itotal-INaP_VC;
            plot_IB;
            if pos1 == posB(ib)
                legend ('control','no INaP');
                title(tltB(ib),'FontSize',8,'FontWeight','bold');
            end
        case 2
            IB = Itotal-ICaL_VC;
            if igr >= 3
                IB = IB-IKCa_VC;
            end
            plot_IB;
            if pos1 == posB(ib)
                legend ('control','no ICaL');
                title(tltB(ib),'FontSize',8,'FontWeight','bold');
            end
        case 3
            IB = Itotal-IM_VC;
            plot_IB;
            if pos1 == posB(ib)
                legend ('control','no IM');
                title(tltB(ib),'FontSize',8,'FontWeight','bold');
            end
        case 4
            if gKCa > 0
                IB = Itotal-IKCa_VC;
                plot_IB;
                if pos1 == posB(ib)
                    legend ('control','no IKCa');
                    title(tltB(ib),'FontSize',8,'FontWeight','bold');
                end
            end
        otherwise
           disp('other value')
    end

end
