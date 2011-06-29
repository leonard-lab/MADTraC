function XDInfo = ReadXDF(filename)
%
% XDInfo = ReadXDF(filename)
%
% Reads the XML-formatted Experiment Data File (XDF) file and returns
% a structure containing the information in the file.
%
% If the filename does not end in '.xdf', it is added automatically.
%
% If the file does not exist or cannot be read by the XML parser, then
% an error is thrown.
%
% The exact contents of XDInfo will depend upon the contents of the file.
% The only two guaranteed fields are XDFFile (the path of the file) and
% XMLStruct (the struct returned by the XML parser as an intermediate in
% parsing the experiment data information).
%
% Currently the following additional fields may be populated:
%
% - DataFiles - Struct array of the data files descriptions and paths.
% - Parameters - Parameters added to the file by addParameter.
% - DataGroups - Array of parameter groups.
% - VideoSource - Path of the video file or other description.
% - Date - mm/dd/yy Date XDF file was created in program.
% - Time - hh:mm:ss (24-hour) time XDF file was created in program.
% - UserName - Note recorded by user.
%
% See also: GetValueFromXDInfoByName, LoadXDFDataFileByName

if (length(filename) < 4) || (~strcmp(filename(end-3 : end), '.xdf')),
    filename = [filename '.xdf'];
end

XDInfo.XDFFile = filename;
if(filename(1) == '/' || strcmp(filename([2 3]), ':\')),
    % absolute path
    p = fileparts(filename);
    XDInfo.PathRoot = p;
else
    % relative path
    p = fileparts(filename);
    the_cwd = pwd;
    cd(p);
    XDInfo.PathRoot = pwd;
    cd(the_cwd);
end

try
    XMLStruct = parseXML(filename);
catch
    error(['Could not read XDF file ' filename '.']);
end

XDInfo.XMLStruct = XMLStruct;

DataGroups = [];

for ix = 1 : length(XMLStruct.Children)
    if(isjunknode(XMLStruct.Children(ix)))
        continue;
    end
    
    if strcmp(XMLStruct.Children(ix).Name, 'DataFiles'),
        DataFiles = [];
        filecount = 1;
        DataNode = XMLStruct.Children(ix);
        for jx = 1 : length(DataNode.Children),
            if ~isjunknode(DataNode.Children(jx)),
                df.Name = strtrim(DataNode.Children(jx).Name);
                df.Path = strtrim(DataNode.Children(jx).Children(1).Data);
                DataFiles = [DataFiles df];
                filecount = filecount + 1;
            end
        end
        XDInfo.DataFiles = DataFiles;
    end
    
    if strcmp(XMLStruct.Children(ix).Name, 'Parameters'),
        Parameters = [];
        ParamsNode = XMLStruct.Children(ix);
        for jx = 1 : length(ParamsNode.Children),
            if ~isjunknode(ParamsNode.Children(jx)),
                pn.Name = strtrim(ParamsNode.Children(jx).Name);
                if ~isempty(ParamsNode.Children(jx).Children) > 0
                    pn.Value = strtrim(ParamsNode.Children(jx).Children(1).Data);
                else
                    pn.Value = '';
                end
                if strcmp(pn.Name, 'Video_Source'),
                    XDInfo.VideoSource = pn.Value;
                end
                if strcmp(pn.Name, 'Date'),
                    XDInfo.Date = pn.Value;
                end
                if strcmp(pn.Name, 'Time'),
                    XDInfo.Time = pn.Value;
                end
                if strcmp(pn.Name, 'User_Note'),
                    XDInfo.UserNote = pn.Value;
                end
                Parameters = [Parameters pn];
            end
        end
        XDInfo.Parameters = Parameters;
    end
    
    if strncmp(XMLStruct.Children(ix).Name, 'Data_Group_', 11),
        Values = [];
        DataGroup.Name = XMLStruct.Children(ix).Name([12 : end]);
        DGNode = XMLStruct.Children(ix);
        for jx = 1 : length(DGNode.Children),
            if ~isjunknode(DGNode.Children(jx)),
                v.Name = strtrim(DGNode.Children(jx).Name);
                v.Value = strtrim(DGNode.Children(jx).Children(1).Data);
                Values = [Values v];
            end
        end
        DataGroup.Values = Values;
        DataGroups = [DataGroups DataGroup];
    end
    
    if strncmp(XMLStruct.Children(ix).Name, 'Reports', 7),
        Reports = [];
        ReportsNode = XMLStruct.Children(ix);
        for jx = 1 : length(ReportsNode.Children)
            if ~isjunknode(ReportsNode.Children(jx))
                rn.Name = strtrim(ReportsNode.Children(jx).Name);
                rn.Value = strtrim(ReportsNode.Children(jx).Children(1).Data);
                Reports = [Reports rn];
            end
        end
        XDInfo.Reports = Reports;
    end
        
end

XDInfo.DataGroups = DataGroups;

return;

function result = isjunknode(node)

if(~isfield(node, 'Name') || ~isfield(node, 'Data')),
    result = 1;
    return;
end

if(strcmp(node.Name, '#text') && isjunkstring(node.Data))
    result = 1;
    return;
end

result = 0;
return

function result = isjunkstring(input)

result = 0;
for ix = 1 : length(input),
    if (input(ix) == ' ') || (input(ix) == 10),
        result = 1;
        return;
    end
end

function theStruct = parseXML(filename)
% PARSEXML Convert XML file to a MATLAB structure.
%  From the MATLAB documentation for xmlread
try
   tree = xmlread(filename);
catch
   error('Failed to read XML file %s.',filename);
end

% Recurse over child nodes. This could run into problems 
% with very deeply nested trees.
try
   theStruct = parseChildNodes(tree);
catch
   error('Unable to parse XML file %s.',filename);
end


% ----- Subfunction PARSECHILDNODES -----
function children = parseChildNodes(theNode)
% Recurse over node children.
children = [];
if theNode.hasChildNodes
   childNodes = theNode.getChildNodes;
   numChildNodes = childNodes.getLength;
   allocCell = cell(1, numChildNodes);

   children = struct(             ...
      'Name', allocCell, 'Attributes', allocCell,    ...
      'Data', allocCell, 'Children', allocCell);

    for count = 1:numChildNodes
        theChild = childNodes.item(count-1);
        children(count) = makeStructFromNode(theChild);
    end
end

% ----- Subfunction MAKESTRUCTFROMNODE -----
function nodeStruct = makeStructFromNode(theNode)
% Create structure of node info.

nodeStruct = struct(                        ...
   'Name', char(theNode.getNodeName),       ...
   'Attributes', parseAttributes(theNode),  ...
   'Data', '',                              ...
   'Children', parseChildNodes(theNode));

% if any(strcmp(methods(theNode), 'getData'))
%    nodeStruct.Data = char(theNode.getData); 
% else
%    nodeStruct.Data = '';
% end

try
    nodeStruct.Data = char(theNode.getData);
catch
    nodeStruct.Data = '';
end

% ----- Subfunction PARSEATTRIBUTES -----
function attributes = parseAttributes(theNode)
% Create attributes structure.

attributes = [];
if theNode.hasAttributes
   theAttributes = theNode.getAttributes;
   numAttributes = theAttributes.getLength;
   allocCell = cell(1, numAttributes);
   attributes = struct('Name', allocCell, 'Value', ...
                       allocCell);

   for count = 1:numAttributes
      attrib = theAttributes.item(count-1);
      attributes(count).Name = char(attrib.getName);
      attributes(count).Value = char(attrib.getValue);
   end
end