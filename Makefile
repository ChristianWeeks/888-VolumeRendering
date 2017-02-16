

OFILES = Matrix.o OIIOFiles.o Camera.o boundingbox.o simplexnoise.o Vector.o Color.o grid.o fields.o SceneManager.o FastNoise.o

ROOTDIR = .
LIB = $(ROOTDIR)/libVoyager.a

CXX = g++ -std=c++11 -Wall -g -O2 -fPIC -D_THREAD_SAFE -pthread -lfftw3 -fopenmp
SWIGEXEC = swig
SWIGLD = $(CXX) -shared -DINCLUDE_TEMPLATES -DDEBUG
OIIOLIB = -L/group/dpa/lib -lOpenImageIO
CHROMELIB = -ldl

PYTHONINCLUDE = -I/usr/include/python2.7 -I/usr/lib/python2.7/config
MAGICKINCLUDE = `Magick++-config  --cppflags --cxxflags --ldflags`
OIIOINCLUDE = -I/group/dpa/include
INCLUDES = -I ./ $(PYTHONINCLUDE) $(MAGICKINCLUDE) $(OIIOINCLUDE)

LN_VOYAGER = -L$(ROOTDIR) -lVoyager
#INCLUDES = -I /usr/local/include/ -I /opt/local/include/ $(PYTHONINCLUDE)

.C.o:
	$(CXX) -c $(INCLUDES) $<

all: $(OFILES)
	ar rv $(LIB) $?

$(LIB): $(OFILES)
	ar rv $(LIB) $?

genswig:	swig/voyager.i $(OFILES)
	$(SWIGEXEC) -c++ -python -shadow -I./ swig/voyager.i
	$(CXX) -c swig/voyager_wrap.cxx  $(INCLUDES) -o swig/voyager_wrap.o
	$(SWIGLD) swig/voyager_wrap.o $(MAGICKINCLUDE) $(CHROMELIB) -lfftw3 -Wl,-rpath,. -L. -Wl,--whole-archive -lVoyager -Wl,--no-whole-archive $(OIIOLIB) -o swig/_voyager.so
#-Wl,-rpath,. -L. -Wl,--whole-archive -lVoyager -Wl,--no-whole-archive 
clean:
	rm -rf *.o swig/*.so swig/*~ swig/*.cxx swig/voyager.py*
