function [xm,fv] = AdapPSO(fitness,N,c1,c2,wmax,wmin,M,D)
%ѧϰ����c1
%ѧϰ����c2
%����Ȩ�����ֵ wmax
%����Ȩ�����ֵ wmin
%���������� M
%�����ռ�ά�� D
% ��ʼ��Ⱥ�������Ŀ N
format long;

%%%%%%%%%%%%%%%%%%%%%%%%��ʼ����Ⱥ�ĸ���%%%%%%%%%%%%%%%%%%%%%
for i=1:N
    for j=1:D
        x(i,j)=randn;
        v(i,j)=randn;
    end
end
%%%%%%%%%%%%%%%%%%%%%%%%����������ӵ���Ӧ��%%%%%%%%%%%%%%%%%%%%%
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
%%%%%%%%%%%%%%%%%%%%%%%%������ѭ��%%%%%%%%%%%%%%%%%%%%%
for t=1:M
    for j=1:N
        fv(j)=fitness(x(j,:));
    end
    fvag=sum(fv)/N;
    fmin=min(fv);
    
    for i=1:N
        if fv(i)<=fvag
            w=wmin+(fv(i)-fmin)*(wmax-wmin)/(fvag-fmin);
        else
            w=wmax;
        end
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
end
xm=pg';%Ŀ�꺯��ȡ��Сֵʱ���Ա���
fv=fitness(pg);%Ŀ�꺯����Сֵ

