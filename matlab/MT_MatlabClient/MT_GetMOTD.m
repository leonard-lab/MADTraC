function motd = MT_GetMOTD(sock, verbose)

MT_ServerDefs

if nargin < 2,
    verbose = 0;
end 

if verbose,
    disp(sprintf('Attempting to get MOTD from MT_Server at %s:%d.',...
                 sock.RemoteHost, sock.RemotePort));
end             

MT_InitServerExchange(sock);

MT_SendMessage(sock, MSG_MOTD);

motd = MT_ReadString(sock);

MT_EndServerExchange(sock);

if verbose,
    disp(['MOTD:  ' motd]);
end
