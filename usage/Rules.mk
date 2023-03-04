# Following code adds for each directory following c and asm flags.
RCFLAGS = -Wno-unused-parameter
RASFLAGS =

$(call subdirs_append_flags, $(d), CF,  $(RCFLAGS) )
$(call subdirs_append_flags, $(d), ASF, $(RASFLAGS))

SUBDIRS_$(d)	:=rbt list elf