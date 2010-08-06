function output = MT_ReadMessage(sock)

MT_ServerDefs

output = fread(sock, 1);
fwrite(sock, MSG_ACK);