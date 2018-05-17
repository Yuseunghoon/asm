#Makefile

OBJECTS = work01.o
TARGET =  work01
SRCS = $(OBJECTS:.o=.s)

$(TARGET): $(OBJECTS)
	gcc -o $@ $+
	
$(OBJECTS): $(SRCS)
	as -o $@ $<
	
clean:
	rm -vf $(TARGET) *.o
	
