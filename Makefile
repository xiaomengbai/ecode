all: tool main
	mkdir -p bin
	mv src/prog bin/
	mv tools/diff bin/
tool:
	make -C tools/

main:
	make -C src/

clean: cleanmain cleantools
	rm -rf bin/

cleanmain:
	make -C src/ clean

cleantools:
	make -C tools/ clean

