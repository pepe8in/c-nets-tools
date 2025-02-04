CC = gcc
CFLAGS = `pkg-config --cflags --libs gtk+-3.0`
SRC = src/test.c
#SRC = src/main.c src/porteye.c
OUT = exe/test2

$(OUT): $(SRC)
	$(CC) -o $(OUT) $(SRC) $(CFLAGS)

clean:
	rm -f $(OUT)
