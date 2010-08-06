function status = MT_SendMessage(sock, message)

fwrite(sock, message);
status = fread(sock, 1);