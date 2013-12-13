% asynchronous_boost_converter_example.m

cd(fileparts(which('asynchronous_boost_converter_example.m')));

tstart = 0;
n = 100;
tend = 5e-4;

%% fixed-step synchonous solver test

clear qtr_async1

% qtr_async1 = synctrcircuit('m-test-2.net');
qtr_async1 = asynctrcircuit('boostconverter.net');

qtr_async1.init(tstart, (tend - tstart) / (10 * n));

% get the number of nodes
N = qtr_async1.getn;

% get the number of voltage sources
M = qtr_async1.getm;

% choose some time points
T1 = linspace(tstart, tend, n)';

Y1 = zeros(numel(T1), M+N);
Y1(1, 1:(N+M)) = qtr_async1.getsolution();

for ind = 2:numel(T1)
    
    fprintf(1, 't: %f\n', T1(ind));
    
	qtr_async1.acceptstep(T1(ind));
    
    Y1(ind, 1:(N+M)) = qtr_async1.getsolution();
    
%     qtr_async.printx()

end

figure; plotyy(T1, Y1(:,1:N), T1, Y1(:,((N+1):(N+M))));

%% ode test

clear qtr_async2

% dbstop in synctrcircuit.m at 203
% dbstop in synctrcircuit.m at 218
% dbstop in synctrcircuit.m at 220

% qtr_async2 = synctrcircuit('m-test-2.net');
qtr_async2 = synctrcircuit('boostconverter.net');

qtr_async2.init(tstart);

y0 = qtr_async2.getsolution();

odeopts = odeset( 'OutputFcn', @synctrcircuit.odeoutputfcn, ...
                  'InitialStep', (tend-tstart)/n/2, ...
                  ... 'MaxStep', (tend-tstart)/n, ...
                  'Refine', 1, ...
                  'RelTol', 1e-2);

[T2, Y2] = ode15s( @synctrcircuit.odefcn, [ tstart+(0.001)*tstart, tend ], y0, odeopts, qtr_async2);

figure; plotyy(T2, Y2(:,1:N), T2, Y2(:,((N+1):(N+M))));


%% comparison


figure; plot(T1, Y1(:,3), 'r', T2, Y2(:,3), 'b');







