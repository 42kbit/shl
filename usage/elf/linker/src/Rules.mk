
ASMOBJS_$(d)	:=		\
		$(od)/entry.o	\
		$(od)/syscalls.o

COBJS_$(d)	:=		\
		$(od)/dlmain.o	\
		$(od)/centry.o

OBJS_$(d)	:=$(ASMOBJS_$(d)) $(COBJS_$(d))

TGTS_$(d)	:=$(bd)/linker
TGT_LINKER	:=$(TGTS_$(d))

CF_$(d)		:=			\
		$(CF_$(d))		\
		-I$(d)/include		\
		-I$(ROOT)		\
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

SUBDIRS_$(d)	:=lib