COBJS_$(d)	:=\
	$(od)/insert.o\
	$(od)/find.o

FIND_OBJS_$(d)	:=$(od)/find.o
INSR_OBJS_$(d)	:=$(od)/insert.o

TGTS_$(d)	:=$(bd)/1_insert $(bd)/1_find

$(call append,TGT_BIN,$(d))
$(d): $(TGTS_$(d))

$(bd)/1_insert:	$(INSR_OBJS_$(d))
	$(CC_LINK)

$(bd)/1_find:	$(FIND_OBJS_$(d))
	$(CC_LINK)
