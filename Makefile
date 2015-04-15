CC     := g++
CFLAGS := -O2 -std=c++0x -lpthread
TARGET := tunnel
OBJS   := main.o tun.o network.o socket.o 

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS)

%.o: %.cpp
	$(CC) -c $< -o $@ $(CFLAGS)
	
clean :
	rm -f $(TARGET)
	rm -f *.o
