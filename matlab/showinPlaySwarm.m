function showinPlaySwarm(x, y, args)
% showinPlaySwarm - Animate trajectory data in PlaySwarm.
%
% showinPlaySwarm(x, y) - x trajectory data, y trajectory data
% showinPlaySwarm(r) - complex trajectory data (real -> x, imag -> y)
% showinPlaySwarm(x, y, args) and
% showinPlaySwarm(r, args) - specify a string of command line arguments
%   for PlaySwarm - see PlaySwarm for usage help.  Default is ''.
%
% x- and y-data must have the same dimensions.  The longer dimension is
% assumed to be the time index (i.e. there must be more time samples
% than agents).  The same is true for complex data.
%
% Assumes that PlaySwarm is located in one of the following locations,
% depending on your system type as determined by MATLAB functions 
% ismac, ispc, and islinux:
%
%  * Mac OS X - /Applications/PlaySwarm.app
%  * PC - Not yet implemented
%  * Linux - Not yet implemented
%
% The data is stored in a temporary file called PSdata_tmp.dat in the
% current working directory.  You can change the directory and filename
% by editing showinPlaySwarm.m.
%
% NOTE: The temporary data file is overwritten each time the function
% is called.  You can copy the file to another location if you want to 
% save it - it should still open in PlaySwarm.
%

% By default, we put the data in the current directory.  You can
%  override this by setting this variable to your favorite location, e.g.
%  /tmp, c:\temp, etc...
temp_dir = '.';

% By default the data file is called 'PSdata_tmp.dat', You can override
%  by changing this variable.
data_file = 'PSdata_tmp.dat';

% The following determines the path of PlaySwarm on your system
%  by looking in the standard places.  If you installed it in a different
%  location or are running it on an unimplemented platform, you can try
%  forcing it by e.g. 
%    PlaySwarmPath = '/path/to/PlaySwarm'
PlaySwarmPath = [];
if ismac(),
    PlaySwarmPath = '/Applications/PlaySwarm.app/Contents/MacOS/PlaySwarm';
    if exist(PlaySwarmPath) ~= 2,
        error(sprintf('PlaySwarmPath %s does not exist.  Try setting it manually in this file.', PlaySwarmPath))
    end
elseif ispc(),
    % try a few guessable locations
    PotentialPaths = {...
        'c:\Program Files\PlaySwarm\PlaySwarm.exe'...
        'c:\Program Files (x86)\PlaySwarm\PlaySwarm.exe'...
        'c:\Program Files\MADTraC\PlaySwarm\PlaySwarm.exe'...
        'c:\Program Files (x86)\MADTraC\PlaySwarm\PlaySwarm.exe'...
        };
    for px = 1 : length(PotentialPaths),
        if exist(PotentialPaths{px}) == 2,
            PlaySwarmPath = PotentialPaths{px};
            break;
        end
    end
    if ~isempty(PlaySwarmPath),
        % make sure there are no spaces
        PlaySwarmPath = ['"' PlaySwarmPath '"'];
    end
elseif isunix(),
    error('Unimplemented on linux/unix.  Try setting PlaySwarmPath in this file.')
else
    % fall through to empty path
end

if isempty(PlaySwarmPath),
    error('Unable to determine PlaySwarm path.  Try setting PlaySwarmPath in this file.');
end

% by calling the function with a single argument, we
%  assume it to be a complex representation.
if nargin == 1,
    y = imag(x);
    x = real(x);
    args = '';
end

% if nargin = 2, determine if the second arg is command line arguments
%  or more data
if nargin == 2,
    if ischar(y),
        args = y;
        y = imag(x);
        x = real(x);
    else
        args = '';
    end
end

% make sure x and y have more columns than rows
sizex = size(x);
if sizex(1) > sizex(2),
    x = x.';
end
sizey = size(y);
if sizey(1) > sizey(2),
    y = y.';
end
% compare the sizes after they have possibly changed
sizex = size(x);
sizey = size(y);

% make sure x and y are the same size
if min(sizex == sizey) == 0,
    error('x and y inputs must have the same size')
end

D = [x; y];
datapath = [temp_dir filesep data_file]; 
save(datapath, '-ASCII', 'D');
system([PlaySwarmPath ' ' datapath ' ' args ' &'])