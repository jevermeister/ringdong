OBJS	= tuerklingel_detect.o
SOURCE	= tuerklingel_detect.c
HEADER	= 
OUT	= tuerklingel_detect
LFLAGS	 = -lconfuse -lmosquitto -lwiringPi -lrt -lcrypt -lm

all: tuerklingel_detect

tuerklingel_detect: $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c $(HEADER)
	$(CC) -c -o $@ $< $(LFLAGS)

clean:
	rm -f $(OBJS) $(OUT)
