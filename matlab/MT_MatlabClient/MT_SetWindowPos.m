function MT_SetWindowPos(sock, x, y, s, c, m)

MT_ServerDefs

msg_table = zeros(8,1);

win_width = 1;
win_height = 2;
win_cwidth = 3;
win_cheight = 4;
win_curr_x = 5;
win_curr_y = 6;
win_set_x = 7;
win_set_y = 8;

for ix = 1 : length(s),
    if(strcmp(m{ix}, 'WindowProps')),
        msg_table(c(ix)+1) = s(ix);
    end
end

MT_InitServerExchange(sock);

MT_SendMappedMessage(sock, win_set_x, msg_table);

MT_SendInt(sock, x);

MT_SendMappedMessage(sock, win_set_y, msg_table);

MT_SendInt(sock, y);

MT_EndServerExchange(sock);