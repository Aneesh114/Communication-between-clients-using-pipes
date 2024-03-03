#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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



char* decodeHamming74(char* data) {
    // Ensure that the input data is exactly 7 bits
    //printf("Strlen %ld\n data: %s\n",strlen(data),data);
    if (strlen(data) != 7) {
        printf("Error: Input data must be 7 bits long for Hamming(7,4) decoding.\n");
        return NULL;
    }

    // Initialize the 4-bit decoded data
    char* decoded = (char*) malloc(5);
    
    
    printf("data: %s\n",data);
    char s1 = (data[0] ^ data[4] ^ data[5] ^ data[6]);
    char s2 = (data[1] ^ data[2] ^ data[5] ^ data[6]);
    char s3 = (data[3] ^ data[2] ^ data[4] ^ data[6]);
    printf("s1,s2,s3: %d,%d,%d\n",s1,s2,s3);
    // Check for and correct single-bit errors
    
    //int errorPos = s1 + 2 * s2 + 4 * s3;
    int errorPos;
    
    if (errorPos != 0) {
        printf("Detected an error at position %d. Correcting...\n", errorPos);

        // Correct the bit at the error position
        if (data[7 - errorPos] == '0') {
            data[7 - errorPos] = '1';
        } else {
            data[7 - errorPos] = '0';
        }
    }

    // Extract the 4 data bits
    decoded[0] = data[2];
    decoded[1] = data[4];
    decoded[2] = data[5];
    decoded[3] = data[6];
    decoded[4] = '\0'; // Null-terminate the decoded data

    return decoded;
}

int main() {
    char input[] = "1010";
    char* encoded = encodeHamming74(input);
    //char p[3];
    char* p = (char*)malloc(4);
    p[0] = encoded[0];
    p[1] = encoded[1];
    p[2] = encoded[3];
    p[3] = '\0';

    if (encoded != NULL) {
        printf("Original Data: %s\n", input);
        printf("Encoded Data: %s\n", encoded);
        printf("parity bits: %s\n",p);

        // Introduce errors into the encoded data
        // For example, let's flip the first bit (introducing an error)
        if (input[3] == '0') {
            input[3] = '1';
        } else {
            input[3] = '0';
        }

        printf("Data with Errors: %s\n", input);

        char* encoded_error = encodeHamming74(input);
        printf("encoded-error: %s\n",encoded_error);
        //char p_new[3];
        char* p_new = (char*)malloc(4);
        p_new[0] = encoded_error[0];
        p_new[1] = encoded_error[1];
        p_new[2] = encoded_error[3];
        p_new[3] = '\0';
        printf("p-new: %s\n",p_new);
        //if ((strcmp(p[0],p_new[0])!=0) && (strcmp(p[3],p_new[3])!=0)) {
        //    printf("Error in bit 1\n");
        //} else if((strcmp(p[1],p_new[1]!=0)) && (strcmp(p[0],p_new[0])!=0)){
        //    printf("Error in bit 2\n");
        //} else if((strcmp(p[1],p_new[1]!=0)) && (strcmp(p[3],p_new[3])!=0)){
        //    printf("Error in bit 3\n");
        //} else if((strcmp(p[1],p_new[1]!=0)) && (strcmp(p[0],p_new[0])!=0) && (strcmp(p[3],p_new[3])!=0)){
        //    printf("Error in bit 4\n");
        //}
        printf("before check\n");
        if ((p[0]!=p_new[0]) && (p[2]!=p_new[2]) && (p[1]==p_new[1])) {
            printf("Error in bit 1\n");
        } else if((p[1]!=p_new[1]) && (p[0]!=p_new[0]) && (p[2]==p_new[2])){
            printf("Error in bit 2\n");
        } else if((p[1]!=p_new[1]) && (p[2]!=p_new[2]) && (p[0]==p_new[0])){
            printf("Error in bit 3\n");
        } else if((p[1]!=p_new[1]) && (p[0]!=p_new[0]) && (p[2]!=p_new[2])){
            printf("Error in bit 4\n");
        }
    }

    return 0;
}
