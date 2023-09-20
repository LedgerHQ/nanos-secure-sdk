
INC := -I . -I ../include
DEFINES := -DLINUX_SIMU -DHAVE_LANGUAGE_PACK -DNBGL_GENERATE_DATA_TEST

ifdef TARGET_STAX
DEFINES += -DHAVE_SE_TOUCH
else
DEFINES += -DBAGL_HEIGHT=64 -DBAGL_WIDTH=128

endif

default: generate_data_test.c
	gcc $(INC) generate_data_test.c $(DEFINES) ../src/nbgl_serialize.c -o generate_data_test

run_test: default
	./generate_data_test > data_test.txt
	pytest -s -vv
	rm data_test.txt

clean:
	rm generate_data_test
