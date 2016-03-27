export VERSION      := 0
export SUBVERSION   := 8
export MINORVERSION := 2-SNAPSHOT

all: nethogs decpcap_test test
	$(MAKE) -C src -f MakeApp.mk $@
	$(MAKE) -C src -f MakeLib.mk $@

.PHONY:
tgz: clean
	cd .. ; tar czvf nethogs-$(VERSION).$(SUBVERSION).$(MINORVERSION).tar.gz --exclude-vcs nethogs/*

check:
	$(MAKE) -C src -f MakeApp.mk $@

install:
	$(MAKE) -C src -f MakeApp.mk $@
	$(MAKE) -C src -f MakeLib.mk $@

install_dev:
	$(MAKE) -C src -f MakeLib.mk $@

uninstall:
	$(MAKE) -C src -f MakeApp.mk $@
	$(MAKE) -C src -f MakeLib.mk $@

nethogs:
	$(MAKE) -C src -f MakeApp.mk $@

decpcap_test:
	$(MAKE) -C src -f MakeApp.mk $@

test:
	$(MAKE) -C src -f MakeApp.mk $@

clean:
	$(MAKE) -C src -f MakeApp.mk $@
	$(MAKE) -C src -f MakeLib.mk $@

format:
	clang-format -i src/*.c src/*.cpp src/*.h
