
ASMOBJS_$(d)	:=
COBJS_$(d)	:=$(od)/io.o

OBJS_$(d)	:=$(ASMOBJS_$(d)) $(COBJS_$(d))

CF_$(d)		:=					\
		$(CF_$(d))				\
		-I$(LINKER_ROOT)/include		\
		-I$(ROOT)				\
		-fno-stack-protector			\
		-Wno-builtin-declaration-mismatch	\
		-fno-builtin

$(d): $(TGT_LINKER)
$(TGT_LINKER):	$(OBJS_$(d))