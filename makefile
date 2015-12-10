
####### Compiler, tools and options

CC            = gcc
CXX           = g++
LINK          = g++

####### Flags

CXXFLAGS      = -std=c++11

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = ejudge_test.cpp

OBJECTS       = ejudge_test.o

TARGET        = solution

#all: $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS)

clean:
	$(RM) $(OBJECTS)
	$(RM) $(TARGET)

ejudge_test.o: ejudge_test.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ejudge_test.o ejudge_test.cpp
