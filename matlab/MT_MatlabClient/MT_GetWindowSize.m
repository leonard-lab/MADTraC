function [w, h, cw, ch] = MT_GetWindowSize(sock, s, c, m)

MT_ServerDefs

msg_table = zeros(4,1);

win_width = 1;
win_height = 2;
win_cwidth = 3;
win_cheight = 4;

for ix = 1 : length(s),
    if(strcmp(m{ix}, 'WindowProps')),
        msg_table(c(ix)+1) = s(ix);
    end
end

MT_InitServerExchange(sock);

MT_SendMappedMessage(sock, win_width, msg_table);

w = MT_ReadInt(sock);

MT_SendMappedMessage(sock, win_height, msg_table);

h = MT_ReadInt(sock);

MT_SendMappedMessage(sock, win_cwidth, msg_table);

cw = MT_ReadInt(sock);

MT_SendMappedMessage(sock, win_cheight, msg_table);

ch = MT_ReadInt(sock);

MT_EndServerExchange(sock);