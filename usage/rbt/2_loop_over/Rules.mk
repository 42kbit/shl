COBJS_$(d)	:=$(od)/loop.o

OBJS_$(d)	:=$(COBJS_$(d))
TGTS_$(d)	:=$(bd)/2_loop_over

$(call append,TGT_BIN,$(d))
$(d): $(TGTS_$(d))
$(TGTS_$(d)):	$(OBJS_$(d))
	$(CC_LINK)

