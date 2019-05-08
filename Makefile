# Copyright 2019 Grama Nicolae

.PHONY: gitignore test cstest clean beauty cpplint pack
.SILENT: beauty csbeauty pack clean gitignore cstest

CC=gcc
CFLAGS=-fPIC -Wall -Wextra --std=c++14
COMPILE=$(CC) $(CFLAGS)

build: libaegraph.so

libaegraph.so: aegraph.cpp
	$(COMPILE) -shared -o $@ $<

clean:
	rm -f libaegraph.so

# Automatic coding style, personal style (4 spaces for tabs)
beauty: 
	clang-format -i -style=file *.cpp *.h

# Automatic coding style, using google standard.
# 4 spaces for .cpp, 2 spaces for .h
csbeauty:
	clang-format -i -style=google *.h
	clang-format -i -style=file *.cpp

# Updates the gitignore rules
gitignore:
	@echo libaegraph.so > .gitignore ||:
	@echo ".vscode*" >> .gitignore ||:
	@echo "README" >> .gitignore ||:
	@echo "archive.zip" >> .gitignore ||:
	echo "Updated .gitignore"

# Test the homework
test:
	./run_all.sh

# Test just the coding style
cstest:
	./cpplint.py *.cpp *.h

# Creates the archive with the homework
pack:
	if [ ! -f README ]; then cp Readme.md README; fi
	zip -FSr archive.zip README aegraph.cpp
	rm README