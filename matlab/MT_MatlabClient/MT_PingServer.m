function status = MT_PingServer(sock, verbose)

MT_ServerDefs

if nargin < 2,
    verbose = 0;
end 

if verbose,
    disp(sprintf('Attempting to ping MT_Server at %s:%d.',...
                 sock.RemoteHost, sock.RemotePort));
end             

t = MT_SendMessage(sock, CMD_PING);
fwrite(sock, MSG_ACK);

if t == MSG_OK
    status = 0;
    if verbose,
        disp('Success.');
    end
else
    status = -1;
    if verbose,
        disp('Error.');
    end
end