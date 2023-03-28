OBJECTS="build/sp_json_reader.c.o build/sp_lines_reader.c.o build/sp_port.c.o build/sp_util.c.o"

TESTS="SerialportJsonReader_test SerialportLinesReader_test"

make

for i in $TESTS; do
	gcc -o build/$i.elf test/$i.c -g -I./lib $OBJECTS -lserialport
done

for i in $TESTS; do
	echo ">>> testing $i"
	./build/$i.elf
	echo
done

