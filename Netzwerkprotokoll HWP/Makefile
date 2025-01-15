CFLAGS = -Wall -pthread -lb15fdrv -I$(INCLUDE_DIR)
CC = gcc
INCLUDE_DIR = headers
TARGET = final
all: final

$(TARGET): Main.o Reciever.o Writer.o Message.o DriverManager.o
	$(CC) -o $(TARGET) Main.o Reciever.o Writer.o Message.o DriverManager.o $(CFLAGS)

Main.o: Main.cpp
	@echo "Compiling Main"
	$(CC) $(CFLAGS) -c Main.cpp

Reciever.o: Reciever.cpp $(INCLUDE_DIR)/Reciever.h
	@echo "Compiling Reciever Class"
	$(CC) $(CFLAGS) -c Reciever.cpp

Writer.o: Writer.cpp $(INCLUDE_DIR)/Writer.h
	@echo "Compiling Writer Class"
	$(CC) $(CFLAGS) -c Writer.cpp

Message.o: Message.cpp $(INCLUDE_DIR)/Message.h
	@echo "Compiling Message Class"
	$(CC) $(CFLAGS) -c Message.cpp

DriverManager.o: DriverManager.cpp $(INCLUDE_DIR)/DriverManager.h
	@echo "Compiling DriverManager Class"
	$(CC) $(CFLAGS) -c DriverManager.cpp

clean:
	@echo "Removing everything but the source files"
	@rm -f *.o final

run-w:
	./$(TARGET) -w

run-r:
	./$(TARGET) -r

run:
	./$(TARGET) $(ARGS)

.PHONY: clean run run-w run-r