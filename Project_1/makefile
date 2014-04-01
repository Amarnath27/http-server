# the compiler: gcc for C program, define as g++ for C++
CC = gcc

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall

# typing 'make' will invoke the first target entry in the file 
# (in this case the default target entry)
# you can name this target entry anything, but "default" or "all"
# are the most commonly used names by convention
#
default: server client 
#daytime_client echo_client

server: errexit.o passivesock.o server_funcs.o server.o 
	$(CC) $(CFLAGS) -o server errexit.o passivesock.o server_funcs.o server.o 

client: client.o process_image.o 
	$(CC) $(CFLAGS) -o client process_image.o client.o

#daytime_client: errexit.o  connectsock.o daytime_client.o
#	$(CC) $(CFLAGS) -o daytime_client errexit.o  connectsock.o daytime_client.o

#echo_client: errexit.o connectsock.o tcp_client.o echo_client.o 
#	$(CC) $(CFLAGS) -o echo_client errexit.o connectsock.o tcp_client.o echo_client.o 

# Simple TCP echo server
#
server.o:  server.c
	$(CC) $(CFLAGS) -c server.c

# Simple TCP echo server
#
server_funcs.o:  server_funcs.c
	$(CC) $(CFLAGS) -c server_funcs.c

# Simple TCP echo server
#
client.o:  client.c
	$(CC) $(CFLAGS) -c client.c

# Simple TCP echo server
#
process_image.o:  process_image.c
	$(CC) $(CFLAGS) -c process_image.c

#echo_client.o:  echo_client.c
#	$(CC) $(CFLAGS) -c echo_client.c
# Simple TCP echo server
#
#daytime_client.o:  daytime_client.c
#	$(CC) $(CFLAGS) -c daytime_client.c

# Simple TCP echo server
#

passivesock.o:  passivesock.c
	$(CC) $(CFLAGS) -c passivesock.c

connectsock.o:  connectsock.c
	$(CC) $(CFLAGS) -c connectsock.c

#tcp_client.o:  tcp_client.c
#	$(CC) $(CFLAGS) -c tcp_client.c

# Simple TCP echo server
#

errexit.o:  errexit.c
	$(CC) $(CFLAGS) -c errexit.c

# To start over from scratch, type 'make clean'.  This
# removes the executable file, as well as old .o object
# files and *~ backup files:
#
clean: 
	rm -f server client *.o *~ *.jpeg
