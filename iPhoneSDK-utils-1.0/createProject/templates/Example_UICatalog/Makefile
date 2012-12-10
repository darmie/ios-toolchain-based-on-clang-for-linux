IPHONE_IP:=
PROJECTNAME:=UICatalog
APPFOLDER:=$(PROJECTNAME).app
INSTALLFOLDER:=$(PROJECTNAME).app

CC:=ios-clang
CPP:=ios-clang++

CFLAGS += -include *.pch
CFLAGS += -objc-arc
CFLAGS += -fblocks
CFLAGS += -g0 -O2
CFLAGS += -I"$(SRCDIR)" -I"$(SRCDIR1)" -I"$(SRCDIR2)"

CPPFLAGS += -include *.pch
CPPFLAGS += -objc-arc
CPPFLAGS += -fblocks
CPPFLAGS += -g0 -O2
CPPFLAGS += -I"$(SRCDIR)" -I"$(SRCDIR1)" -I"$(SRCDIR2)"

LDFLAGS += -framework CoreFoundation 
LDFLAGS += -framework Foundation 
LDFLAGS += -framework UIKit 
LDFLAGS += -framework CoreGraphics

SRCDIR=.
SRCDIR1=./Picker
SRCDIR2=./TableCells

OBJS+=$(patsubst %.m,%.o,$(wildcard $(SRCDIR)/*.m))
OBJS+=$(patsubst %.c,%.o,$(wildcard $(SRCDIR)/*.c))
OBJS+=$(patsubst %.cpp,%.o,$(wildcard $(SRCDIR)/*.cpp))
OBJS+=$(patsubst %.m,%.o,$(wildcard $(SRCDIR1)/*.m))
OBJS+=$(patsubst %.c,%.o,$(wildcard $(SRCDIR1)/*.c))
OBJS+=$(patsubst %.cpp,%.o,$(wildcard $(SRCDIR1)/*.cpp))
OBJS+=$(patsubst %.m,%.o,$(wildcard $(SRCDIR2)/*.m))
OBJS+=$(patsubst %.c,%.o,$(wildcard $(SRCDIR2)/*.c))
OBJS+=$(patsubst %.cpp,%.o,$(wildcard $(SRCDIR2)/*.cpp))

INFOPLIST:=$(wildcard *Info.plist)

RESOURCES+=$(wildcard ./Images/*)
RESOURCES+=$(wildcard ./Resources/*)
RESOURCES+=$(wildcard ./Localizations/*)


all:	$(PROJECTNAME)

$(PROJECTNAME):	$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(filter %.o,$^) -o $@ 

%.o:	%.m
	$(CC) -c $(CFLAGS) $< -o $@

%.o:	%.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o:	%.cpp
	$(CPP) -c $(CPPFLAGS) $< -o $@

dist:	$(PROJECTNAME)
	mkdir -p $(APPFOLDER)
ifneq ($(RESOURCES),)
	cp -r $(RESOURCES) $(APPFOLDER)
endif
	cp $(INFOPLIST) $(APPFOLDER)/Info.plist
	ldid -S $(PROJECTNAME)
	cp $(PROJECTNAME) $(APPFOLDER)

langs:
	ios-genLocalization

install: dist
	ping -t 3 -c 1 $(IPHONE_IP)
	ssh root@$(IPHONE_IP) 'rm -fr /Applications/$(INSTALLFOLDER)'
	scp -r $(APPFOLDER) root@$(IPHONE_IP):/Applications/$(INSTALLFOLDER)
	@echo "Application $(INSTALLFOLDER) installed"
	ssh mobile@$(IPHONE_IP) 'uicache'

uninstall:
	ping -t 3 -c 1 $(IPHONE_IP)
	ssh root@$(IPHONE_IP) 'rm -fr /Applications/$(INSTALLFOLDER)'
	@echo "Application $(INSTALLFOLDER) uninstalled"

clean:
	@rm -f $(SRCDIR)/*.o  $(SRCDIR1)/*.o  $(SRCDIR2)/*.o
	@rm -rf $(APPFOLDER)
	@rm -f $(PROJECTNAME)

.PHONY: all dist install uninstall clean
