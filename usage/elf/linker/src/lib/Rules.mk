
ASMOBJS_$(d)	:=
COBJS_$(d)	:=		\
		$(od)/io.o	\
		$(od)/string.o	\
		$(od)/mem.o	\
		$(od)/getenv.o


OBJS_$(d)	:=$(ASMOBJS_$(d)) $(COBJS_$(d))

CF_$(d)		:=					\
		$(CF_$(d))				\
		-I$(LINKER_ROOT)/include		\
		-I$(LINKER_ABI_ROOT)/include		\
		-I$(ROOT)				\
		-fno-stack-protector			\
		-Wno-builtin-declaration-mismatch	\
		-mstackrealign				\
		-fno-builtin

$(d): $(TGT_LINKER)
$(TGT_LINKER):	$(OBJS_$(d))