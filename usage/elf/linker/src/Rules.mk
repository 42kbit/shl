
ASMOBJS_$(d)	:=		\
		$(od)/entry.o	\
		$(od)/syscalls.o

COBJS_$(d)	:=		\
		$(od)/dlmain.o	\
		$(od)/centry.o	\
		$(od)/io.o

OBJS_$(d)	:=$(ASMOBJS_$(d)) $(COBJS_$(d))

TGTS_$(d)	:=$(bd)/linker

CF_$(d)		+=			\
		-fno-stack-protector	\
		-Wno-builtin-declaration-mismatch	\
		-fno-builtin

ASF_$(od)/entry.o	+=

LF_$(TGTS_$(d))	:=		\
		-nostdlib	\
		-shared		\
		-static-pie

$(call append,TGT_BIN,$(d))
$(d): $(TGTS_$(d))
$(TGTS_$(d)):	$(OBJS_$(d))
	$(L_LINK)