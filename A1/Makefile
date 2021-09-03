all: client.c tserver.c
	gcc client.c -o client
	gcc tserver.c -o server -lpthread
clear:
	rm for_server for_client client_received
clear_all:
	rm for_server for_client client_received server client
