function ret= arduino_interf(cmd, arg1)
%
% Assume ctrl_ord2_serial.pde is loaded into the arduino
%   This test makes the arduino led blink some few times

% July 2012, Oct 2015 (def addr), JG

if nargin<1
    if exist('arduino_tst.m','file')
        arduino_tst;
        return
    end
    disp('Usage: arduino_interf(cmd, arg1)');
    return
end
if nargin<2
    arg1= [];
end

global sp1

switch cmd
    case 'ini'
        % initialize connection to the arduino
        spAddressDefault= 'COM3';
        if nargin>=2
            spAddress= arg1;
        elseif exist('arduino_interf_addr.m','file')
            spAddress= arduino_interf_addr;
        else
            spAddress= spAddressDefault;
        end
        if isempty(sp1) || ~isvalid(sp1)
            sp1 = serial(spAddress);
            %set(sp1, 'BaudRate', 9600);
            set(sp1, 'BaudRate', 115200);
            fopen(sp1);
            %get(sp1) % show the properties of the object
            pause(1); % give some time to garantee work done ok
        end
        if ~strcmp(spAddress, spAddressDefault)
            if exist('arduino_interf_addr.m','file') ...
                    && strcmp(spAddress, spAddressDefault)
                % do nothing, just return
                return
            else
                fid= fopen('arduino_interf_addr.m','wt');
                if fid<1
                    warning('cannot write "arduino_interf_addr.m"');
                    return;
                end
                fprintf(fid, 'function y= arduino_interf_addr\n');
                fprintf(fid, 'y= ''');
                fwrite(fid, uint8(spAddress), 'uint8');
                fprintf(fid, ''';\n');
                fclose(fid);
            end
        end
        
    case 'end'
        % end connection to the arduino
        if isvalid(sp1)
            fclose(sp1)
            delete(sp1)
            clear sp1
        end
        
    case 'send_recv'
        % send command and receive answer from the arduino
        ret= NaN;
        if isvalid(sp1)
            ret= send_recv_str(sp1, arg1);
        end
        
    case 'get_array'
        % get the internal state of the buffer
        if isvalid(sp1)
            ret= arduino_get_array_main(sp1, arg1);
        end
        
    case 'debug'
        % generic evaluation of commands (just for debug)
        for i=1:length(arg1)
            eval(arg1{i})
        end
        
    otherwise
        error('invalid cmd')
end

return; % end of main function


% -------------------------------------------------------------
function y= arduino_get_array_main(sp1, fout)

if nargin<2
    fout=[];
end
y= [];

% get data using the serial port
data= send_recv_str(sp1, '<');

% now parse the data...
ind= find(data==':');
y= eval([ '[' data(ind(1)+1:end) '];' ]);

% output data to fout
if ~isempty(fout)
    if save_as_mat(fout, data, y)
        return
    elseif ischar(fout)
        fid= fopen(fout, 'wt');
    else
        fid= fout;
    end
    fprintf(fid, '%s', data);
    if ischar(fout)
        fclose(fid);
    end
end

return

% % OLD code
% data = fgetl(sp1); %flushinput(sp1);
% %nlines= str2num(data);
% 
% ind= strfind(data, '=');
% nlines= sscanf(data(ind(1)+1:end), '%d');
% time0= sscanf(data(ind(2)+1:end), '%f');
% for i=1:nlines
%     data = fgetl(sp1);
%     if ~isempty(fout)
%         fprintf(fout, '(%d:%d) %s\n', i, nlines, data);
%     end
%     y(i,:)= str2num(data);
% end


function flag= save_as_mat(fout, dataStr, dataValues)
flag= 0;
if ~ischar(fout)
    return;
end
[~,~,e]= fileparts( fout );
if ~strcmpi( e, '.mat' )
    return;
end
save( fout, 'dataStr', 'dataValues')
flag= 1;
return


% -------------------------------------------------------------
function y= send_recv_str(sp1, str2send, estr)
if isempty(str2send)
    y= '';
    return;
end

if nargin<3
    estr= '> '; % terminating string
end

% send command, and wait response
%
fwrite(sp1, str2send);
y= fread_till_estr(sp1, estr); y= char(y);

% remove 'str2send' in the begining and 'estr' in the end
%
N1= length(str2send); N2= length(estr); N3= length(y);
if N1+N2 > N3
    warning('string received is too small');
else
    % remove start & end strings
    %y= y(N1+1:end);
    y= y(N1+3:end); % remove also the CR+LF
    y= y(1:end-N2);
    
    % remove end of lines
    ind= find(y~=10); y= y(ind); % Line Feed
    %ind= find(y~=13); y= y(ind); % Carriage Return
end

return


function buff= fread_till_estr(sp1, estr)
done= 0;
buff= [];
N= length(estr);
tf= now +seconds(20);
while ~done
    [y, ylen]= fread(sp1,1);
    buff= [buff, y];
    if length(buff) >= N && strcmp(estr, char(buff(end-N+1:end)))
        done= 1;
    else
        if length(buff) >= N && ~done && has_string(buff, estr)
            %if has_string(buff, estr)
            %    has_string(buff, estr)
            %end
            done= 1;
        end
    end
    % PUT here a timeout...
    if now>tf
        warning('Timed out... current buffer:');
        char(buff)
        break
    end
end
return


function ret= has_string(buff, estr)
ret= 0;
ind= find(buff==estr(1));
for i= ind
    ret= 1;
    for j= 2:length(estr)
        % look for a mismatch
        if i+j-1>length(buff) || buff(i+j-1)~=estr(j)
            ret= 0; break
        end
    end
    if ret
        % found estr
        break
    end
end

function square_wave(sp1)
for i=1:3
    pause(1)
    fwrite(sp1, 'ddo0');
    y= fread_till_estr(sp1, '> '); char(y)
    pause(1)
    fwrite(sp1, 'ddo1');
    y= fread_till_estr(sp1, '> '); char(y)
end
pause(1)
fwrite(sp1, 'ddo0');
y= fread_till_estr(sp1, '> '); char(y)
