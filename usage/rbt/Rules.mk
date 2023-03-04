RCFLAGS	:=-Wno-sign-compare
$(call subdirs_append_flags, $(d), CF,  $(RCFLAGS) )

SUBDIRS_$(d)	:=\
	0_starting \
	1_insert_and_find \
	2_loop_over \
	3_remove \
	4_tree_abstraction \
	dictionary \
	rng_test