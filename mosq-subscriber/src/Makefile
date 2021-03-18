BIN:=mosq-subscriber
LDFLAGS:=-lmosquitto -lsqlite3

.PHONY: all clean

all:$(BIN)

$(BIN): mosq.c db.c
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(BIN)