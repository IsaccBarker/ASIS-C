# The Makefile system here is heavily based on the article below.
# I'm not nearly this good at writting Makefiles ;)
#
# https://www.cmcrossroads.com/article/painless-non-recursive-make

SUBDIRS = src

include $(addsuffix /Makefile,$(SUBDIRS))

all:
	$(PROGRESS) "STATUS" "done"

clean:
	$(PROGRESS) "RM" "target"
	@rm -rf target
