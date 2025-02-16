#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#define CAN_INTERFACE "can0"  // Directly use the interface name

// Function to initialize PCAN interface
int init_pcan() {
    int sock;
    
    // Create socket
    sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        perror("Error creating socket");
        return -1;
    }

    // Get interface index
    int ifindex = if_nametoindex(CAN_INTERFACE);
    if (ifindex == 0) {
        perror("Error getting interface index");
        close(sock);
        return -1;
    }

    struct sockaddr_can addr = {0};
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifindex;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Error binding socket");
        close(sock);
        return -11;
    }

    printf("Successfully initialized %s at 500000 bit/s\n", CAN_INTERFACE);
    return sock;
}

// Function to send a CAN message
int send_pcan(int sock, int can_id, unsigned char *data, int len) {
    struct can_frame frame = {0};
    frame.can_id = can_id;
    frame.can_dlc = len;
    memcpy(frame.data, data, len);

    if (write(sock, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Error sending CAN message");
        return -1;
    }
    return 0;
}

// Function to close the CAN socket
void close_pcan(int sock) {
    if (sock >= 0) {
        close(sock);
    }
}
