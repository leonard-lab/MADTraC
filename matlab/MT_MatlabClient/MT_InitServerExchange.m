function MT_InitServerExchange(sock, verbose)

if nargin < 2,
    verbose = 0;
end

MT_ServerDefs

fwrite(sock, CMD_COMMSTART);
fread(sock, 1); % OK
fwrite(sock, MSG_ACK);
