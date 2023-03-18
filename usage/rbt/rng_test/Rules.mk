COBJS_$(d)	:=\
	$(od)/order.o\
	$(od)/rbt.o

ORD_OBJS_$(d)	:=$(od)/order.o
RBT_OBJS_$(d)	:=$(od)/rbt.o

TGTS_$(d)	:=$(bd)/order $(bd)/rbt

$(call append,TGT_BIN,$(d))
$(d): $(TGTS_$(d))

$(bd)/rbt:	$(ORD_OBJS_$(d))
	$(CC_LINK)

$(bd)/order:	$(RBT_OBJS_$(d))
	$(CC_LINK)
