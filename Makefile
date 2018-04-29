#CC=gcc
#OUTPUT=-o

#		$@	代表当前规则的目标文件
#		$<	代表当前规则的第一个依赖文件
#		$^	代表当前规则的所有依赖文件


select_client:select_client.c
	gcc -o select_client select_client.c
select_server:select_server.c
	gcc -o select_server select_server.c

.PHONY:clean
clean:
		@rm -fr select_client select_server
		@echo "del ok"
