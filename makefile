CC = g++
CFLAGS = -c -Wall -O3
LDFLAGS =
SOURCES = mysh.cpp stack.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = mysh

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS)

veryclean:
	rm $(OBJECTS) $(EXECUTABLE) *~