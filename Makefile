all: tool main
tool:
	make -C tools/

main:
	make -C src/

clean: cleanmain cleantools

cleanmain:
	make -C src/ clean

cleantools:
	make -C tools/ clean

