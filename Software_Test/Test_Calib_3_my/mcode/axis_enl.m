function a2= axis_enl(k, minWH)
%
% function a2= axis_enl(k)
%
% axis enlarge by a factor k
% minWH: 1x1: minimum window size

% 30/8/99, 8/2/01 (bug in h), 1.6.08 (nargout<1), J. Gaspar

if nargin<1,
   k=0.1;
end
if nargin<2
   minWH=1;
end

a= axis;
w= a(2)-a(1)+minWH;
h= a(4)-a(3)+minWH;
if length(a)==4
    a= a + [-k*w +k*w -k*h +k*h];
else
    a= a + [-k*w +k*w -k*h +k*h -k*h +k*h]; % 3d plot
end

if nargout<1
   axis(a)
else
   a2= a;
end
