
.PHONY: doc clean all

all : doc

doc :
	(cd ..; doxygen doc/Doxyfile)
	echo "Acceder a file://$$PWD/html/index.html"

clean :
	rm -rf html
