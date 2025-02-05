CC = gcc
CFLAGS = `pkg-config --cflags --libs gtk+-3.0` -I/usr/include/libxml2 -lcrypto -lpcap -lxml2 -lsqlite3
SRC = src/main.c src/interface.c src/porteye.c src/filesecure.c src/ddoswatcher.c src/xmlbuilder.c
OUT = exe/c-nets-tools

$(OUT): $(SRC)
	$(CC) -o $(OUT) $(SRC) $(CFLAGS)

clean:
	rm -f $(OUT)
