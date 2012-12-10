SUBDIRS = templates

AM_CPPFLAGS =						\
	-DBINDIR=\"$(bindir)\"			 	\
	-DDATADIR=\"$(datadir)\"			\
	-DPREFIX=\""$(prefix)"\" 			\
	-DSYSCONFDIR=\""$(sysconfdir)"\" 		\
	-DLIBDIR=\""$(libdir)"\" 			\
	$(NULL)

mkprog_in_files = ios-createProject.sh.in
mkprog_file = $(mkprog_in_files:%.sh.in=%)
$(mkprog_file): $(mkprog_in_files)
	sed -e "s|\@TEMPLATES_DIR\@|$(TEMPLATES_DIR)|" $<> $@

bin_SCRIPTS = $(mkprog_file)

EXTRA_DIST = $(mkprog_in_files)

CLEANFILES = $(mkprog_file)
DISTCLEANFILES = $(mkprog_file)
