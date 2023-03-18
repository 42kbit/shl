COBJS_$(d)	:=$(od)/elf_usage.o

OBJS_$(d)	:=$(COBJS_$(d))
TGTS_$(d)	:=$(bd)/elf_usage

CF_$(d)		:=-O2

# Remove multichar warning, because multichar is used to determine endiannes at compile time
CF_$(od)/elf_usage.o	:=-Wno-multichar 

$(call append,TGT_BIN,$(d))
$(d): $(TGTS_$(d))
$(TGTS_$(d)):	$(OBJS_$(d))
	$(CC_LINK)
