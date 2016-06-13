BUILD_KERNEL = $(shell uname -r)
SEARCH_SRC = /lib/modules/${BUILD_KERNEL}/build
BUILD_DIR = ${CURDIR}
KERNEL_SRC = $(shell [ -e ${SEARCH_SRC} ] && echo ${SEARCH_SRC})
ifeq (,${KERNEL_SRC})
        $(warning *** Kernel headers not found, try to run) 
        $(error apt-get install linux-headers-${BUILD_KERNEL}-generic)
endif  # ifeq (,${KERNEL_SRC})

kernelbuild = ${MAKE} -C ${KERNEL_SRC} \
                       M=${BUILD_DIR} \
                       CONFIG_${MODULE}=m \
                       ${2} ${1}
