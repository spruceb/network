CFLAGS = -std=c11 -Wall -Wextra -pedantic -Wno-sign-compare
LIBS = vector.c network_util.c strings.c
SOURCES = $(LIBS)

INC = /Users/spruce/Developer/Projects/Personal/networking/

http-test: LIBS += http_lib.c
http-test: DRIVER = tests/functional/http_test.c
http-test: EXECUTABLE = http-test
http-test: all

network-test: DRIVER = tests/functional/util_test.c
network-test: EXECUTABLE = util-test
network-test: all

unit-http: LIBS += http_lib.c
unit-http: DRIVER = tests/unit/test_http_lib.c
unit-http: EXECUTABLE = unit-test-http
unit-http: all

INC_PARAMS = $(foreach d,$(INC),-I $d)
all: $(LIBS) $(DRIVER)
	$(CC) $(INC_PARAMS) $(CFLAGS) $(LIBS) $(DRIVER) -o $(EXECUTABLE)
