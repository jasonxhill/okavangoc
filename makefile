cc:=gcc -std=c11 -O0 -g3 -Wall -c -fmessage-length=0 -o
link:=gcc
srcdir:=src
tstdir:=test
outputdir:=debug
target:=okavangoc
srcmain:=main.o
srcfiles=$(wildcard $(srcdir)/*.c)
tstfiles=$(wildcard $(tstdir)/*.c)

uname:=$(shell uname -s)

ifeq ($(uname),Darwin)
  targetExtension:=.app
endif

targetExtension?=.exe

targetPath:=$(outputdir)/$(target)$(targetExtension)
testTargetPath:=$(outputdir)/$(target)-test$(target-extension)$(targetExtension)

red=\033[0;31m
blue=\033[0;34m
green=\033[0;32m
noColor=\033[0m

build: compile
	rm -f $(testTargetPath)
	$(eval srcObjects=$(wildcard $(outputdir)/$(srcdir)/*.o))
	$(link) -o $(targetPath) $(srcObjects)
	
	$(eval srcObjects=$(filter-out $(outputdir)/$(srcdir)/$(srcmain), $(srcObjects)))
	$(eval testObjects=$(wildcard $(outputdir)/$(tstdir)/*.o))
	$(link) -o $(testTargetPath) $(srcObjects) $(testObjects)

compile: makedirs $(patsubst %.c,$(outputdir)/%.o,$(srcfiles) $(tstfiles))

$(outputdir)/%.o: $(patsubst %.o,%.c,%.o)
	$(cc) $@ $^

makedirs: $(patsubst %,_MAKE_DIR_%,$(sort $(dir $(wildcard $(srcdir)/*/) $(wildcard $(tstdir)/*/))))

_MAKE_DIR_%:
	mkdir -p $(patsubst _MAKE_DIR_%, $(outputdir)/%, $@)

.PHONY: clean
clean:
	rm -rf $(outputdir)
	
test: build
	
	@echo
	@echo ======================================================================
	@echo " Starting Tests"
	@echo ----------------------------------------------------------------------
	@$(testTargetPath)
	@echo
	@echo ----------------------------------------------------------------------
	@printf "$(green) Success$(noColor)\n"
	@echo ======================================================================
	