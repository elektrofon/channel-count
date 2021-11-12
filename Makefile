channel-count: main.c
	gcc -o channel-count main.c -lasound

.PHONY: install
install: channel-count
	mkdir -p /usr/bin
	install channel-count /usr/bin

.PHONY: clean
clean:
	rm -f channel-count
