

OFILES = Matrix.o OIIOFiles.o Camera.o boundingbox.o simplexnoise.o simplextextures.o grid.o Vector.o Color.o SceneManager.o

ROOTDIR = .
LIB = $(ROOTDIR)/libVoyager.a

LINKS =  `Magick++-config  --cppflags --cxxflags --ldflags --libs` -L/usr/local/lib -lfftw3 -lOpenImageIO


CXX = g++ -std=c++11 -Wall -g -O2 -fPIC -D_THREAD_SAFE -pthread
SWIGEXEC = swig
SWIGLD = $(CXX) -shared -DINCLUDE_TEMPLATES -DDEBUG
OIIOLIB = /group/dpa/lib/libOpenImageIO.so
PYTHONINCLUDE = -I/usr/include/python2.7 -I/usr/lib/python2.7/config
INCLUDES = -I ./ `Magick++-config  --cppflags --cxxflags` $(PYTHONINCLUDE)
LN_VOYAGER = -L$(ROOTDIR) -lvoyager
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

wispw:  wispWedge.C $(LIB)
	$(CXX) $(INCLUDES) wispWedge.C -o img $(LIB) $(LINKS)

genswig:	swig/voyager.i $(OFILES)
	$(SWIGEXEC) -c++ -python -shadow -I. swig/voyager.i
	$(CXX) -c swig/voyager_wrap.cxx  $(INCLUDES) -o swig/voyager_wrap.o
	$(SWIGLD) swig/voyager_wrap.o $(LIB) $(OIIOLIB) -ldl -L/usr/local/lib -L/opt/local/lib -lfftw3 -o swig/_voyager.so

clean:
	rm -f *.o
