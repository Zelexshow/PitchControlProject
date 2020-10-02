function F = fitness(pid)
    Kp=pid(1);
    Ki=pid(2);
    ref=600000;
    sprintf('The value of interation Kp= %3.0f,Ki= %3.0f', pid(1),pid(2)); 
    % Compute function value
    %simopt = simset('solver','ode5','SrcWorkspace','Current','DstWorkspace','Current');  % Initialize sim options
    [tout,xout,yout] = sim('ModelTestVersion',2);
   %最简单的评价函数，计算方差
    len=size(yout)
    siz=len(1)
    sum=0;
    for i=1:siz
        sum=sum+(yout(i)-ref)^2;
    end
    F=sum
    Kp
    Ki
end

