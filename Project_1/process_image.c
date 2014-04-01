#include "tcp.h"

void send_image(int socket, char* fs)
{
    FILE *picture;
    int size, read_size, stat, packet_index;
    char send_buffer[BUF_SIZE], read_buffer[256];
    packet_index = 1;

    picture = fopen(fs, "r");
    if(picture == NULL) {
        printf("Error Opening Image File"); } 

    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);

    //Send Picture Size
    write(socket, (void *)&size, sizeof(int));

    //Read while we get errors that are due to signals.
    do { 
      	stat = read(socket, &read_buffer, 255);
    } while (stat < 0);

    while(!feof(picture)) {
        //Read from the file into our send buffer
        read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);

        //Send data through our socket 
        do {
            stat = write(socket, send_buffer, read_size);  
        } while (stat < 0);

        packet_index++;  

        //Zero out our send buffer
        bzero(send_buffer, sizeof(send_buffer));
    }
}


