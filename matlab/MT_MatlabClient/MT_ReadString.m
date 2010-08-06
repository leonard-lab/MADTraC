function output = MT_ReadString(sock)

MT_ServerDefs

len = MT_ReadInt(sock);

output = fread(sock, len);
fwrite(sock, MSG_ACK);

% matlab strings don't need a null terminator
if(output(end) == 0),
    output = output(1 : end-1);
end

output = char(output');
