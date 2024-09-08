LIBRARY			= nodoka.dll
CXX					= clang++
CXXFLAGS		= -I . -Wall -Ofast -std=c++17 -g
LD					= clang++
LDFLAGS			= -shared -lmsvcrt -Xlinker /NODEFAULTLIB:LIBCMT
OBJS				= nodoka.o
SAORI_OBJS			= \
					  lib.o \
					  util.o \
					  windows/dll.o \
					  base/header.o
ALL					= all

.SUFFIXES: .cc .o

.PHONY: all
$(ALL): $(LIBRARY)

$(LIBRARY): $(OBJS) $(SAORI_OBJS)
	$(LD) $(LDFLAGS) -o $(LIBRARY) $(OBJS) $(SAORI_OBJS)

.cc.o:
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -f *.o windows/*.o base/*.o *.exp *.lib *.exe *.dll
