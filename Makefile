CC=g++
CFLAGS=-c -Wall -std=c++11
LDFLAGS= 
SOURCES=shift_tracking.cpp send_img_ssh.cpp uart.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=tracking.out
LFLAGS = -I/usr/include -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_video

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o tracking.out
