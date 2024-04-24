function ret_= arduino_cmd(cmdList)
%
% Arduino interf, 2010.02.23, 2015.10.16 (ret), JG

if ischar(cmdList)
    % single string to workout
    %
    ret= parse_single_cmd(cmdList); % cmdList is a single string

elseif iscell(cmdList)
    % multiple strings to workout
    %
	ret= {};
    for i=1:length(cmdList)
        str= cmdList{i};
        sList= break_strings(str);
        for j=1:length(sList)
            ret{end+1}= parse_single_cmd(sList{j});
        end
    end

else
    % cannot understand the input
    %
    cmdList
    error('invalid input')
end

if nargout>0
    ret_= ret;
end

return; % end of main function


% ------------------------------------------------------------
function sList= break_strings(s)
sList= {};
if isempty(s)
    return;
end

% preserve strings enclosed in "" (can contain spaces)
if s(1)=='"'
    if s(end=='"')
        sList= { s(2:end-1) };
    else
        sList= { s(2:end) };
    end
    return;
end

% break the string
remain= s;
while 1
    [token, remain] = strtok(remain, ' ');
    sList{end+1}= token;
    if isempty(remain)
        break;
    end
end


% ------------------------------------------------------------
function ret= parse_single_cmd(s)

if length(s)>0 && s(1)=='<'
    if length(s)>1
        % output to a file
        arduino_get_array( s(2:end) );
        ret= [];
        return
    end
    % output to screen
    y= arduino_get_array(1); % 1 == show the array while loading
    figure
    %plot(y(:,end)-y(1,end),y(:,2:end-2), '.-')
    plot( (y(:,end)-y(1,end))/1e3, y(:,2:end-2), '.-'); xlabel('t [msec]');
    drawnow
	ret= y;
    return
end
ret= arduino_send_recv(s);
disp(ret)
return
