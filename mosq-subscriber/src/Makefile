BIN:=mosq-subscriber
LDFLAGS:=-lmosquitto -lsqlite3 -luci -ljson-c -lcurl

.PHONY: all clean

all:$(BIN)

$(BIN): mosq.c db.c uci-config.c email.c
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(BIN)
