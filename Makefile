CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g

project: project.o ppm_io.o image_manip.o
	$(CC) -o project project.o ppm_io.o image_manip.o -lm

checkerboard: checkerboard.o ppm_io.o
	$(CC) -o checkerboard checkerboard.o ppm_io.o

img_cmp: img_cmp.o ppm_io.o
	$(CC) -g img_cmp img_cmp.o

ppm_io.o: ppm_io.c ppm_io.h
	$(CC) $(CFLAGS)	-c ppm_io.c 

image_manip.o: image_manip.c image_manip.h ppm_io.h
	$(CC) $(CFLAGS) -c image_manip.c 

img_cmp.o: img_cmp.c ppm_io.h
	$(CC) $(CFLAGS) -c img_cmp.c 

checkerboard.o: checkerboard.c
	$(CC) $(CFLAGS) -c checkerboard.c 
clean:
	rm -f *~ *.o main
