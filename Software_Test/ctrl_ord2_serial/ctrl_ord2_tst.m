function ret= ctrl_ord2_tst( tstId, a1,a2,a3 )

% Usage to show last file:
% ctrl_ord2_tst('sh_last_data')
%
% Usage to show selected files (maybe more than one):
% ctrl_ord2_tst('sh_selected_data')
%
% Usage to acquire data:
% ctrl_ord2_tst('acq_data')

% Nov2023 J. Gaspar

if ~voptions('istrue', 'doneCnf') && exist('ctrl_ord2_tst_cnf.m', 'file')
    % allow once having a editable configuration
    % clear doneCnfFlag: ctrl_ord2_tst('*','set','doneCnf', 0);
    % see current doneCnfFlag: ctrl_ord2_tst('*','get','doneCnf')
    voptions('set', 'doneCnf', 1);
    ctrl_ord2_tst_cnf();
end

if nargin<1
    %tstId= 'sh_version';
    %tstId= 'signal_resp';
    %tstId= 'acq_data';
    tstId= 'sh_last_data';
    %tstId= 'sh_selected_data';
    %tstId= 'sh_last_selected_data';
end

switch tstId
    case 'sh_version', run_commands_list_and_save('?');
    case 'acq_data', acq_data_step_resp();
    case 'signal_resp', acq_data_signal_resp();
    case {'2', 'show_adc0_2Hz'}, acq_data_2Hz  % ctrl_ord2_tst 2
        % usages ** requiring ** one USB/connected Arduino/WeMos:
        % ctrl_ord2_tst sh_version
        % ctrl_ord2_tst acq_data
        % ctrl_ord2_tst signal_resp
        % ctrl_ord2_tst show_adc0_2Hz
    
    case 'sh_last_data', tst_show_last_data();
    case 'sh_selected_data', tst_show_selected_data();
    case 'sh_last_selected_data', tst_show_selected_data(1);
        % usages on saved data:
        % ctrl_ord2_tst sh_last_data
        % ctrl_ord2_tst sh_selected_data
        % ctrl_ord2_tst sh_last_selected_data

    case '*'
        if strcmp(a1,'get'), a3= []; end
        ret= voptions(a1,a2,a3);
    case '*all', ret= voptions('get_all');
        % usages for this fn configuration:
        % ctrl_ord2_tst *all
        % ctrl_ord2_tst('*','set','twoBlades',1) 
        % ctrl_ord2_tst('*','set','twoBlades',0) 

    otherwise
        error('inv tstId');
end


% -----------------------------------------------------------------------
function acq_data_signal_resp(options)
if nargin<1
    options= [];
end
options.doEndFlag= 0; % do not close the serial port

% make a signal in the Arduino memory
% (make a square wave filling the array)
tx= mk_array([], [1 0 1 0 0]*255);
nv= array_resize(tx);
put_signal_into_the_buffer(nv, options);

% % future: create standard [aIni aStep aEnd v]
% err   nv= [0 2 124 255; 124 2 248 0; 248250 2 499 0];
% 0 124 248 372 498
% 2 2 2 2

% run the acquisition
ofname= mk_fname;
cLst= {'m30', 'T1', 'r0', 's', ['<' ofname], 'o0'};
run_commands_list_and_save( cLst, ofname, struct('doIniFlag',0) );


% -----------------------------------------------------------------------
function acq_data_2Hz
% init comms to the Arduino, usage: ctrl_ord2_tst 2
if ~local_arduino_ini
    return
end

% run commands at the Arduino
for i=1:1e3
    fprintf('i=%d ', i);
    arduino_cmd('i0')
    if aborttst, break; end;
    pause(.5);
end

% close comms to the Arduino
arduino_end;


% -----------------------------------------------------------------------
function acq_data_step_resp
% usage: tst acq_data

%arduino_ini; arduino_cmd({'C', 's', '<'}); arduino_end
%arduino_ini; arduino_cmd({'C', 'r0', 's', '<'}); arduino_end
%arduino_ini; arduino_cmd({'r100', 's', '<'}); arduino_end
%arduino_ini; arduino_cmd({'r900', 's', '<'}); arduino_end
%arduino_ini; arduino_cmd({'r900', 'x', '<'}); arduino_end

% cLst= {'o0', 'D1000', 'r200', 's', '<'};
% cLst= {'o0', 'D1000', 'c02', 'c10', 'c30', 'C', 'r800', 'x', '<'};
% cLst= {'o0', 'D1000', 'c00.35', 'c1-0.314', 'c3-1', 'C', 'r800', 'x', '<'};

