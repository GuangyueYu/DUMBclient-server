all: DUMBclient DUMBserve

DUMBclient:
	gcc DUMBclient.c -o DUMBclient

DUMBserve:
	gcc DUMBserver.c -o DUMBserve -lpthread
	
clean:
	rm -f DUMBclient
	rm -f DUMBserve
