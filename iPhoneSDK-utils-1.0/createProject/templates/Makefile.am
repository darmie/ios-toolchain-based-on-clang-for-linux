TEMPLATES = Application \
	    Command_line_utility \
	    Example_Player	\
	    Example_UICatalog	\
	    Notification_Center_widget

TEMPLATE_FILES = $(TEMPLATES:%=%.template)

templatedir = $(TEMPLATES_DIR)
template_DATA = $(TEMPLATE_FILES)
%.template:%
	@cd $(abs_srcdir)/$^; tar zcf $(abs_builddir)/$@ *
	@echo $@ genarated.

EXTRA_DIST = $(TEMPLATES)
CLEANFILES = $(TEMPLATE_FILES)
DISTCLEANFILES = $(TEMPLATE_FILES)
