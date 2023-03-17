
ASMOBJS_$(d)	:=
COBJS_$(d)	:=$(od)/io.o

OBJS_$(d)	:=$(ASMOBJS_$(d)) $(COBJS_$(d))

CF_$(d)		:=			\
		$(CF_$(d))		\
		-I$(d)/../include	\
		-I$(ROOT)		\
		-fno-stack-protector	\
		-Wno-builtin-declaration-mismatch	\
		-fno-builtin

ASF_$(od)/entry.o	+=

$(d): $(TGT_LINKER)
$(TGT_LINKER):	$(OBJS_$(d))