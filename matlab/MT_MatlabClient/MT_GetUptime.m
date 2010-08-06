function uptime = MT_GetUptime(sock, verbose)

MT_ServerDefs

if nargin < 2,
    verbose = 0;
end 

if verbose,
    disp(sprintf('Attempting to get uptime from MT_Server at %s:%d.',...
                 sock.RemoteHost, sock.RemotePort));
end             

MT_InitServerExchange(sock);

MT_SendMessage(sock, MSG_UPTIME); % OK

uptime = MT_ReadDouble(sock);

MT_EndServerExchange(sock);

if verbose,
    disp(sprintf('Uptime was %f', uptime));
end
