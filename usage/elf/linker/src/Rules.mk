
ASMOBJS_$(d)	:=
COBJS_$(d)	:=

OBJS_$(d)	:=$(ASMOBJS_$(d)) $(COBJS_$(d))

TGTS_$(d)	:=$(bd)/linker

TGT_LINKER	:=$(TGTS_$(d))
LINKER_ROOT	:=$(d)

CF_$(d)		:=					\
		$(CF_$(d))				\
		-I$(d)/include				\
		-I$(ROOT)				\
		-fno-stack-protector			\
		-Wno-builtin-declaration-mismatch	\
		-fno-builtin

ASF_$(od)/entry.o	+=

LF_$(TGTS_$(d))	:=			\
		-nostdlib		\
		--no-dynamic-linker	\
		-pie

$(call append,TGT_BIN,$(d))
$(d): $(TGTS_$(d))
$(TGTS_$(d)):	$(OBJS_$(d))
	$(L_LINK)

SUBDIRS_$(d)	:=lib abi