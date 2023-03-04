
ASMOBJS_$(d)	:=$(od)/entry.o
OBJS_$(d)	:=$(ASMOBJS_$(d))

TGTS_$(d)	:=$(bd)/linker

LINKER_BINARY	:=$(abspath $(bd)/../src/linker)

ASMF_$(od)/entry.o	:=-nostdlib

LF_$(TGTS_$(d))	:=-nostdlib -dynamic-linker /lib64/ld-linux-x86-64.so.2 

$(call append,TGT_BIN,$(d))
$(d): $(TGTS_$(d))
$(TGTS_$(d)):	$(OBJS_$(d))
	$(L_LINK)