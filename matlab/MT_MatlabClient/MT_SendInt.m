function MT_SendInt(sock, val)

fwrite(sock, swapbytes(int32(val)), 'int32');
fread(sock, 1);