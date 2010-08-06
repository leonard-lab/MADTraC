function sock = MT_ServerConnect(address, port)

if nargin < 1,
    address = 'localhost';
end

if nargin < 2,
    port = 3000;
end

try
    % connect to server
    sock = tcpip('localhost', port);
    sock.InputBufferSize = 4096;
    fopen(sock);
    
    % make sure we connected
    if(~strcmp(sock.status, 'open'))
        error(['Could not connect to server ' address ':' int2str(port)]);
    end
catch err
    error(['Could not connect to server ' address ':' int2str(port)]);
end