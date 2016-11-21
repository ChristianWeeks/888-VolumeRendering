

OFILES = Matrix.o OIIOFiles.o Camera.o boundingbox.o simplexnoise.o simplextextures.o grid.o Vector.o Color.o

ROOTDIR = .
LIB = $(ROOTDIR)/libVR.a 

LINKS =  `Magick++-config  --cppflags --cxxflags --ldflags --libs` -L/usr/local/lib -lfftw3 -lOpenImageIO

INCLUDES = -I ./ `Magick++-config  --cppflags --cxxflags`

CXX = clang++ -std=c++11 -Wall -g -O2 -D_THREAD_SAFE -pthread
SWIGEXEC = swig

#PYTHONINCLUDE = -I/usr/include/python2.7 -I/usr/lib/python2.7/config
#INCLUDES = -I /usr/local/include/ -I /opt/local/include/ $(PYTHONINCLUDE)

.C.o:
	$(CXX) -c $(INCLUDES) $<

all: $(OFILES)
	ar rv $(LIB) $?

$(LIB): $(OFILES)
	ar rv $(LIB) $?	

run:  volumesTest.C Volume.h Image.h
	$(CXX) $(INCLUDES) volumesTest.C -o run  $(LINKS) $(PYTHONINCLUDE)

simplex:  SimplexTest.cpp $(LIB)
	$(CXX) $(INCLUDES) SimplexTest.cpp -o simplex $(LIB) $(LINKS) $(PYTHONINCLUDE)

img:  imageTest.C $(LIB)
	$(CXX) $(INCLUDES) imageTest.C -o img $(LIB) $(LINKS)

wisp:  wispAnimation.C $(LIB)
	$(CXX) $(INCLUDES) wispAnimation.C -o img $(LIB) $(LINKS)

genswig:	swig/Volume.i $(OFILES)
	$(SWIGEXEC) -c++ -python -shadow swig/Volume.i

clean:
	rm -f *.o
