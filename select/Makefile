CC=gcc
OUTPUT=-o

.PHONY:ALL
ALL:select_client select_server new_select_server


select_client:select_client.c
	$(CC) $^ $(OUTPUT) $@
select_server:select_server.c
	$(CC) $^ $(OUTPUT) $@
new_select_server:new_select_server.c
	$(CC) $^ $(OUTPUT) $@

.PHONY:clean
clean:
	@rm -rf select_server select_client new_select_server
	@echo "delete success!"
