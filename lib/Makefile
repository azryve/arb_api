LIBS := nl-3 \
       nl-genl-3
LIBARGS = $(foreach lib,${LIBS},-l ${lib})

INCDIR := /usr/include/libnl3 \
	${CURDIR}/../include

CFLAGS += $(foreach inc, ${INCDIR}, -I${inc})

OBJS := arb_api.o\
	dump.o\
       	arb_api\
	dump

build: ${OBJS}

clean:
	@echo Cleaning files
	$(shell rm ${OBJS} 2> /dev/null || true)

%: %.o
	$(CC) -o $@ $< ${LIBARGS}

.PHONY: build clean
