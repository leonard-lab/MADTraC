%% DEFINITIONS

CMD_COMMSTART = uint8(hex2dec('fe'));
CMD_PING      = uint8(hex2dec('fd'));
CMD_LIST      = uint8(hex2dec('fc'));

MSG_END     = uint8(1);
MSG_ACK     = uint8(2);
MSG_OK      = uint8(3);
MSG_ERR     = uint8(4);
MSG_UNKNOWN = uint8(5);
MSG_UPTIME  = uint8(6);
MSG_MOTD    = uint8(7);