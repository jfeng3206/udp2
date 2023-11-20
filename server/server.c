//
//  main.c
//  server
//
//  Created by mac on 2023/11/13.
//

#ifndef server_h
#define server_h

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#endif /* server_h */

//primitives
#define PORT 6666
#define STARTPACKETID 0XFFFF
#define ENDPACKETID 0XFFFF
#define CLIENTID 0XFF
#define ACC_PER 0XFFF8
#define TIMEOUT 5
#define SUB_LIST_LENGTH 10

//acc types
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

//void print_req(struct req_packet req){
//    printf("START PACKET ID: %x\n", req.start_id);
//    printf("CLIENT ID: %x\n", req.cli_id);
//    printf("ACCESS PERMISSION: %x\n", req.acc_per);
//    printf("SEGMENT NUMBER: %d\n", req.seg_no);
//    printf("LENGTH: %d\n", req.len);
//    printf("TECHNOLOGY: %x\n", req.technology);
//    printf("SUBSCRIBER NUMBER: %lu\n", req.subscriber_no);
//    printf("END PACKET ID: %x\n", req.end_id);
//}

//void print_subscriber(struct subscriber sub){
//    printf("SUBSCRIBER NUMBER: %lu\n", sub.subscriber_no);
//    printf("TECHNOLOGY: %d\n", sub.technology);
//    printf("PAID: %X\n",sub.paid);
//};

// Create Response
struct res_packet create_response(struct req_packet req) {
    struct res_packet res;
    res.start_id = req.start_id;
    res.cli_id = req.cli_id;
    res.seg_no = req.seg_no;
    res.subscriber_no = req.subscriber_no;
    res.end_id = req.end_id;
    return res;
};

int check_subscriber (struct subscriber subscriber_list[], unsigned int subscriber_no,uint8_t technology){
    for(int i =0;i<SUB_LIST_LENGTH;i++){
        if (subscriber_list[i].subscriber_no==subscriber_no && subscriber_list[i].technology==technology){
            return subscriber_list[i].paid;
        } else if (subscriber_list[i].subscriber_no==subscriber_no){
            return 2;
        }
    }
    return 3;
}


void error(const char *msg){
    perror(msg);
    exit(1);
}

int main(int argc, const char * argv[]) {
    int sock, n;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    FILE *file;
    struct req_packet req;
    struct res_packet res;
    
    //Create a socket
    printf("******Create the socket*****\n");
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    char line[255];
    int seg_counter = 1;//count number of requests received
    int i = 0; //index tracker for loading subscriber data
    if(sock<0)
        error("ERROR: opening socket");
    
    //Bind the socket
    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);
    if( bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
        error("ERROR: binding socket");
    }
    printf("******SERVER INITIALIZED SUCCESSFULLY*****\n");
    
    // Load Validation Database
    struct subscriber subscriber_list[SUB_LIST_LENGTH];
    file = fopen("Verification_Database.txt", "rt");
    if (file == NULL) {
        error("ERROR: input file");
    }
    printf("******LOADING VERIFIED SUBSCRIBER DATA*****\n");
    while (fgets(line, sizeof(line), file) != NULL){
        subscriber_list[i].subscriber_no  = atol(strtok(line, " "));
        subscriber_list[i].technology =atoi(strtok(NULL, " "));
        subscriber_list[i].paid =atoi(strtok(NULL, " "));
//        sscanf(line, "%lu %hhu %hhu",  subscriber_list[i].subscriber_no, subscriber_list[i].technology,  subscriber_list[i].paid);
//        print_subscriber(subscriber_list[i]);
        i++;
    }
    
     
    //Start receiving and responding...
    while(1){
        cli_len = sizeof(cli_addr);
        n = recvfrom(sock, &req, sizeof(struct req_packet), 0, (struct sockaddr *)&cli_addr, &cli_len);
        if(n<0) error("ERROR: receive from client");
        seg_counter++;
        //print_req(req);
        
        //check request
        int check;
        //res = create_response(req);
        check = check_subscriber(subscriber_list, req.subscriber_no, req.technology);
        if(req.seg_no ==11){
            sleep(10);//case for simulating ack time out
        } else{
            if(check == 3){
                res.acc_type = NOTEXIST;
                printf("******SUBSCRIBER NOT EXIST******\n");
            } else if (check == 2){
                res.acc_type = NOTEXIST;
                printf("******SUBSCRIBER'S TECHNOLOGY NOT EXIST******\n");
            } else if(check==0){
                res.acc_type = NOTPAID;
                printf("******SUBSCRIBER DOES NOT PAY******\n");
            } else {
                res.acc_type = OK;
                printf("Subscriber ok to access\n");
            }
        }
        n =sendto(sock,&res,sizeof(struct res_packet),0,(struct sockaddr *)&cli_addr,cli_len);
        if (n < 0) error("Sendto");
        if(req.seg_no==11){
                    printf("-------FINISHED RECEIVING ALL PACKETS. SERVER CLOSING-------\n");
                    exit(1);
        }
        n=0;
        printf("------New packet below------\n");
        
    }
}
