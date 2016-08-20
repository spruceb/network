network-test: network_util.c vector.c util_test.c
	gcc -std=c11 network_util.c vector.c util_test.c -o network
