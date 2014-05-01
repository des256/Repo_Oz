all: ozserver

OBJS := Camera.o main.o Microphone.o Server.o Servos.o Speaker.o SpeechToText.o Task.o TextToSpeech.o Vision.o

LIBS := -ljpeg -lz -lm -ljson -lasound -lbabile -lcurl -lFLAC -lpthread

%.o: %.cpp
	g++ -I../include -O0 -g3 -Wall -c -fmessage-length=0 -o "$@" "$<"

ozserver: $(OBJS)
	g++ -L../arch/robokind-r50/lib -o "ozserver" $(OBJS) $(LIBS)
	-cp ozserver ../arch/robokind-r50/bin

clean:
	-rm *.o ozserver

