%%初始化
clear
clc
n=50;%种群数量
bird_step=50;%最大爹代次数，即步伐数
%dim=3;%针对Kp,Ki,Kd三个维度
dim=2;%针对Kp,Ki两个维度
c2=1.2;%加速常数c2
c1=0.12;%加速常数c1
w=0.9;%惯性权重，后期可以考虑采用变化值
%fitness=0*ones(n,bird_step);
gbp=0;%用于存储最优position

  %-----------------------------%
  %    initialize the parameter %
  %-----------------------------%
  
R1=rand(dim,n);
R2=rand(dim,n);
current_fitness=0*ones(n,1);%50×1
  
  %------------------------------------------------%
  % Initializing swarm and velocities and position %
  %------------------------------------------------%
current_position=10*(rand(dim,n)-.5);
velocity=.3*randn(dim,n);
local_best_position=current_position;%3×50
  
  %-------------------------------------------%
  %     Evaluate initial population           %           
  %-------------------------------------------%
  
for i=1:n
  current_fitness(i)=trackAndSet(current_position(:,i));
end
  
local_best_fitness=current_fitness;%50×1
[global_best_fitness,g]=min(local_best_fitness);%min函数得到的是每列最小的数及其对应的行值，[1×1 1×1]

for i=1:n
  global_best_position=local_best_position(:,g);%3×1，代表的是当前最好的位置
end
  
   %-------------------%
   %  VELOCITY UPDATE  %
   %-------------------%
  
velocity=w*velocity+c1*(R1.*(local_best_position-current_position)) + c2*(R2.*(global_best_position-current_position));
   
    %------------------%
    %   SWARM UPDATE   %
    %------------------%
  
current_position=current_position+velocity;

%%主循环
iter=0;%迭代次数
while (iter < bird_step)
    iter=iter+1;

    for i=1:n
        current_fitness(i)=trackAndSet(current_position(:,i));
    end

    for i=1:n
        if current_fitness(i)<local_best_fitness(i)%50×1
            local_best_fitness(i)=current_fitness(i);%50×1
            local_best_position(:,i)=current_position(:,i);%3×50
        end
    end
    %找到全局最优适应度的值及其对应的位置，g为具体的个体位置
    [current_global_best_fitness,g]=min(local_best_fitness);

    if current_global_best_fitness < global_best_fitness 
        global_best_fitness=current_global_best_fitness;
        global_best_position=local_best_position(:,g);
        gbp=global_best_position;
    end

    velocity=w*velocity+c1*(R1.*(local_best_position-current_position)) + c2*(R2.*(global_best_position-current_position));
    current_position=current_position+velocity;

    sprintf('The value of interation iter %3.0f ', iter );
end
%打印最优位置
gbp;
    
    
