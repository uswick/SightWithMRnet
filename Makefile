DBGLOG_O := dbglog.o widgets.o attributes.o binreloc.o getAllHostnames.o widgets/valSelector.o widgets/trace.o
DBGLOG_H := dbglog.h dbglog_internal.h widgets.h attributes.h binreloc.h getAllHostnames.h widgets/valSelector.h widgets/trace.h
DBGLOG := ${DBGLOG_O} ${DBGLOG_H} gdbLineNum.pl dbglogDefines.pl

OS := $(shell uname -o)
ifeq (${OS}, Cygwin)
EXE := .exe
endif

all: libdbglog.a allExamples \
     widgets/shellinabox/bin/shellinaboxd${EXE} widgets/mongoose/mongoose${EXE} widgets/graphviz/bin/dot${EXE} script/taffydb \
     widgets/ID3-Decision-Tree \
     external_scripts
	chmod 755 html img script killP
	chmod 644 html/* img/* script/*
	chmod 755 widgets/canviz-0.1 script/taffydb
	chmod 644 widgets/canviz-0.1/* script/taffydb/*
	chmod 755 widgets/canviz-0.1/excanvas widgets/canviz-0.1/lib widgets/canviz-0.1/path widgets/canviz-0.1/prototype
	chmod 644 widgets/canviz-0.1/*/*

# Set this to the current Operating System (needed by the Mongoose web server). 
# Choices: linux|bsd|solaris|mac|windows|mingw|cygwin
ifeq (${OS}, Cygwin)
OS_MONGOOSE := cygwin
else
OS_MONGOOSE := linux
endif

DBGLOG_PATH = `pwd`

# Set to "-DGDB_ENABLED" if we wish gdb support to be enabled and otherwise not set
GDB_ENABLED := -DGDB_ENABLED

# The port on which dbglog sets up a daemon that invokes gdb so that it runs upto a particular point
# in the target application's execution
GDB_PORT := 17500

allExamples:
	cd examples; make

runExamples:
	cd examples; make run

#pattern${EXE}: pattern.C pattern.h libdbglog.a ${DBGLOG_H}
#	g++ -g pattern.C -L. -ldbglog  -o pattern${EXE}

libdbglog.a: ${DBGLOG_O} ${DBGLOG_H}
	ar -r libdbglog.a ${DBGLOG_O}
	
dbglog.o: dbglog.C dbglog.h attributes.h
	g++ -g dbglog.C -DROOT_PATH="\"${CURDIR}\"" ${GDB_ENABLED} -DGDB_PORT=${GDB_PORT} -c -o dbglog.o
	
widgets.o: widgets.C widgets.h attributes.h
	g++ -g widgets.C -DROOT_PATH="\"${CURDIR}\"" ${GDB_ENABLED} -DGDB_PORT=${GDB_PORT} -c -o widgets.o
	
attributes.o: attributes.C attributes.h
	g++ -g attributes.C -DROOT_PATH="\"${CURDIR}\"" ${GDB_ENABLED} -DGDB_PORT=${GDB_PORT} -c -o attributes.o

widgets/valSelector.o: widgets/valSelector.C widgets/valSelector.h
	g++ -g widgets/valSelector.C -DROOT_PATH="\"${CURDIR}\"" ${GDB_ENABLED} -DGDB_PORT=${GDB_PORT} -c -o widgets/valSelector.o

widgets/trace.o: widgets/trace.C widgets/trace.h
	g++ -g widgets/trace.C -DROOT_PATH="\"${CURDIR}\"" ${GDB_ENABLED} -DGDB_PORT=${GDB_PORT} -c -o widgets/trace.o

binreloc.o: binreloc.c binreloc.h
	g++ -g binreloc.c -c -o binreloc.o

getAllHostnames.o: getAllHostnames.C getAllHostnames.h
	g++ -g getAllHostnames.C -c -o getAllHostnames.o

gdbLineNum.pl: setupGDBWrap.pl dbglog.C
	./setupGDBWrap.pl

dbglogDefines.pl:
	printf "\$$main::dbglogPath = \"${DBGLOG_PATH}\";" > dbglogDefines.pl

clean:
	./killP widgets/mongoose/mongoose
	./killP widgets/shellinabox/bin/shellinaboxd
	rm -rf dbg *.o *.exe widgets/shellinabox* widgets/mongoose* widgets/graphviz* gdbLineNum.pl
	cd examples; make clean

