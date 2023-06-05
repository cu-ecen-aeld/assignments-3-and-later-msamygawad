writer: writer.c

ifdef CROSS_COMPILE
	$(CROSS_COMPILE)gcc -o writer writer.c
else
	echo "CROSS_COMPILE is not defined"
	gcc -o writer writer.c
endif

gcc:
	gcc -o writer writer.c

clean:
	rm -rf writer
