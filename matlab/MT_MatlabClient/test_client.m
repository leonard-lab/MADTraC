clear all;  close all;

%% MAIN CLIENT PROGRAM

sock = MT_ServerConnect();

status = MT_PingServer(sock, 1);

t = MT_GetUptime(sock, 1);

motd = MT_GetMOTD(sock, 1);

[s, c, d, m] = MT_GetMessageTable(sock);

[w, h, cw, ch] = MT_GetWindowSize(sock, s, c, m);

MT_SetWindowPos(sock, 100, 100, s, c, m);

MT_ServerDisconnect(sock);
clear sock