# note this must match version macros in initialize_block.h
FD_VERSION=11.0

PROGRAM = initialize_block
OBJS = initialize_block.o

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

# avoid linking against all libs that the server links against (xml, selinux, ...)
LIBS = $(libpq_pgport)

DISTFILES= Makefile Makefile.contrib \
	initialize_block.c 

dist:
	rm -rf initialize_block-${FD_VERSION} initialize_block-${FD_VERSION}.tar.gz
	mkdir initialize_block-${FD_VERSION}
	cp -p ${DISTFILES} initialize_block-${FD_VERSION}
	tar cfz initialize_block-${FD_VERSION}.tar.gz initialize_block-${FD_VERSION}
	rm -rf initialize_block-${FD_VERSION}

