%计算fitness函数，同时将值传入到simuLink中
function F = trackAndSet(pid)
         % Track the output of optsim to a signal of 1
        
         % Variables a1 and a2 are shared with RUNTRACKLSQ
         Kp = pid(1);
         Ki = pid(2);
         %Kd = pid(3);
         reference=600000;
         sprintf('The value of interation Kp= %3.0f,Ki= %3.0f', pid(1),pid(2)); 
         % Compute function value
         simopt = simset('solver','ode5','SrcWorkspace','Current','DstWorkspace','Current');  % Initialize sim options
         [tout,xout,yout] = sim('ModelTestVersion',[0 50],simopt);
         e=yout-reference ;  % compute the error 
         sys_overshoot=max(yout)-reference; % compute the overshoot
         
      alpha=10;beta=10;
      F=e^2*beta+sys_overshoot*alpha;
    end