ofname= mk_fname;

% cLst= {'r0', 's', 'r200', 's', '<'};
% cLst= {'o0', 'T2000', 'r200', 's', '<'};
% cLst= {'m0', 'T500', 'r200', 's', '<', 'o0'};
% cLst= {'o200', 'D500', 'T500', 'r0', 's', '<', 'o0'};
% cLst= {'T500', 'm5', 'r200', 's', '<', 'o0'};
% cLst= {'m6', 'r200', 's', '<', 'o0'};
% cLst= {'m4', 'T100', 'r255', 's', '<', 'o0'};
% cLst= {'m8', 'r100', 's', '<', 'o0'};
% cLst= {'m7', 'r100', 's', '<', 'o0'};
% cLst= {'m8', 'r200', 's', '<', 'o0'};
% cLst= {'<'};
% cLst= {'<tst_v0_res.mat'};
% cLst= {'m9', 'r100', 's', '<tst_v0_res.mat', 'o0'};
% cLst= {'m9', 'r100', 's', ['<' ofname], 'o0'};
% % cLst= {'m9', 'r100', 's', '<', 'o0'};
% cLst= {'m9', 'r80', 's', ['<' ofname], 'o0'};
% cLst= {'m3', 'r80', 's', ['<' ofname], 'o0'};
% cLst= {'m3', 'T100', 'r80', 's', ['<' ofname], 'o0'};
% cLst= {'m3', 'T10', 'r80', 's', ['<' ofname], 'o0'};
% cLst= {'m3', 'T10', 'r255', 's', ['<' ofname], 'o0'};
% cLst= {'m31', 'T10', 'r255', 's', ['<' ofname], 'o0'};
% cLst= {'m31', 'T1', 'r255', 's', ['<' ofname], 'o0'};
% cLst= {'m9', 'r100', 's', ['<' ofname], 'o0'};
% cLst= {'m31', 'T1', 'r255', 's', ['<' ofname], 'o0'};
% cLst= {'m4', 'T1', 'r255', 's', ['<' ofname], 'o0'};
% cLst= {'m4', 'T100', 'r255', 's', ['<' ofname], 'o0'};
% cLst= {'m4', 'T1000', 'r255', 's', ['<' ofname], 'o0'};
% cLst= {'m31', 'T1', 'r255', 's', ['<' ofname], 'o0'};
% cLst= {'m31', 'T1', 'r50', 's', ['<' ofname], 'o0'};
% cLst= {'m9', 'r200', 's', ['<' ofname], 'o0'};
% cLst= {'m31', 'T1', 'r100', 's', ['<' ofname], 'o0'};
% cLst= {'m31', 'T1', 'r200', 's', ['<' ofname], 'o0'};
% cLst= {'m33', 'T1', 'r200', 's', ['<' ofname], 'o0'};
% cLst= {'m33', 'T1', 'r255', 's', ['<' ofname], 'o0'};
cLst= {'m33', 'T1', 'r150', 's', ['<' ofname], 'o0'};

% finally run all commands included the cLst cell array
run_commands_list_and_save( cLst, ofname );


% -----------------------------------------------------------------------
function okFlag= local_arduino_ini(options)
if nargin<1
    options= [];
end
if isfield(options, 'doIniFlag') && ~options.doIniFlag
    % upper flag says not to ini
    okFlag= 1;
    return
end

try
    arduino_ini;
    okFlag= 1;
catch
	arduino_end;
    disp('** failed arduino_ini() **');
	lasterror
    okFlag= 0;
end


function run_commands_list_and_save( cLst, ofname, options )
if nargin<2
    ofname= '';
end
if nargin<3
    options= [];
end

% init comms to the Arduino
if ~local_arduino_ini(options);
    return
end

% run commands at the Arduino
try
	arduino_cmd(cLst);
catch
    lasterror
	arduino_end;
	return
end

% close comms to the Arduino
arduino_end;

% save data to a local file
if ~isempty(ofname)
    save( ofname, '-append', 'cLst');
    if voptions('istrue', 'twoBlades')
        setupName= 'TwoBlades';
        save( ofname, '-append', 'setupName');
    end
    show_mat_file( ofname )
end


% -----------------------------------------------------------------------
function tx= mk_array( t, x )
if isempty(t)
    N= length(x);
    t= (0:N-1)'/(N-1);
end
tx= [t(:) x(:)];


