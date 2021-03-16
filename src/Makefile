BIN:=mosq-subscriber
LDFLAGS:=-lmosquitto 

.PHONY: all clean

all:$(BIN)

$(BIN): mosq.c
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(BIN)