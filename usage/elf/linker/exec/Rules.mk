
COBJS_$(d)	:=$(od)/main.o

OBJS_$(d)	:=$(COBJS_$(d))
TGTS_$(d)	:=$(bd)/main

LINKER_BINARY	:=$(abspath $(bd)/../src/linker)

# IDK how to set up .interp right, so take this piece of shit you dumb static linker
CF_$(od)/main.o	:=  -D__INTERP_PATH=$(LINKER_BINARY)

LF_$(TGTS_$(d))	:=-nostdlib

PURE_LD	= ld \
	$(LF_ALL) \
	$(LF_$@) \
	-o $@ $(filter %.o,$^)

$(call append,TGT_BIN,$(d))
$(d): $(TGTS_$(d))
$(TGTS_$(d)):	$(OBJS_$(d))
	$(PURE_LD)