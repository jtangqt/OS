.PHONY: all clean debug run1

INCLUDE = inc/ 
INCLUDE_OPTS = $(foreach d, $(INCLUDE), -I$d)
CXX = gcc

OUT_FILE = fifo_test spinlock_test
OUT_DEPS = src/cv.c src/fifo.c src/spinlock.c inc/*

all: $(OUT_FILE)

fifo_test: $(OUT_DEPS) src/fifo_test.c
	@$(CXX) $(INCLUDE_OPTS) -o fifo_test $^

spinlock_test: $(OUT_DEPS) src/spin_test.c
	@$(CXX) $(INCLUDE_OPTS) -o spinlock_test $^

clean:
	rm -f *.exe *.o *.stackdump *~ fifo_test spinlock_test

run1:
	./spinlock_test 10 100000