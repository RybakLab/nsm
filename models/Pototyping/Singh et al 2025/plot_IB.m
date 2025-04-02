PIC = min(IB(1:t_array(2)*10));
PICdesc = min(IB(t_array(2)*10+1:length(Itotal)));
subplot(5,5,pos1+ib);
plot(timeV,Itotal,'k','LineWidth',lwidth);
hold on;
plot(timeV,IB,'r','LineWidth',lwidth);
hold on ;
ylabel(' ', 'FontSize',10,'FontWeight','Bold');
xlabel(' ');
% txt_PICs;
if abs(PIC) < 0.1 && abs(PICdesc) < 0.1
    text(txt0,txt3,'no PICs','FontSize',10,'FontWeight','bold','Color','r');
elseif abs(PIC) < 0.1 && abs(PICdesc) >= 0.1
    text(txt0,txt2,'no PICa','FontSize',10,'FontWeight','bold','Color','r');
    text(txt0,txt3,strcat('PICd=',num2str(PICdesc,'%.2f')),'FontSize',10,'FontWeight','bold','Color','r');
elseif abs(PIC) >= 0.1 && abs(PICdesc) < 0.1
    text(txt0,txt2,strcat('PICa=',num2str(PIC,'%.2f')),'FontSize',10,'FontWeight','bold','Color','r');
    text(txt0,txt3,'no PICd','FontSize',10,'FontWeight','bold','Color','r');
else
    text(txt0,txt2,strcat('PICa=',num2str(PIC,'%.2f')),'FontSize',10,'FontWeight','bold','Color','r');
    text(txt0,txt3,strcat('PICd=',num2str(PICdesc,'%.2f')),'FontSize',10,'FontWeight','bold','Color','r');
end
axis([0 time_maxV IminV ImaxB])
hold on;