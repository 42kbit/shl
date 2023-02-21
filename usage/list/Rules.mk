COBJS_$(d)	:=$(od)/list_usage_unsafe.o

OBJS_$(d)	:=$(COBJS_$(d))
TGTS_$(d)	:=$(bd)/list_usage_unsafe

CF_$(d)		:=-O2

$(call append,TGT_BIN,$(d))
$(d): $(TGTS_$(d))
$(TGTS_$(d)):	$(OBJS_$(d))
	$(L_LINK)





COBJS_$(d)	:=$(od)/list_usage_safe.o

OBJS_$(d)	:=$(COBJS_$(d))
TGTS_$(d)	:=$(bd)/list_usage_safe

CF_$(d)		:=-O2

$(call append,TGT_BIN,$(d))
$(d): $(TGTS_$(d))
$(TGTS_$(d)):	$(OBJS_$(d))
	$(L_LINK)