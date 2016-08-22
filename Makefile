CFLAGS = -std=c11 -Wall -Wextra -pedantic -Wno-sign-compare
LIBS = vector.c network_util.c
SOURCES = $(LIBS)

http-test: LIBS += http_lib.c
http-test: DRIVER = http_test.c
http-test: EXECUTABLE = http-test
http-test: all

network-test: DRIVER = util_test.c
network-test: EXECUTABLE = util-test
network-test: all

all: $(LIBS) $(DRIVER)
	$(CC) $(CFLAGS) $(LIBS) $(DRIVER) -o $(EXECUTABLE)
