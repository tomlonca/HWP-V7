# Nombre del ejecutable
TARGET = main

# Compilador y flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread -lb15fdrv

# Archivos fuente
SRCS = Main.cpp DriverManager.cpp Writer.cpp Reciever.cpp

# Archivos objeto
OBJS = $(SRCS:.cpp=.o)

# Regla por defecto
all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(OBJS)
    $(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Regla para compilar archivos .cpp a .o
%.o: %.cpp
    $(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para limpiar archivos compilados
clean:
    rm -f $(TARGET) $(OBJS)

# Regla para limpiar todo
distclean: clean
    rm -f *~ *.bak

# Dependencias
Main.o: DriverManager.h Writer.h Reciever.h
DriverManager.o: DriverManager.h
Writer.o: Writer.h DriverManager.h
Reciever.o: Reciever.h DriverManager.h
