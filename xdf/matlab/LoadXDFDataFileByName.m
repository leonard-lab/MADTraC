function data = LoadXDFDataFileByName(XDInfo, name, padwith)
%
% data = LoadXDFDataFileByName(XDInfo, name, padwith (optional))
%
% Read data from a data file indexed by the XDF file from which
% XDInfo was obtained.  
%
% Searches the files in XDFInfo.DataFiles by the Name field for a 
% match to name.  If a match is not found an error is thrown.  
%
% If the file contains rectangular data (i.e. each line has the same 
% number of entries) then data is returned as an array.  
%
% If the data is not rectangular, then a cell array of the data
% contained on each line is returned in data.Lines
%
% If padwith is specified, then short lines are padded with the
% value padwith (e.g. NaN) and the resulting array is returned as
% data.PaddedData.
%
% See also: ReadXDF, GetValueFromXDInfoByName

filename = '';

for ix = 1 : length(XDInfo.DataFiles),
    if strcmp(XDInfo.DataFiles(ix).Name, name),
        filename = XDInfo.DataFiles(ix).Path;
    end
end

if isempty(filename),
    error(['XDF ' XDInfo.XDFFile ' does not have a file named ' name '.']);
end

filename = fullfile(XDInfo.PathRoot, filename);

try
    data = load(filename, '-ascii');
    return
catch
    lines = cell(0);
    fid = fopen(filename);
    c = 1;
    longest = 1;
    tline = fgetl(fid);
    while ischar(tline),
        lines{c} = sscanf(tline, '%f')';
        tline = fgetl(fid);
        if length(lines{c}) > longest,
            longest = length(lines{c});
        end
        c = c + 1;
    end
    fclose(fid)
    data.Lines = lines;
    if nargin == 3,
        d = [];
        for ix = 1 : length(lines),
            d = [d; [lines{ix} padwith*ones(1, longest - length(lines{ix}))]];
        end
        data.PaddedData = d;
    end
    return
end