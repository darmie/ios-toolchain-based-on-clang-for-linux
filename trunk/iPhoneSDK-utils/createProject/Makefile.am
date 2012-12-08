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
	install -D -m 0755 createProject $(DESTDIR)/$(bindir)/ios-createProject
	sed -i "s#DATA_DIR#$(datadir)#g" $(DESTDIR)/$(bindir)/ios-createProject
	mkdir -p $(DESTDIR)/$(datadir)/iPhoneTemplates
	install -D -m 0644 iPhoneTemplates/*.template $(DESTDIR)/$(datadir)/iPhoneTemplates
