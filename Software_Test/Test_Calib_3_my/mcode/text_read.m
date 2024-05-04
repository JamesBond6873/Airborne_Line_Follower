function y= text_read(filename)
%
% Load a text file into a list of strings.
% The list does not contain end of line characters (CR, LF, CRLF, LFCR).

% March 2013, JG

fid = fopen(filename);
if fid<1
    error(['Opening file: ' filename])
end

y = {};
tline = fgetl(fid);
while ischar(tline)
    y{end+1}= tline;
    tline = fgetl(fid);
end
fclose(fid);
