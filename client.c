#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>  
#include <unistd.h>
#include <stdint.h>
#include <sys/socket.h> 
#include <netinet/in.h> 

#define PORT 12346
//#include "crc32.h" // Include your CRC-32 library
int crc_length = 10;
// Structure to pass data to the threads
struct ThreadData {
    char *inputString;
};
// Thread function to change lowercase 'a' to uppercase 'A'
void* modifyA(void* arg) {
    struct ThreadData *threadData = (struct ThreadData*)arg;
    char *inputString = threadData->inputString;

    for (int i = 0; i < strlen(inputString); ++i) {
        if (inputString[i] == 'a') {
            inputString[i] = 'A';
        }
    }

    pthread_exit(NULL);
}

// Thread function to change lowercase 'e' to uppercase 'E'
void* modifyE(void* arg) {
    struct ThreadData *threadData = (struct ThreadData*)arg;
    char *inputString = threadData->inputString;

    for (int i = 0; i < strlen(inputString); ++i) {
        if (inputString[i] == 'e') {
            inputString[i] = 'E';
        }
    }

    pthread_exit(NULL);
}
void* modifyI(void* arg) {
    struct ThreadData *threadData = (struct ThreadData*)arg;
    char *inputString = threadData->inputString;

    for (int i = 0; i < strlen(inputString); ++i) {
        if (inputString[i] == 'i') {
            inputString[i] = 'I';
        }
    }

    pthread_exit(NULL);
}
void* modifyO(void* arg) {
    struct ThreadData *threadData = (struct ThreadData*)arg;
    char *inputString = threadData->inputString;

    for (int i = 0; i < strlen(inputString); ++i) {
        if (inputString[i] == 'o') {
            inputString[i] = 'O';
        }
    }

    pthread_exit(NULL);
}
void* modifyU(void* arg) {
    struct ThreadData *threadData = (struct ThreadData*)arg;
    char *inputString = threadData->inputString;

    for (int i = 0; i < strlen(inputString); ++i) {
        if (inputString[i] == 'u') {
            inputString[i] = 'U';
        }
    }

    pthread_exit(NULL);
}
char* encodeHamming74(char* data) {
    // Ensure that the input data is exactly 4 bits
    if (strlen(data) != 4) {
        printf("Error: Input data must be 4 bits long for Hamming(7,4) encoding.\n");
        return NULL;
    }

    // Initialize the 7-bit encoded data
    char* encoded = (char*) malloc(7);

    // Copy the data bits into the encoded array
    encoded[2] = data[0];
    encoded[4] = data[1];
    encoded[5] = data[2];
    encoded[6] = data[3];  // Parity bit 1

    // Calculate the parity bits (P1, P2, and P4)
    
    
    encoded[0] = ((encoded[2] ^ encoded[4]) ^ encoded[6]) ;
    encoded[1] = ((encoded[4] ^ encoded[5]) ^ encoded[6]) ;
    encoded[3] = ((encoded[2] ^ encoded[5]) ^ encoded[6]) ;    
    return encoded;
}
char* char_to_binary(char c)
{
    char* binary = (char*) malloc(8);  // 8 bits + null terminator
    int count = 0;
    for (int i = 6; i >=0; i--) {
        binary[6-i] = (c & (1 << i)) ? '1' : '0';
        if(binary[6-i]=='1') count++;
    }
    binary[7] = (count%2==0)?'1':'0';
    
    return binary;
}
char* intToBinary(int n) {
    char* binary = (char*)malloc(8);
    for (int i = 0; i < 8; i++) {
        
        binary[7 - i] = (n & (1 << i)) ? '1' : '0';
    }
    return binary;
}
char* frame_characters(char* frame, char* ptr){
    int j = 0; 
    for (int i = 0; i < strlen(ptr); i++) {
        char* l = char_to_binary(ptr[i]);
       
        for (int k = 0; k < 8; k++) {
            frame[j++] = l[k];
        }
    }
    return frame;
}
// Generate the CRC-32 lookup table
static uint32_t crc32_table[256];
static void generateCRCTable() {
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc = crc >> 1;
            }
        }
        crc32_table[i] = crc;
    }
}
uint32_t calculateCRC32(const char* data, size_t length) {
    if (!data) return 0;

    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < length; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ data[i]];
    }

    return ~crc;
}
char binary_to_char_with_parity(char* binary) {
    if (!binary || strlen(binary) != 8) {
        printf("Error: Invalid binary input.\n");
        return '\0';
    }
    
    int count = 0;
    char character = 0;
    
    
    for (int i = 0; i < 7; i++) {
        if (binary[i] == '1') {
            character |= (1 << (6 - i));
            count++;
        }
    }

    
    
    char expected_parity = (count % 2 == 0) ? '1' : '0';  // Validate parity bit
    if (binary[7] != expected_parity) {
        printf("Warning: Parity bit mismatch!\n");
    }
    
    return character;
    
}
char* add_length_and_syn(char arr_frame[],int length){
    char* g = (char*) malloc(length*8+24+1);  
    char k[16] = "0001011000010110";
    char* p = (char*)malloc(8);
    p = intToBinary(length);
   
    for(int i = 0;i<16;i++){
        g[i] = k[i];
    }
    for(int i =0;i<8;i++){
        g[i+16] = p[i];
    }
    for(int i = 0;i<length*8;i++){
        g[i+24] = arr_frame[i];
    }
    g[length*8+24] = '\0';
   
    return g;

}
// Initialize the CRC-32 table
//static const int crc32_table_initialized = (generateCRCTable(), 1);

