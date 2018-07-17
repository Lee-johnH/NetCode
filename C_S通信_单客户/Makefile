CC=gcc
OUTPUT=-o

all:server client

server:server.c
	$(CC) $^ $(OUTPUT) $@
client:client.c
	$(CC) $^ $(OUTPUT) $@

.PHONY:clean
clean:
	@rm -rf server client
	@echo "del ok"
