
%本代码用于产生呼吸灯使用的指数函数数据
clear;

x = [0 : 8/19 : 8];       %设置序列 ，指数上升
up = 2.^x ;               %求上升指数序列  
up = uint8(up);           %化为8位数据

y = [8: -8/19 :0];       %设置序列 ，指数下降
down = 2.^y ;            %求下降指数序列
down = uint8(down);      %化为8位数据

line = [[0:8/19:8],[8:8/19:16]]         %拼接序列
val = [up , down]                       %拼接输出序列

dlmwrite('index_wave.c',val);       %输出到文件index_wave.c
plot(line,val,'.');                 %显示波形图
