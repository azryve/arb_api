ifneq (,${KERNELRELEASE})
#
#	Kbuild part
#

obj-${CONFIG_ARB_API} = arb_genl_api.o

endif  # ifneq (,${KERNELRELEASE})
ifeq (,${KERNELRELEASE})
#
#	Makefile part
#

include common.mk

MODULE = ARB_API
OBJS = arb_genl_api.o
INCLUDE = ${CURDIR}/../include

BUILDFLAGS = $(foreach obj, ${OBJS}, CFLAGS_${obj}=-I${INCLUDE})
build:
	@echo Building module
	@+$(call kernelbuild,modules,${BUILDFLAGS})

clean:
	@echo Cleaning files
	$(shell rm *.ko *.mod.c *.o modules.order Module.symvers 2>/dev/null || true)


.PHONY: build 
endif  # ifeq (,${KERNELRELEASE})
