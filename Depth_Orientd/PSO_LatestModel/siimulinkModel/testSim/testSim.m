[tout,xout,yout] = sim('tmp',5);
ref=1;
len=size(yout)
size=len(1)
sum=0;
for i=1:size
    sum=sum+(yout(i)-ref)^2
end
sum/size