function nv= array_resize( nv )
% Nx2 -> 500x2
N= 499;
t= (0:N)'/N;
v= interp1( nv(:,1), nv(:,2), t, 'previous' );
%nv= [t(:) v(:)];
nv= [(0:N)' round(v(:))];


function cmdList= mk_list_of_refs( nv )
% nv: Nx2 : arrayAddress value
cmdList= {};
nRange= 1:size(nv,1);
for n= nRange
    s1= sprintf('r%d', round(nv(n,2)) );
    s2= sprintf('>%d', round(nv(n,1)*2) ); % using just even addr
    if n==nRange(1) || abs(nv(n,2)-nv(n-1,2))>1e3*eps
        cmdList{end+1,1}= s1;
        cmdList{end+1,1}= s2;
    else
        cmdList{end+1,1}= s2;
    end
end


function put_signal_into_the_buffer(nv, options)
% init comms to the Arduino
if ~local_arduino_ini(options)
    return
end

% % run commands at the Arduino
% nRange= 1:size(nv,1);
% for n= nRange
%     s1= sprintf('r%d', round(nv(n,2)) );
%     s2= sprintf('>%d', round(nv(n,1)*2) ); % using just even addr
%     if n==nRange(1) || abs(nv(n,2)-nv(n-1,2))>1e3*eps
%         arduino_cmd( {s1, s2} );
%     else
%         arduino_cmd( s2 );
%     end
% end
% % arduino_cmd('r');
% % arduino_cmd('<')

% run commands at the Arduino
cmdList= mk_list_of_refs( nv );
for i= 1:length(cmdList)
    arduino_cmd( cmdList{i} );
end

% close comms to the Arduino
if ~isfield(options, 'doEndFlag') || options.doEndFlag
    arduino_end;
end
return


% -----------------------------------------------------------------------
function fname= mk_fname
fname= mkfname( 'tst_res/tst_', 'mat', struct('outputFormat',3) );


function tst_show_last_data
ofname= filenames_last_only('tst_res/tst_*.mat');
show_mat_file( ofname )


function tst_show_selected_data( useLastFName )
if nargin<1
    useLastFName= 0;
end

if useLastFName
    % use the last filename used
    ofname= voptions('get', 'fname');
    pn= '';
    fn= {ofname};
else
    % ask the filename(s)
    [fn, pn]= uigetfile('tst_res/tst_*.mat', 'choose files to show', ...
        'MultiSelect','on');
    if isnumeric(fn) && fn==0
        return
    end
    if ischar(fn)
        fn= {fn};
    end
end

for i= 1:length(fn)
    show_mat_file( [pn fn{i}] );
end


% =======================================================================
function show_mat_file( ofname )
if isempty( ofname )
    ofname= voptions('get', 'fname');
end
if ~exist(ofname, 'file')
    return
end

x= load(ofname);
% x = 
%        dataStr: 'nLines=500 ti[us]=8839918 (n v1 v2 t1 t2):�'
%     dataValues: [500x5 double]
voptions('set', 'fname', ofname); % save the filename seen

loopMode= '0';
if isfield(x, 'cLst')
    for i= 1:length(x.cLst)
        str= x.cLst{i};
        if str(1)=='m' && ('0'<=str(2) && str(2)<='9')
            loopMode= num2str(str(2:end));
        end
    end
end

switch loopMode
    case '3',  show_A0( x )
    case {'31', '32', '33'}
        % single time series
        show_A0( x, struct('timeCols', 4:5) )
    case '30'
        % two time series
        show_A0( x, struct('timeCols', 4:5, 'keepTimeNCols',1) )
    case '4'
        % using the potentiometer
        show_A0( x, struct('timeCols', 4:5, 'keepTimeNCols',1, 'derivValues',1) )
    case '9'
        show_time_diff( x )
    otherwise
        error('yet to implement')
end
show_title( ofname, x )


% -----------------------------------------------------------------------
function show_time_diff( x )
t= x.dataValues(:,4:5)';
% figure; plot(diff(t(:))/1e3, '.-')

t = t(:)/1e3; t= t-t(1);
dt= diff(t); % msec
v = 180./dt; % deg per sec

ind= find(dt<0);
if ~isempty(ind)
    ind= ind(1);
    t= t(1:ind);
    dt= dt(1:ind-1);
    v= v(1:ind-1);
end

% plot the data
figure
plot( t(2:end), v, '.-' )
xlabel('t [msec]')
ylabel('v [deg/sec]')


% -----------------------------------------------------------------------
function show_A0( x, options )
if nargin<2
    options= [];
end
timeCols= default_value(5, options, 'timeCols');

t0= [];
if voptions('istrue','show_t0_t1')
    % get t0 from x
    t0= sscanf_t0( x );
end

if isfield(options, 'keepTimeNCols') && options.keepTimeNCols
    % keep multiple signals
    % rescale time
    t= x.dataValues(:,timeCols);
    v= x.dataValues(:,timeCols-2);
    t = t(:,:)/1e3; %t= t-t(1);
    %v = v(:);
else
    % reshape to single data signal
    % rescale time
    t= x.dataValues(:,timeCols)';
    v= x.dataValues(:,timeCols-2)';
    t = t(:)/1e3; %t= t-t(1);
    v = v(:);
end
if isempty(t0)
    t= t-t(1);
else
    t= t-t0/1e3;
end

% crop signals if time goes back
ind= find(diff(t)<0);
if ~isempty(ind)
    ind= ind(1);
    t= t(1:ind);
    v= v(1:ind);
end

% plot the data
figure
subplot(211)
plot( t, v, '.-' )
xlabel('t [msec]')
ylabel('v *3.3/1024[V]')

subplot(212)
if isfield(options, 'derivValues') && options.derivValues
    % estimates of rotation speed based on the potentiometer
    dt= diff(t);
    v2= diff(v)./dt;
    t2= t(1:end-1,:);
    plot(t2, v2, '.-');
else
    % pulses indicate rotation speed
    % two cases to consider: N-slits or 2-blades (twoBlades)
    % figure
    if voptions('istrue', 'twoBlades') || ...
            (isfield(x, 'setupName') && strcmp(x.setupName,'TwoBlades'));
        % long + short = total time
        % ctrl_ord2_tst('*','set','twoBlades',1);
        plot_rps_2_blades( t, v );
    else
        %thr= 300;
        thr= median(v);
        [t2, ~]= find_events( t,v, thr );
        plot(t2(1:end-1), 1./diff(t2), '.-')
        ylabel('speed')
    end
end
xlabel('t [msec]')

subplot(211)


function t0= sscanf_t0( x )
p= 'ti[us]=';
i= strfind(x.dataStr, p); i= i+length(p);
if ~isempty(i)
    t0= sscanf(x.dataStr(i:i+20), '%d', 1);
else
    t0= 0;
end
return


function [t2, ix]= find_events( t,v, thr )
t2= t(1);
ix= 1;

if v(1)<=thr
    findRisingEdge= 1;
else
    findRisingEdge= 0;
end

for i= 2:length(t)
    switch findRisingEdge
        case 1
            if v(i)>thr
                t2(end+1)= t(i);
                ix(end+1)= i;
                findRisingEdge= 0;
            end
        case 0
            if v(i)<=thr
                t2(end+1)= t(i);
                ix(end+1)= i;
                findRisingEdge= 1;
            end
    end
end

return


function plot_rps_2_blades( t, v )
%thr= 300;
thr= median(v);
[t2, ~]= find_events( t,v, thr ); % up and down crossings of thr

if 0
    % at 2nd events show speed
    t3= t2(1:2:end);
    plot(t3(1:end), [0 1./diff(t3)*1e3]/2, '.-')
    ylabel('speed [rps]')
else
    % redefine times as middle-times of blades passing
    t3= t2(1:2:end);
    t4= [0 conv(t3(:)', [1 1]/2, 'valid')];
    plot(t4, [0 1./diff(t3)*1e3]/2, '.-')
    ylabel('speed [rps]')
end


function show_title( ofname, x )
% show the cLst if available
[~,f,e]= fileparts(ofname);
str= [f e];
if isfield(x, 'cLst')
    str= [str 10];
    for i= 1:length(x.cLst)
        str= [str ' ' x.cLst{i}];
    end
end
str= strrep(str, '_', '\_');
title(str)


% =======================================================================
function ret= default_value( defaultOpt, options, optName )
% usage example:
% options.xpto = default_value( 123, options, 'xpto' );
if isfield(options, optName)
    ret= getfield( options, optName );
else
    ret= defaultOpt;
end


function ret= voptions(op, a1, a2)
global VOPT
ret= [];
switch op
    case 'set', VOPT.(a1)= a2;
    case 'get'
        if isfield(VOPT, a1)
            ret= VOPT.(a1);
        else
            ret= [];
        end
    case 'istrue'
        ret= (isfield(VOPT,a1) && VOPT.(a1));
    case 'get_all'
        ret= VOPT;
end