widgets/shellinabox/bin/shellinaboxd${EXE}:
	rm -f widgets/shellinabox-2.14.tar.gz
	cd widgets; wget --no-check-certificate https://shellinabox.googlecode.com/files/shellinabox-2.14.tar.gz
	cd widgets; tar -xf shellinabox-2.14.tar.gz
	cd widgets/shellinabox-2.14; ./configure --prefix=${CURDIR}/widgets/shellinabox
	cd widgets/shellinabox-2.14; make
	cd widgets/shellinabox-2.14; make install
	rm -r widgets/shellinabox-2.14 widgets/shellinabox-2.14.tar.gz

widgets/mongoose/mongoose${EXE}:
	rm -f widgets/mongoose-3.8.tgz
	cd widgets; wget --no-check-certificate https://mongoose.googlecode.com/files/mongoose-3.8.tgz
	cd widgets; tar -xf mongoose-3.8.tgz
	cd widgets; rm mongoose-3.8.tgz
	cd widgets/mongoose; make ${OS_MONGOOSE}

widgets/graphviz/bin/dot${EXE}:
ifeq (${OS},Cygwin)
	rm -rf widgets/graphviz-2.32.zip widgets/release
	cd widgets; wget http://www.graphviz.org/pub/graphviz/stable/windows/graphviz-2.32.zip
	cd widgets; unzip graphviz-2.32.zip
	mkdir widgets/graphviz
	mv widgets/release/* widgets/graphviz
	rm -r widgets/graphviz-2.32.zip widgets/release
	chmod 755 widgets/graphviz/bin/*
else
	cd widgets; wget http://www.graphviz.org/pub/graphviz/stable/SOURCES/graphviz-2.32.0.tar.gz
	cd widgets; tar -xf graphviz-2.32.0.tar.gz
	cd widgets/graphviz-2.32.0; export CC=gcc; export CXX=g++; ./configure --prefix=${CURDIR}/widgets/graphviz --disable-swig --disable-sharp --disable-go --disable-io --disable-java --disable-lua --disable-ocaml --disable-perl --disable-php --disable-python --disable-r --disable-ruby --disable-tcl --without-pic --without-efence --without-expat --without-devil --without-webp --without-poppler --without-ghostscript --without-visio --without-pangocairo --without-lasi --without-glitz --without-freetype2 --without-fontconfig --without-gdk-pixbuf --without-gtk --without-gtkgl --without-gtkglext --without-gts --without-glade --without-ming --without-qt --without-quartz --without-gdiplus --without-libgd --without-glut --without-sfdp --without-smyrna --without-ortho --without-digcola --without-ipsepcola --enable-static --disable-shared
	cd widgets/graphviz-2.32.0; make
	cd widgets/graphviz-2.32.0; make install
	mv widgets/graphviz/bin/dot_static widgets/graphviz/bin/dot
	rm -r widgets/graphviz-2.32.0 widgets/graphviz-2.32.0.tar.gz
endif

script/taffydb:
	cd script; wget --no-check-certificate https://github.com/typicaljoe/taffydb/archive/master.zip
	cd script; mv master master.zip; unzip master.zip
	mv script/taffydb-master script/taffydb
	rm script/master*
	chmod 755 script/taffydb
	chmod 644 script/taffydb/*

widgets/ID3-Decision-Tree:
	rm -fr widgets/master*
	cd widgets; wget --no-check-certificate https://github.com/willkurt/ID3-Decision-Tree/archive/master.zip
	cd widgets; mv master master.zip; unzip master.zip
	mv widgets/ID3-Decision-Tree-master widgets/ID3-Decision-Tree
	rm -r widgets/master*
	chmod 755 widgets/ID3-Decision-Tree
	chmod 644 widgets/ID3-Decision-Tree/*
	chmod 755 widgets/ID3-Decision-Tree/data widgets/ID3-Decision-Tree/js
	chmod 644 widgets/ID3-Decision-Tree/*/*

# JavaScript files from standard JS libraries that we wish to keep local copies of

external_scripts: widgets/yui-min.js widgets/jquery-1.8.1.min.js widgets/underscore-min.js widgets/d3.v2.js

widgets/yui-min.js:
	cd widgets; wget http://yui.yahooapis.com/3.11.0/build/yui/yui-min.js

widgets/jquery-1.8.1.min.js:
	cd widgets; wget http://code.jquery.com/jquery-1.8.1.min.js
	
widgets/underscore-min.js:
	cd widgets; wget http://cdnjs.cloudflare.com/ajax/libs/underscore.js/1.3.3/underscore-min.js

widgets/d3.v2.js:
	cd widgets; wget http://d3js.org/d3.v2.js
