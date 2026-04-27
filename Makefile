CXX = g++
CXXFLAGS = -Wall -std=c++11 -MMD -MP -DMEMTRACE


# Program Generation
SOURCES = main.cpp Cipher.cpp CipherIterator.cpp XORCipher.cpp CaesarCipher.cpp
DEBUG = memtrace.cpp

CPP = $(SOURCES) $(DEBUG)

OBJECTS = $(CPP:.cpp=.o)
DEPS = $(CPP:.cpp=.d)
-include $(DEPS)

cipher: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@
	./$@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


# Documentation Generation
DOCSOURCES = $(wildcard docs/*.md) docs/diagram.puml
DOCOBJECTS = $(DOCSOURCES:.md=.pdf) docs/diagram.svg
doc: $(DOCOBJECTS)

%.svg: %.puml
	plantuml $< -tsvg
%.pdf: %.md
	pandoc -V lang=hu -t pdf -o $@ --pdf-engine=xelatex $<


# Cleaning up
.PHONY: clean
clean:
	rm -f *.o *.d cipher
