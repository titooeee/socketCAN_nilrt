#ifndef SOCKETCAN_H
#define SOCKETCAN_H

int init_pcan(void);
int send_pcan(int sock, int can_id, unsigned char *data, int len);
void close_pcan(int sock);

#endif
