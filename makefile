CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = sensor_node
SOURCES = sensor_node.c http_client.c utils.c
HEADERS = http_client.h utils.h
OBJS = $(SOURCES:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: clean