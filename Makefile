CFLAGS = -std=c11 -Wall -Wextra -pedantic -Wno-sign-compare
LIBS = vector.c network_util.c
SOURCES = $(LIBS)

INC=$(/Users/spruce/Developer/Projects/Personal/networking)

http-test: LIBS += http_lib.c
http-test: DRIVER = http_test.c
http-test: EXECUTABLE = http-test
http-test: all

network-test: DRIVER = util_test.c
network-test: EXECUTABLE = util-test
network-test: all

http-unit-test: LIBS += http_lib.c
http-unit-test: DRIVER = test_http_lib.c
http-unit-test: EXECUTABLE = unit-test
http-unit-test: all

INC_PARAMS=$(foreach d, $(INC), -I$d)
all: $(LIBS) $(DRIVER)
	$(CC) $(INC_PARAMS) $(CFLAGS) $(LIBS) $(DRIVER) -o $(EXECUTABLE)
