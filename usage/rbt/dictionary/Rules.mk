COBJS_$(d)	:=$(od)/main.o
OBJS_$(d)	:=$(COBJS_$(d))
TGTS_$(d)	:=$(bd)/dict

CF_$(d)		:=-O2

$(call append,TGT_BIN,$(d))
$(d): $(TGTS_$(d))
$(TGTS_$(d)):	$(OBJS_$(d))
	$(L_LINK)

