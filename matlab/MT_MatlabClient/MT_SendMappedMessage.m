function MT_SendMappedMessage(sock, msg, map)

fwrite(sock, uint8(map(msg)));
fread(sock, 1); % OK