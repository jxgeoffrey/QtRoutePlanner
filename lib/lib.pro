TARGET = PugiXml
TEMPLATE = lib
CONFIG += static

HEADERS += pugiconfig.hpp \
			pugixml.hpp

SOURCES += pugixml.cpp 

#keep the generated lib in the project lib folder
DESTDIR = $$PWD/

