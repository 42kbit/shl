ASMOBJS_$(d)	:=
COBJS_$(d)	:=			\
		$(od)/so_mem.o		\
		$(od)/so_mem_load.o	\
		$(od)/so_mem_rela.o

OBJS_$(d)	:=$(ASMOBJS_$(d)) $(COBJS_$(d))

CF_$(d)		:=					\
		$(CF_$(d))				\
		-I$(LINKER_ROOT)/include		\
		-I$(ROOT)				\
		-I$(LINKER_ABI_ROOT)/include		\
		-fno-stack-protector			\
		-mno-red-zone				\
		-mstackrealign				\
		-Wno-builtin-declaration-mismatch	\
		-fno-builtin

ASF_$(d)	:=					\
		$(ASF_$(d))				\
		-I$(d)/include

$(d): $(TGT_LINKER)
$(TGT_LINKER):	$(OBJS_$(d))