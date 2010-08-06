function output = MT_ReadDouble(sock, num)

MT_ServerDefs

if nargin < 2,
    num = 1;
end

output = swapbytes(fread(sock, num, 'double'));
fwrite(sock, MSG_ACK);