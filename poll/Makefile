CC=gcc
OUTPUT=-o

.PHONY:ALL
ALL:poll_client poll_server

poll_client:poll_client.c wrap.c
	$(CC) $^ $(OUTPUT) $@
poll_server:poll_server.c wrap.c
	$(CC) $^ $(OUTPUT) $@

.PHONY:clean
clean:
	@rm -rf poll_server poll_client 
	@echo "delete success!"
