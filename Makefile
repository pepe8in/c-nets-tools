CC = gcc
CFLAGS = `pkg-config --cflags --libs gtk+-3.0`
SRC = src/main.c src/porteye.c
OUT = exe/c-nets-tools

$(OUT): $(SRC)
	$(CC) -o $(OUT) $(SRC) $(CFLAGS)

clean:
	rm -f $(OUT)
