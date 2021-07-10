CC	= g++ -std=gnu++11 -g3
CPPFLAGS= -Wno-deprecated -L/usr/local/lib64 -O3 -c 
LDFLAGS	= -O3 
SOURCES	= tempath.cpp
OBJECTS	= $(SOURCES:.cpp=.o)
EXECUTABLE=tempath

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o : 
	$(CC) $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o
