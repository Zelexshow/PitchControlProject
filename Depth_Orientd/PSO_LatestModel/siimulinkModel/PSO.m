function[xm,fv]=PSO(fitness,N,c1,c2,w,M,D)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%ѧϰ����c1
%ѧϰ����c2
%����Ȩ��w
%���������� M
%�����ռ�ά�� D
% ��ʼ��Ⱥ�������Ŀ N
format long;
for i=1:N
    for j=1:D
        x(i,j)=randn;
        v(i,j)=randn;
    end
end
for i=1:N
    p(i)=fitness(x(i,:));
    y(i,:)=x(i,:);
end
pg=x(N,:);%PgΪȫ������
for i=1:(N-1)
    if fitness(x(i,:))<fitness(pg)
        pg=x(i,:);
    end
end

for t=1:M
    for i=1:N
        v(i,:)=w*v(i,:)+c1*rand*(y(i,:)-x(i,:))+c2*rand*(pg-x(i,:));
        x(i,:)=x(i,:)+v(i,:);
        if fitness(x(i,:))<p(i)
            p(i)=fitness(x(i,:));
            y(i,:)=x(i,:);
        end
        if p(i)<fitness(pg)
            pg=y(i,:);
        end
    end
    Pbest(t)=fitness(pg);
end
disp('********************************')
xm=pg'
disp('Ŀ�꺯��ȡ��Сֵʱ���Ա�����')
fv=fitness(pg)
disp('********************************')

            