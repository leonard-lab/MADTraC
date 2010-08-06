function output = MT_ReadInt(sock, send_ack)

if nargin < 2
    send_ack = 1;
end

MT_ServerDefs

output = fread(sock, 1, 'int');
output = double(swapbytes(cast(output, 'int32')));
if(send_ack)
    fwrite(sock, MSG_ACK);
end