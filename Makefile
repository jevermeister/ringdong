OBJS	= ringdong.o
SOURCE	= ringdong.c
HEADER	= 
OUT	= ringdong
LFLAGS	 = -lconfuse -lmosquitto -lwiringPi -lrt -lcrypt -lm

all: ringdong

ringdong: $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c $(HEADER)
	$(CC) -c -o $@ $< $(LFLAGS)

clean:
	rm -f $(OBJS) $(OUT)
