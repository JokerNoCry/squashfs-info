all: squashfs-info

squashfs-info: squashfs-info.o
	$(CC) $(LDFLAGS) -Wall -Werror squashfs-info.o -o squashfs-info
squashfs-info.o: squashfs-info.c
	$(CC) $(CFLAGS) -c squashfs-info.c

clean:
	rm -f *.o squashfs-info