// Calculate the CRC-32 checksum for data of the given length

int client_socket;
struct sockaddr_in server_addr;
char buffer[1024];
int binary_to_int(char binary[]) {
    int number = 0;
    for (int i = 0; i < 8; i++) {
        if (binary[i] == '1') {
            number += (1 << (7 - i));
        }
    }
    return number;
}

void removeNewline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}
void* receive_message(void* client_socket) {
    while (1) {
        int read_size = recv(*(int*)client_socket, buffer, sizeof(buffer), 0);
        if (read_size <= 0) {
            printf("Connection to the server has been closed.\n");
            close(*(int*)client_socket);
            exit(1);
        }
        buffer[read_size] = '\0';
        struct ThreadData threadDataA, threadDataE,threadDataI,threadDataO,threadDataU;
        threadDataA.inputString = buffer;
        threadDataE.inputString = buffer;
        threadDataI.inputString = buffer;
        threadDataO.inputString = buffer;
        threadDataU.inputString = buffer;





        // Create threads
        pthread_t threadA, threadE,threadI,threadO,threadU;

        if (pthread_create(&threadA, NULL, modifyA, (void*)&threadDataA) != 0 ||
            pthread_create(&threadE, NULL, modifyE, (void*)&threadDataE) != 0 ||
            pthread_create(&threadI, NULL, modifyI, (void*)&threadDataI) != 0 ||
            pthread_create(&threadO, NULL, modifyO, (void*)&threadDataO) != 0 ||
            pthread_create(&threadU, NULL, modifyU, (void*)&threadDataU) != 0) {
            fprintf(stderr, "Error creating thread\n");
            exit(EXIT_FAILURE);
        }

        // Wait for the threads to finish
        pthread_join(threadA, NULL);
        pthread_join(threadE, NULL);
        pthread_join(threadI, NULL);
        pthread_join(threadO, NULL);
        pthread_join(threadU, NULL);
        //printf("received buffer: %s\n",buffer);
        //
    
        int required_size = 8 * strlen(buffer);
        char arr_frame[required_size];
        char* frame = (char*)malloc(required_size+1);
        frame[required_size] = '\0'; 
        frame = frame_characters(frame,buffer); // binary characters frame

        for(int i = 0;i<strlen(frame);i+=4){
            char temp1[5];
            strncpy(temp1,frame+i,4);
            temp1[4] = '\0';
            printf("message to binary: %s ",temp1);
            char* encoded_hamming = encodeHamming74(temp1);
            printf("enoded-hamming: %s\n",encoded_hamming);
        }
        strcpy(arr_frame,frame); 
        char* syn_frame = (char*)malloc(required_size+24);
        syn_frame = add_length_and_syn(arr_frame,strlen(buffer)); //encoded version of input data
        //printf("syn_frame: %s\n",syn_frame);
        char* crc_prod_ptr = (char*)malloc(required_size+24+2+50);
        generateCRCTable();
        uint32_t crc_prod;
        crc_prod = calculateCRC32(syn_frame, strlen(syn_frame));
        printf("crc-prod: %u\n",crc_prod);
        //
        //generateCRCTable();
        //uint32_t crc_calculated;
        //crc_calculated = calculateCRC32(encoded_data_received,strlen(encoded_data_received));
        //char* crc_calculated_str = (char*)malloc(1000);
        //sprintf(crc_calculated_str,"%u",crc_calculated);
        //printf("crc-calculated: %u\ncrc-received: %s\n",crc_calculated,crc_received);
        //int new_length = strlen(encoded_data_received)-8;
        //for(int i = 24;i<new_length;i+=4){
        //    char temp1[5];
        //    strncpy(temp1,encoded_data_received+i,4);
        //    temp1[4] = '\0';
        //    printf("temp1: %s ",temp1);
        //    char* encoded_hamming = encodeHamming74(temp1);
        //    printf("enoded-hamming: %s\n",encoded_hamming);
        //}
        ////printf("num-bits: %d",num_bits);
        //char decoded_str[num_bits+1]; 
        //int decode_idx = 0;
        //if(strcmp(crc_calculated_str,crc_received)==0){
        //    //printf("Entered condition..\n");
        //    
//
//
        //    for (int i = 24; i < 24+num_bits; i += 8) {
        //        char temp[9];
        //        strncpy(temp, encoded_data_received+i, 8);
        //        temp[8] = '\0';
        //        //printf("temp: %s\n",temp);
        //        char decoded_char = binary_to_char_with_parity(temp); // converting binary with parity to character
        //        //printf("decoded-char: %c\n",decoded_char);
        //        decoded_str[decode_idx++] = decoded_char;
        //    }       
        //    decoded_str[decode_idx] = '\0';
            //printf("decoded-str: %s\n", decoded_str);
        printf("Other User: %s\n", buffer);
        }
        
    }


