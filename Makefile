CFLAGS = -W -Wall -Wextra -Wno-unused-variable -Wno-unused-function -Wno-c++98-compat -Wno-missing-prototypes -march=skylake-avx512 -O3 -ggdb3

HEADERS = *.h *.hpp

.PRECIOUS: %.o %-assert.o

#OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

.PHONY: all

BINS = bench-cuckoo-filter-bucket.exe bench-pd.exe pd-test-assert.exe

all: $(BINS) #$(OBJS) $(STATIC_OBJS)

clean:
	/bin/rm -f $(BINS) #$(OBJS) $(STATIC_OBJS)

%.o: %.cpp ${HEADERS} Makefile
	$(CXX) -c $(CFLAGS) -DNDEBUG $< -o $@

%.exe: %.o %.cpp Makefile
	$(CXX) $< -o $@ ${LDFLAGS}

%-assert.o: %.cpp ${HEADERS} Makefile
	$(CXX) -c $(CFLAGS) -O0 -fsanitize=undefined $< -o $@

%-assert.exe: %-assert.o %.cpp Makefile
	$(CXX) -fsanitize=undefined $< -o $@ ${LDFLAGS}
