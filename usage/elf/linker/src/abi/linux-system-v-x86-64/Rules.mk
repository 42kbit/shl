ASMOBJS_$(d)	:=			\
		$(od)/entry.o		\
		$(od)/syscalls.o

COBJS_$(d)	:=$(od)/centry.o

OBJS_$(d)	:=$(ASMOBJS_$(d)) $(COBJS_$(d))

CF_$(d)		:=					\
		$(CF_$(d))				\
		-I$(LINKER_ROOT)/include		\
		-I$(ROOT)				\
		-I$(d)/include				\
		-fno-stack-protector			\
		-mno-red-zone				\
		-mstackrealign				\
		-Wno-builtin-declaration-mismatch	\
		-fno-builtin

ASF_$(d)	:=					\
		$(ASF_$(d))				\
		-I$(d)/include

LF_$(TGT_LINKER):=					\
		$(LF_$(TGT_LINKER))			\
		-T $(d)/linux-system-v-x86-64.ld

$(d): $(TGT_LINKER)
$(TGT_LINKER):	$(OBJS_$(d)) $(d)/linux-system-v-x86-64.ld

SUBDIRS_$(d)	:=lib