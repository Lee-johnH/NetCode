CC=gcc
OUTPUT=-o

#		$@	代表当前规则的目标文件
#		$<	代表当前规则的第一个依赖文件
#		$^	代表当前规则的所有依赖文件


all:select_client select_server

select_client:select_client.c
	$(CC) $^ $(OUTPUT) $@
select_server:select_server.c
	$(CC) $^ $(OUTPUT) $@

.PHONY:clean
clean:
		@rm -fr select_client select_server
		@echo "del ok"
