%%��ʼ��
clear
clc
n=50;%��Ⱥ����
bird_step=50;%��������������������
%dim=3;%���Kp,Ki,Kd����ά��
dim=2;%���Kp,Ki����ά��
c2=1.2;%���ٳ���c2
c1=0.12;%���ٳ���c1
w=0.9;%����Ȩ�أ����ڿ��Կ��ǲ��ñ仯ֵ
%fitness=0*ones(n,bird_step);
gbp=0;%���ڴ洢����position

  %-----------------------------%
  %    initialize the parameter %
  %-----------------------------%
  
R1=rand(dim,n);
R2=rand(dim,n);
current_fitness=0*ones(n,1);%50��1
  
  %------------------------------------------------%
  % Initializing swarm and velocities and position %
  %------------------------------------------------%
current_position=10*(rand(dim,n)-.5);
velocity=.3*randn(dim,n);
local_best_position=current_position;%3��50
  
  %-------------------------------------------%
  %     Evaluate initial population           %           
  %-------------------------------------------%
  
for i=1:n
  current_fitness(i)=trackAndSet(current_position(:,i));
end
  
local_best_fitness=current_fitness;%50��1
[global_best_fitness,g]=min(local_best_fitness);%min�����õ�����ÿ����С���������Ӧ����ֵ��[1��1 1��1]

for i=1:n
  global_best_position=local_best_position(:,g);%3��1��������ǵ�ǰ��õ�λ��
end
  
   %-------------------%
   %  VELOCITY UPDATE  %
   %-------------------%
  
velocity=w*velocity+c1*(R1.*(local_best_position-current_position)) + c2*(R2.*(global_best_position-current_position));
   
    %------------------%
    %   SWARM UPDATE   %
    %------------------%
  
current_position=current_position+velocity;

%%��ѭ��
iter=0;%��������
while (iter < bird_step)
    iter=iter+1;

    for i=1:n
        current_fitness(i)=trackAndSet(current_position(:,i));
    end

    for i=1:n
        if current_fitness(i)<local_best_fitness(i)%50��1
            local_best_fitness(i)=current_fitness(i);%50��1
            local_best_position(:,i)=current_position(:,i);%3��50
        end
    end
    %�ҵ�ȫ��������Ӧ�ȵ�ֵ�����Ӧ��λ�ã�gΪ����ĸ���λ��
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
%��ӡ����λ��
gbp;
    
    
