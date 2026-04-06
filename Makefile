CXX = g++
CXXFLAGS = -Wall -std=c++11 -MMD -MP


# Program Generation
SOURCES = main.cpp Cipher.cpp CipherIterator.cpp XORCipher.cpp CaesarCipher.cpp

OBJECTS = $(SOURCES:.cpp=.o)
DEPS = $(SOURCES:.cpp=.d)
-include $(DEPS)

cipher: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@
	./$@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


# Documentation Generation
DOCSOURCES = $(wildcard docs/*.md)
DOCOBJECTS = $(DOCSOURCES:.md=.pdf)
doc: $(DOCOBJECTS)

%.pdf: %.md
	pandoc -V lang=hu -t pdf -o $@ --pdf-engine=xelatex $<


# Cleaning up
.PHONY: clean
clean:
	rm -f *.o *.d cipher
