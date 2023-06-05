CROSS_COMPILE = aarch64-none-linux-gnu-

defalut: 

ifdef CROSS_COMPILE
	$(CROSS_COMPILE)gcc -o writer writer.c
else
	gcc -o writer writer.c
endif

clean:
	rm -rf writer