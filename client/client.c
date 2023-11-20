//
//  main.c
//  client
//
//  Created by mac on 2023/11/13.
//

#include <stdio.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
//primitives
#define PORT 6666
#define STARTPACKETID 0XFFFF
#define ENDPACKETID 0XFFFF
#define CLIENTID 0XFF
#define TIMEOUT 3
#define NOTPAID 0XFFF9
#define NOTEXIST 0XFFFA
#define OK 0XFFFB
//define data packet
struct req_packet{
    uint16_t start_id;
    uint8_t cli_id;
    uint16_t acc_per;
    uint8_t seg_no;
    uint8_t len;
    uint8_t technology;
    unsigned long subscriber_no;
    uint16_t end_id;
};

//define ack packet format
struct res_packet{
    uint16_t start_id;
    uint8_t cli_id;
    uint16_t acc_type;
    uint8_t seg_no;
    uint8_t len;
    uint8_t technology;
    unsigned long subscriber_no;
    uint16_t end_id;
};
struct subscriber{
    unsigned long subscriber_no;
    uint8_t technology;
    uint8_t paid;
};
void error(const char *msg){
    perror(msg);
    exit(1);
}
//void print_req(struct req_packet req){
//    printf("START PACKET ID: %x\n", req.start_id);
//    printf("CLIENT ID: %x\n", req.cli_id);
//    printf("ACCESS PERMISSION: %x\n", req.acc_per);
//    printf("SEGMENT NUMBER: %d\n", req.seg_no);
//    printf("LENGTH: %d\n", req.len);
//    printf("TECHNOLOGY: %x\n", req.technology);
//    printf("SUBSCRIBER NUMBER: %lu\n", req.subscriber_no);
//    printf("END PACKET ID: %x\n", req.end_id);
//};
struct req_packet create_req(void) {
    struct req_packet req;
    req.start_id = STARTPACKETID;
    req.cli_id = CLIENTID;
    req.end_id = ENDPACKETID;
    return req;
}

int main(int argc, const char * argv[]) {
    int sock, len;
    socklen_t addr_len;
    struct sockaddr_in serv_addr, from;
    struct req_packet req;
    struct res_packet res;
    FILE *file;
    char line[255];
    int n = 0;
    int retry = 0;
    int seg_counter = 1;
    //Create a socket
    printf("******Create a socket*****\n");
    sock = socket(AF_INET, SOCK_DGRAM, 0);;
    if(sock<0)
        error("ERROR: opening socket");
    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);
    
    //Read datapacket from datapacket.txt
    file = fopen("datapacket.txt", "rt");
    if (file == NULL) {
        error("ERROR: input file");
    }
    req = create_req();
    while (fgets(line, sizeof(line), file) != NULL){
        n=0;
        retry = 0;
        req.seg_no=seg_counter;
        req.len = strlen(line);
        req.subscriber_no  = atol(strtok(line, " "));
        req.technology =atoi(strtok(NULL, " "));
        seg_counter++;
        //print_req(req);
        
        while(n<=0 &&  retry <3){
            //send data packet to server
            addr_len=sizeof(struct sockaddr_in);
            n=sendto(sock,&req,sizeof(struct req_packet),0,(struct sockaddr *)&serv_addr,addr_len);
            if (n < 0) error("Sendto");
            
            //set timer
            struct timeval tv;
            tv.tv_sec = TIMEOUT;
            tv.tv_usec = 0;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));
            
            //recieving response
            n = recvfrom(sock,&res,sizeof(struct res_packet),0,(struct sockaddr *)&from, &addr_len);
            
            if (n <= 0 ) {
                printf("******Failed to receive a response. Retrying..*****\n");
                retry ++;
            }else {
                if (res.acc_type== NOTPAID) {
                    printf("******SUBSCRIBER NOT PAID******\n");
                } else  if (res.acc_type== NOTEXIST) {
                    printf("******SUBSCRIBER NOT EXIST******\n");
                } else if (res.acc_type== OK) {
                    printf("Ok to access the network\n");
                }
            }
        }
        if (retry >=3){
            printf("******SERVER NOT RESPONDING*****\n");
            exit(0);
        }
        if(seg_counter<=10){
            printf("------New packet below------\n");
        }
    }
    printf("------All Packets Sent------\n");
    exit(0);
}
