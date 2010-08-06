function MT_ServerDisconnect(sock)

% disconnect from server
fclose(sock);
delete(sock);

