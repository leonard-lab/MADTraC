function [server_codes, client_codes, descriptions, modules] = ...
    MT_GetMessageTable(sock)

MT_ServerDefs

MT_SendMessage(sock, CMD_LIST);
fwrite(sock, MSG_ACK);

n_msg = MT_ReadInt(sock);

server_codes = zeros(n_msg, 1);
client_codes = zeros(n_msg, 1);
descriptions = cell(n_msg, 1);
modules = cell(n_msg, 1);

for ix = 1 : n_msg,
    s = MT_ReadMessage(sock);
    c = MT_ReadMessage(sock);
    d = MT_ReadString(sock);
    m = MT_ReadString(sock);
    server_codes(ix) = s;
    client_codes(ix) = c;
    descriptions{ix} = d;
    modules{ix} = m;
end