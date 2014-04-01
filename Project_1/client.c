#include "tcp.h"

void send_image(int socket, char* fs);

int main(int argc , char *argv[])
{
    int socket_desc, i;
    struct sockaddr_in server;

    static struct timeval tm1, tm2;
    float t;

    // Get start time
    gettimeofday(&tm1, NULL);
		
    for(i = 1; i < argc; i++) {
	//Create socket
    	socket_desc = socket(AF_INET , SOCK_STREAM , 0);

    	if (socket_desc == -1) {
            printf("Could not create socket");
    	}

    	memset(&server, 0, sizeof(server));
    	server.sin_addr.s_addr = inet_addr("127.0.0.1");
    	server.sin_family = AF_INET;
    	server.sin_port = htons( 8081 );

    	//Connect to remote server
    	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0) {
            close(socket_desc);
  	    puts("Connect Error");
  	    return 1;
     	}

	printf("Send Image - %i\n", i);
    	send_image(socket_desc, argv[i]);

    	close(socket_desc);
    }

    // Get end time
    gettimeofday(&tm2, NULL);

    // Print process time
    t = 1000000 * (tm2.tv_sec - tm1.tv_sec) + (tm2.tv_usec - tm1.tv_usec);
    printf("%s %0.3f ms\n", "total processing time: ",  t / 1000); 

    return 0;
}
