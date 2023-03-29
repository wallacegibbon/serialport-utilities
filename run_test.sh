OBJECTS=$(ls build/*.o | grep -v main)
TESTS=$(for i in $(ls test/*.c | grep -v main); do basename $i .c; done)

make clean && make

for i in $TESTS; do
	gcc -o build/$i.elf test/$i.c -g -I./lib $OBJECTS -lserialport
done

for i in $TESTS; do
	echo ">>> testing $i"
	./build/$i.elf
	echo
done

