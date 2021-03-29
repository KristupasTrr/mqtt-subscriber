BIN:=mosq-subscriber
LDFLAGS:=-lmosquitto -lsqlite3 -luci

.PHONY: all clean

all:$(BIN)

$(BIN): mosq.c db.c uci-config.c
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(BIN)