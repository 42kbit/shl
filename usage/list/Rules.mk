#COBJS_$(d)	:=$(od)/list_usage_unsafe.o
#
#OBJS_$(d)	:=$(COBJS_$(d))
#TGTS_$(d)	:=$(bd)/list_usage_unsafe
#
#$(call append,TGT_BIN,$(d))
#$(d): $(TGTS_$(d))
#$(TGTS_$(d)):	$(OBJS_$(d))
#	$(L_LINK)
#
#
#
#
#
#COBJS_$(d)	:=$(od)/list_usage_safe.o
#
#OBJS_$(d)	:=$(COBJS_$(d))
#TGTS_$(d)	:=$(bd)/list_usage_safe
#
#$(call append,TGT_BIN,$(d))
#$(d): $(TGTS_$(d))
#$(TGTS_$(d)):	$(OBJS_$(d))
#	$(L_LINK)

COBJS_UNSAFE_$(d)	:=$(od)/list_usage_unsafe.o
COBJS_SAFE_$(d)		:=$(od)/list_usage_safe.o

COBJS_$(d)	:=			\
		$(COBJS_UNSAFE_$(d))	\
		$(COBJS_SAFE_$(d))

OBJS_UNSAFE_$(d)	:=$(COBJS_UNSAFE_$(d))
OBJS_SAFE_$(d)		:=$(COBJS_SAFE_$(d))

OBJS_$(d)	:=			\
		$(OBJS_UNSAFE_$(d))	\
		$(OBJS_SAFE_$(d))

TGT_UNSAFE_$(d)	:=$(bd)/list_usage_unsafe
TGT_SAFE_$(d)	:=$(bd)/list_usage_safe

TGTS_$(d)	:=$(TGT_UNSAFE_$(d)) $(TGT_SAFE_$(d))

$(call append,TGT_BIN,$(d))
$(d): $(TGT_UNSAFE_$(d))
$(TGT_UNSAFE_$(d)): $(TGT_SAFE_$(d)) $(OBJS_UNSAFE_$(d))
	$(L_LINK)

$(d): $(TGT_SAFE_$(d))
$(TGT_SAFE_$(d)): $(OBJS_SAFE_$(d))
	$(L_LINK)