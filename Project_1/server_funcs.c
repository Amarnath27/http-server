#include "tcp.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFERSIZE	4096
#define LINELEN		128
#define BYTES 		1024

extern int	errno;
char *ROOT;

/* HTTP response and header for a successful request.  */
static char* ok_response =
  "HTTP/1.1 200 OK\n"
  "Content-type: text/html\n"
  "\n";

/* HTTP response, header, and body indicating that the we didn't
   understand the request.  */
static char* bad_request_response = 
  "HTTP/1.0 400 Bad Request\n"
  "Content-type: text/html\n"
  "\n"
  "<html>\n"
  " <body>\n"
  "  <h1>Bad Request</h1>\n"
  "  <p>This server did not understand your request.</p>\n"
  " </body>\n"
  "</html>\n";

/* HTTP response, header, and body template indicating that the
   requested document was not found.  */
static char* not_found_response_template = 
  "HTTP/1.0 404 Not Found\n"
  "Content-type: text/html\n"
  "\n"
  "<html>\n"
  " <body>\n"
  "  <h1>Not Found</h1>\n"
  "  <p>The requested URL %s was not found on this server.</p>\n"
  " </body>\n"
  "</html>\n";

/* HTTP response, header, and body template indicating that the
   method was not understood.  */
static char* bad_method_response_template = 
  "HTTP/1.0 501 Method Not Implemented\n"
  "Content-type: text/html\n"
  "\n"
  "<html>\n"
  " <body>\n"
  "  <h1>Method Not Implemented</h1>\n"
  "  <p>The method %s is not implemented by this server.</p>\n"
  " </body>\n"
  "</html>\n";

void handle_get (int fd, char *url)
{
    char path[256], data_to_send[BYTES];
    int file, bytes_read;
    char response[1024];

    //Because if no file is specified, index.html will be opened by default
    if ( strncmp(url, "/\0", 2)==0 )
  	url = "/index.html";  

    strcpy(path, ROOT);
    strcpy(&path[strlen(ROOT)], url);
    printf("file: %s\n", path);

    if ( (file = open(path, O_RDONLY)) != -1 )	//FILE FOUND
    {
	printf("%s", ok_response);
	send(fd, ok_response, strlen (ok_response), 0);
	while ( (bytes_read = read(file, data_to_send, BYTES))>0 )
	    write (fd, data_to_send, bytes_read);
    }
    else {					//FILE NOT FOUND
        snprintf (response, sizeof (response), not_found_response_template, url);
	printf ("%s", response);        
	write (fd, response, strlen (response));
    }
}

int errexit(const char *format, ...);
void handle_get (int fd, char *url);

//client connection
void handle_http(int connection_fd)
{
    char buffer[256];
    ssize_t bytes_read;

    char method[sizeof (buffer)];
    char url[sizeof (buffer)];
    char protocol[sizeof (buffer)];
    char response[1024];

    memset( (void*)buffer, 0, sizeof (buffer) );

    /* Read some data from the client.  */
    bytes_read = recv(connection_fd, buffer, sizeof (buffer) - 1, 0);

    /* The first line the client sends is the HTTP request, which is
       composed of a method, the requested page, and the protocol
       version.  */
    sscanf (buffer, "%s %s %s", method, url, protocol);

    /* The client may send various header information following the
       request.  We need to read any data the client tries to send.  Keep
       on reading data until we get to the end of the header, which is
       delimited by a blank line.  HTTP specifies CR/LF as the line
       delimiter.  */
    while (strstr (buffer, "\r\n\r\n") == NULL)
      	bytes_read = read (connection_fd, buffer, sizeof (buffer));

    /* Make sure the last read didn't fail.  If it did, there's a
       problem with the connection, so give up.  */
    if (bytes_read == -1) {
      	close (connection_fd);
     	return;
    }

    /* Check the protocol field.  We understand HTTP versions 1.0 and 1.1.  */
    if (strcmp (protocol, "HTTP/1.0") && strcmp (protocol, "HTTP/1.1")) {
      /* We don't understand this protocol.  Report a bad response.  */
      write (connection_fd, bad_request_response, sizeof (bad_request_response));
    }
    else if (strcmp (method, "GET")) {
      /* This server only implements the GET method.  The client
	 specified some other method, so report the failure.  */
      snprintf (response, sizeof (response), bad_method_response_template, method);
      printf("%s", response);
      printf("%s", buffer);
      write (connection_fd, response, strlen (response));
    } 
    else { 
	/* A valid request.  Process it.  */
      	handle_get (connection_fd, url);
    }

    //Closing SOCKET
    shutdown (connection_fd, SHUT_RDWR);   	//All further send and recieve operations are DISABLED...
    close(connection_fd);
}

void handle_tcp(int socket)
{ 
    // Start function 
    int recv_size = 0,
	size = 0, 
	read_size, 
	write_size, 
	packet_index = 1,
	stat;

    int pid;

    static struct timeval tm1, tm2;
    //unsigned long long t;
    float t;

    pid = getpid();

    char imagearray[BUF_SIZE + 1];
    FILE *image;

    gettimeofday(&tm1, NULL);

    //Find the size of the image
    do {
	stat = read(socket, &size, sizeof(int));
    } while (stat < 0);

    char buffer[] = "ACK_GET";

    //Send our verification signal
    do {
	stat = write(socket, &buffer, sizeof(int));
    } while (stat < 0);

    char name[16];
    sprintf(name, "out_%d.jpeg", pid);
    image = fopen(name, "w");

    if( image == NULL) {
    	printf("Error has occurred. Image file could not be opened\n");
	exit(0);
    }

    //Loop while we have not received the entire file yet
    struct timeval timeout = {10,0};

    fd_set fds;
    int buffer_fd;

    while(recv_size < size) {
    	FD_ZERO(&fds);
    	FD_SET(socket, &fds);

    	buffer_fd = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);

    	if (buffer_fd < 0)
       	    printf("error: bad file descriptor set.\n");

    	if (buffer_fd == 0)
       	    printf("error: buffer read timeout expired.\n");

    	if (buffer_fd > 0) {
            do{
               	read_size = read(socket, imagearray, BUF_SIZE + 1);
            } while (read_size < 0);

            //Write the currently read data into our image file
            write_size = fwrite(imagearray,1,read_size, image);

            if(read_size !=write_size) {
                printf("error in read write\n");    
	    }

            //Increment the total number of bytes read
            recv_size += read_size;
            packet_index++;
    	}
    }

    fclose(image);
    close(socket);

    // Print process time
    gettimeofday(&tm2, NULL);
    t = 1000000 * (tm2.tv_sec - tm1.tv_sec) + (tm2.tv_usec - tm1.tv_usec);
    printf("[%d] %s: %0.3f ms\n", pid, "processing time:", t/1000);

    exit(0);
}

