CC = gcc
CFLAGS = `pkg-config --cflags --libs gtk+-3.0` -lcrypto
SRC = src/main.c src/porteye.c src/filesecure.c src/interface.c
OUT = exe/c-nets-tools

$(OUT): $(SRC)
	$(CC) -o $(OUT) $(SRC) $(CFLAGS)

clean:
	rm -f $(OUT)
