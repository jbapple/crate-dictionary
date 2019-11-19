CFLAGS = -W -Wall -Wextra -Wno-unused-variable -Wno-unused-function -Wno-c++98-compat -Wno-missing-prototypes -march=skylake-avx512 -O3 -ggdb3 -DNDEBUG

HEADERS = *.h *.hpp

.PRECIOUS: %.o

#OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

.PHONY: all

BINS = pd-test.exe bench-cuckoo-filter-bucket.exe bench-pd.exe

all: $(BINS) #$(OBJS) $(STATIC_OBJS)

clean:
	/bin/rm -f $(BINS) #$(OBJS) $(STATIC_OBJS)

%.o: %.cpp ${HEADERS} Makefile
	$(CXX) -c $(CFLAGS) $< -o $@

%.exe: %.o %.cpp Makefile
	$(CXX) $< -o $@ ${LDFLAGS}
