SUBDIRS = lib \
	module
.PHONY: subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)

$(SUBDIRS):
	make -C $@

.PHONY: build
