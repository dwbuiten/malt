######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.1.0-dev                    #
#            DATE     : 02/2018                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
RONN=ronn
TARGETS=malt-passwd.1.bz2 malt-webview.1.bz2 malt-qt5.1.bz2 malt.1.bz2 malt-passwd.1.html malt-webview.1.html malt.1.html malt-qt5.1.html
RM=rm -f
BZIP2=bzip2

######################################################
all: $(TARGETS)

######################################################
clean:
	$(RM) $(TARGETS)

######################################################
%.1.bz2: %.ronn
	$(RONN) -r --pipe $< | $(BZIP2) > $@

######################################################
%.1.html: %.ronn
	$(RONN) -5 $<

######################################################
.PHONY: all clean
