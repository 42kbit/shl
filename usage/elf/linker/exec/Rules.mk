
COBJS_EXEC_$(d)	:=$(od)/main.o
COBJS_LTEST_$(d):=$(od)/libtest.o

COBJS_$(d)	:=			\
		$(COBJS_EXEC_$(d))	\
		$(COBJS_LTEST_$(d))

OBJS_EXEC_$(d)	:=$(COBJS_EXEC_$(d))
OBJS_LTEST_$(d)	:=$(COBJS_LTEST_$(d))

OBJS_$(d)	:=			\
		$(OBJS_EXEC_$(d))	\
		$(OBJS_LTEST_$(d))

TGT_EXEC_$(d)	:=$(bd)/main
TGT_LTEST_$(d)	:=$(bd)/libtest.so

TGTS_$(d)	:=$(TGT_EXEC_$(d)) $(TGT_LTEST_$(d))

LINKER_BINARY	:=$(abspath $(bd)/../src/linker)

CF_$(od)/main.o		:=
CF_$(od)/libtest.o	:=-fPIC

LF_$(TGT_EXEC_$(d))	:=				\
		-dynamic-linker,$(LINKER_BINARY)	\
		-nostdlib 				\
		-L$(bd)					\
		-ltest

LF_$(TGT_LTEST_$(d))	:=				\
		-nostdlib 				\
		-shared

PURE_LD	= ld \
	$(LF_ALL) \
	$(LF_$@) \
	-o $@ $(filter %.o,$^)

$(call append,TGT_BIN,$(d))
$(d): $(TGT_EXEC_$(d))
$(TGT_EXEC_$(d)): $(TGT_LTEST_$(d)) $(OBJS_EXEC_$(d))
	$(CC_LINK)
	patchelf --set-interpreter $(LINKER_BINARY) $@

$(d): $(TGT_LTEST_$(d))
$(TGT_LTEST_$(d)): $(OBJS_LTEST_$(d))
	$(CC_LINK)