all: tpfs

tpfs: object/main.o object/driver_base.o object/analizadorfs.o object/driver_fat.o object/driver_ext.o object/driver_ntfs.o
	@echo -e "Generando \033[33m$@\033[0m ..."
	g++ -g -o tpfs $^ -lstdc++

object/%.o: source/%.cpp include/%.h
	@echo -e "Compilando \033[33m$<\033[0m ..."
	g++ -g -O0 -Wno-address-of-packed-member -Iinclude -o $@ -c $<

.PHONY: clean
clean:
	rm -rf object/*.o source/*~ include/*~ tpfs



