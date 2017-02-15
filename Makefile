

OFILES = Matrix.o OIIOFiles.o Camera.o boundingbox.o simplexnoise.o simplextextures.o Vector.o Color.o grid.o fields.o SceneManager.o 

ROOTDIR = .
LIB = $(ROOTDIR)/libVoyager.a

LINKS = -L/usr/local/lib -lfftw3 -lOpenImageIO -fopenmp

CXX = clang++ -std=c++11 -Wall -g -O2 -fPIC -D_THREAD_SAFE -pthread 
SWIGEXEC = swig
SWIGLD = $(CXX) -shared -DINCLUDE_TEMPLATES -DDEBUG
OIIOLIB = -L/group/dpa/lib/lib -lOpenImageIO
CHROMELIB = -ldl

PYTHONINCLUDE = -I/usr/include/python2.7 -I/usr/lib/python2.7/config
MAGICKINCLUDE = `Magick++-config  --cppflags --cxxflags --ldflags`
OIIOINCLUDE = -I/group/dpa/include/OpenImageIO
INCLUDES = -I ./ $(PYTHONINCLUDE) $(MAGICKINCLUDE) $(OIIOINCLUDE)

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
	$(SWIGEXEC) -c++ -python -shadow -I./ swig/voyager.i
	$(CXX) -c swig/voyager_wrap.cxx  $(INCLUDES) -o swig/voyager_wrap.o
	$(SWIGLD) swig/voyager_wrap.o $(OIIOLIB) $(MAGICKINCLUDE) $(CHROMELIB) -L/usr/local/lib -lfftw3 -L./ -lVoyager -o swig/_voyager.so
#-Wl,-rpath,. -L. -Wl,--whole-archive -lVoyager -Wl,--no-whole-archive 
clean:
	rm -rf *.o swig/*.so swig/*~ swig/*.cxx swig/voyager.py*
