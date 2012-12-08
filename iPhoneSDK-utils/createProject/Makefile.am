##############################
# The main binary
##############################

AM_CPPFLAGS =						\
	-DBINDIR=\"$(bindir)\"			 	\
	-DDATADIR=\"$(datadir)\"			\
	-DPREFIX=\""$(prefix)"\" 			\
	-DSYSCONFDIR=\""$(sysconfdir)"\" 		\
	-DLIBDIR=\""$(libdir)"\" 			\
	$(NULL)

install-data-hook:
	mkdir -p $(DESTDIR)/$(datadir)
	sed -i "s#DATA_DIR#$(datadir)#g" createProject
	install -m 0755 createProject $(DESTDIR)/$(bindir)/ios-createProject
	cp -r iPhoneTemplates $(DESTDIR)/$(datadir)/
