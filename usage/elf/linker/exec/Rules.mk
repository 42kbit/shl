
COBJS_$(d)	:=$(od)/main.o

OBJS_$(d)	:=$(COBJS_$(d))
TGTS_$(d)	:=$(bd)/main

LINKER_BINARY	:=$(abspath $(bd)/../src/linker)

CF_$(od)/main.o	:=

LF_$(TGTS_$(d))	:=-dynamic-linker,$(LINKER_BINARY) -nostdlib

PURE_LD	= ld \
	$(LF_ALL) \
	$(LF_$@) \
	-o $@ $(filter %.o,$^)

$(call append,TGT_BIN,$(d))
$(d): $(TGTS_$(d))
$(TGTS_$(d)):	$(OBJS_$(d))
	$(L_LINK)
	patchelf --set-interpreter $(LINKER_BINARY) $@