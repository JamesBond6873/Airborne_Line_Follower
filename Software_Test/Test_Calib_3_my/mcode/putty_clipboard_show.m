function putty_clipboard_show( tstId )
if nargin<1
    tstId= 4;
end
if length(tstId)>1
    for i= tstId, putty_clipboard_show(i); disp('press ret to continue'); pause; end
    return
end

switch tstId
    case 1, fname= 'putty_clipboard.txt';
    case 2, fname= 'putty_clipboard2.txt'; % using t0undefined
    case 3, fname= 'putty_clipboard3.txt'; % cut most of prints in check color
    case 4, fname= 'putty_clipboard4.txt'; % myserialprint() empty, sike due to putty
end

putty_clipboard_show_main( fname )


function putty_clipboard_show_main( fname )
% load data from the text file
x= text_read(fname);

% select lines starting by 't='
ind= [];
for i=1:length(x)
    str= x{i};
    if length(str)>=2 && str(1)=='t' && str(2)=='='
        ind(end+1)= i;
    end
end
ind= ind(end:-1:1);
y= x(ind);

% example of one line, y{1} :
% t=62244 Q= 0 0 0 0 0 0 0 0 RGB= 0 0 0 Dist=0.00 t=62245 S= -255 255
z= [];
for i=1:length(y)
    z(end+1,:)= sscanf(y{i}, 't=%d Q= %d %d %d %d %d %d %d %d RGB= %d %d %d Dist=%f t=%d S= %d %d')';
end

% get the time
% z(1,:)= 62244 0 0 0 0 0 0 0 0 0  0  0  0  62245 -255 255
%         1     2 3 4 5 6 7 8 9 10 11 12 13 14    15   16
t= z(:,[1 14]); t= t-min(min(t));
dt= diff(t,1,1);

figure(201); clf
subplot(211); plot( t, '.-' ); title('time [msec] vs sample number')
subplot(212); plot( t, '.-' ); title('zoom of: time [msec] vs sample number')
a= axis; a(2)=20; axis(a);

figure(202); clf
subplot(311); plot( dt, '.-' ); title('time deltas [msec] vs sample number'); axis_enl;
subplot(312); plot( dt, '.-' ); title('zoom of: time deltas [msec] vs sample number');
a= axis; a(2)=20; axis(a); axis_enl;
subplot(313); plot( dt, '.-' ); title('zoom of: time deltas [msec] vs sample number')
a= axis; a(1)=10; a(2)=20; a(4)=1.2*max(dt(10:20,1)); axis(a);

return
