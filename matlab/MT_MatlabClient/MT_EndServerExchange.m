function MT_EndServerExchange(sock, verbose)

if nargin < 2,
    verbose = 0;
end

MT_ServerDefs

MT_SendMessage(sock, MSG_END);
fread(sock, 1); % END
fwrite(sock, MSG_ACK);