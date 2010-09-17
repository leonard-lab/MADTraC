function value = GetValueFromXDInfoByName(XDInfo, name)
%
% value = GetValueFromXDInfoByName(XDInfo, name)
%
% Searches the XDInfo structure's Parameters and DataGroups.Values
% fields for a Name matching name and returns the corresponding
% Value.
%
% If no match is found, 'Error' is returned.
%
% See also: ReadXDF, LoadXDFDataFileByName

for ix = 1 : length(XDInfo.Parameters),
    if strcmp(XDInfo.Parameters(ix).Name, name),
        value = XDInfo.Parameters(ix).Value;
        return
    end
end

for ix = 1 : length(XDInfo.DataGroups),
    for jx = 1 : length(XDInfo.DataGroups(ix).Values),
        if strcmp(XDInfo.DataGroups(ix).Values(jx).Name, name),
            value = XDInfo.DataGroups(ix).Values(jx).Value;
            return
        end
    end
end

value = 'Error';
return;