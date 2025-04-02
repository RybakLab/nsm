pos = NP2*(ip1-1)+ip2;
fs = 8; 
Xtxt = t1; Ytxt = -80;
Xtxt1 = t1; Ytxt1 = -85; 
figure(1),subplot(NP1,NP2,pos),plot(time,x(:,1),'k','LineWidth',1.);
hold on;
set(gca,'TickDir','out');
set(gca,'TickLength',[0.02 1]);
set(gca,'YTick',[-60 -40 -20  0 ],'FontSize',fs);
if ip2 == 1
    set(gca,'YTickLabel',[-60 -40 -20 0 ],'FontSize',fs);
else 
     set(gca,'YTickLabel',' ','FontSize',fs);
end   
ttick(1) = t1;
for iti = 1:5
    ttick(iti+1) = ttick(iti)+2000;
end
set(gca,'XTick',ttick);
if ip1 == NP1
    set(gca,'XTickLabel',[0 2 4 6 8 10],'FontSize',fs);
else 
     set(gca,'XTickLabel',' ');
end   
set(gca,'LineWidth',1.);
if ip1 == 1
    if ip2 == 1
        str = strcat(strcat(par2_name,'='),mat2str(par2(ip2)));
    else
        str = mat2str(par2(ip2));
    end
    title(str,'FontSize',10,'FontWeight','normal');
else
    title(' ','FontSize',10,'FontWeight','normal');
end
if  ip2 == 1
    if ip1 == NP1
        str = strcat(strcat(par1_name,'='),mat2str(par1(ip1)));
    else
        str = mat2str(par1(ip1));
    end
    ylabel(str,'FontSize',10);
else
    ylabel(' ','FontSize',10);
end
axis([t1 time_max -70 10]);
if ip1 == NP1
    xlabel('time','FontSize',fs);
    if ip2 == 1
        text(Xtxt,Ytxt,strALL,'FontSize',fs,'FontWeight','bold');
        text(Xtxt1,Ytxt1,strALL1,'FontSize',fs,'FontWeight','bold');
    end
else
    xlabel(' ','FontSize',9);
end    
grid on;

