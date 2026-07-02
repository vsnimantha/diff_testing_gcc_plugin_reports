# GCC plugin for unencrypted function pointer discovery.
# Copyright (C) 2017 Free Software Foundation, Inc.
# Contributed by Aldy Hernandez <aldyh@redhat.com>.

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or (at
# your option) any later version.

# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


# Not any old GCC will work, as GCC's plugin API is a moving target.
# This plugin has only been tested on GCC 6 and 7.

FUNCP_SRC=funcp-encrypt.cc
SRCS=$(FUNCP_SRC)

GCCPLUGINS_DIR:= $(shell $(CC) -print-file-name=plugin)
CXXFLAGS+= -I$(GCCPLUGINS_DIR)/include -fPIC -fno-rtti -g -shared -std=c++11 -Wall

FUNCP_PLUGIN=funcp-encrypt.so
FUNCP_TESTCASE=test-funcp-encrypt.c
TESTCASES=$(FUNCP_TESTCASE)

all: $(FUNCP_PLUGIN)

$(FUNCP_PLUGIN): $(FUNCP_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(FUNCP_PLUGIN) *.c.* *.s *.o nohup.out

check: $(FUNCP_PLUGIN)
	./check.sh $(CC) ./$(FUNCP_PLUGIN) $(FUNCP_TESTCASE)
	echo "Tests OK.  Number of warnings are as expected."

wc:	# Cause deep down inside, we all suffer from OCD.
	wc $(SRCS) Makefile check.sh $(TESTCASES)

dist:	clean
	(cd ..;	tar --exclude=.git* -cvzf /tmp/funcp-encrypt.tar.gz funcp-encrypt)
	scp /tmp/funcp-encrypt.tar.gz people.redhat.com:public_html/
	rm /tmp/funcp-encrypt.tar.gz

.PHONY: all clean check wc dist