int main() {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Error in socket");
        exit(1);
    }
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection error.....");
        exit(1);
    }

    pthread_t receive_thread;
    if (pthread_create(&receive_thread, NULL, receive_message, &client_socket) < 0) {
        perror("Error creating receive thread....");
        exit(1);
    }

    while (1) {
        char c = '\0';
        printf("%c",c);
        fgets(buffer, sizeof(buffer), stdin);
        removeNewline(buffer);
        char* ptr = buffer;
        //printf("Sent buffer: %s   %ld\n",ptr,strlen(ptr));
        int required_size = 8 * (strlen(ptr)-1);
        char arr_frame[required_size];
        char* frame = (char*)malloc(required_size+1);
        frame[required_size] = '\0'; 
        frame = frame_characters(frame,ptr); // binary characters frame
        //printf("frame-characters: %s\n",frame);
        //printf("frame: %s\n",frame);
        strcpy(arr_frame,frame); 
        //printf("array-frame: %s\n",arr_frame);    
        char* syn_frame = (char*)malloc(required_size+24);
        syn_frame = add_length_and_syn(arr_frame,strlen(ptr)); //encoded version of input data
        //printf("syn_frame: %s\n",syn_frame);
        char* crc_prod_ptr = (char*)malloc(required_size+24+2+50);
        generateCRCTable();
        uint32_t crc_prod;
        crc_prod = calculateCRC32(syn_frame, strlen(syn_frame));
        //printf("crc-prod: %u\n",crc_prod);
        sprintf(crc_prod_ptr,"%u", crc_prod);
        crc_length = strlen(crc_prod_ptr);
        strcat(crc_prod_ptr,syn_frame);
        //printf("crc-prod-ptr: %s\n",crc_prod_ptr);
        //printf("syn-frame+crc-prod: %s\ncrc-length: %d\n",crc_prod_ptr,crc_length);
        //send(client_socket, crc_prod_ptr, strlen(crc_prod_ptr), 0);
        send(client_socket, buffer, strlen(buffer), 0);
    }

    close(client_socket);

    return 0;

}

