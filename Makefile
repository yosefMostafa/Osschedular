dirbuild:
	gcc process_generator.c -o process_generator.out -Wall
	gcc clk.c -o clk.out
	gcc scheduler.c -o scheduler.out -lm -Wall 
	gcc process.c -o process.out -Wall
	gcc test_generator.c -o test_generator.out -Wall

clean:
	rm -f *.out  processes.txt

all: clean build

run:
	./process_generator.out