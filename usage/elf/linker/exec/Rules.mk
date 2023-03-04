
COBJS_$(d)	:=$(od)/main.o

OBJS_$(d)	:=$(COBJS_$(d))
TGTS_$(d)	:=$(bd)/main

LINKER_BINARY	:=$(abspath $(bd)/../src/linker)

CF_$(od)/main.o	:=-D__INTERP_PATH=$(LINKER_BINARY) -nostdlib

LF_$(TGTS_$(d))	:=-nostdlib

$(call append,TGT_BIN,$(d))
$(d): $(TGTS_$(d))
$(TGTS_$(d)):	$(OBJS_$(d))
	$(L_LINK)