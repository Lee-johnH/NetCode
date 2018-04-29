CC=gcc
OUTPUT=-o

.PHONY:ALL
ALL:epoll_client epoll_server epoll_serv_ET_LT epoll_serv_ONESHOT

epoll_client:epoll_client.c wrap.c
	$(CC) $^ $(OUTPUT) $@
epoll_server:epoll_server.c wrap.c
	$(CC) $^ $(OUTPUT) $@
epoll_serv_ET_LT:epoll_serv_ET_LT.c wrap.c
	$(CC) $^ $(OUTPUT) $@
epoll_serv_ONESHOT:epoll_serv_ONESHOT.c
	$(CC) $^ $(OUTPUT) $@ -lpthread

.PHONY:clean
clean:
	@rm -rf epoll_server epoll_client epoll_serv_ET_LT epoll_serv_ONESHOT
	@echo "delete success!"
