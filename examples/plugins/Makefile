# Qucs example plugin Makefile
# (c) 2021 Felix Salfelder
# GPLv3+

QUCS_CONF = $(shell which qucs-conf)

ifneq ($(QUCS_CONF),)
 QUCS_CXX      = $(shell $(QUCS_CONF) --cxx)
 QUCS_CPPFLAGS = $(shell $(QUCS_CONF) --cppflags)
 QT_CPPFLAGS = $(shell $(QUCS_CONF) --qt-cppflags)
 QUCS_LDFLAGS  = $(shell $(QUCS_CONF) --ldflags)
 QUCS_CXXFLAGS = $(shell $(QUCS_CONF) --cxxflags)
 HAVE_QUCS=yes
else
 HAVE_QUCS=no
 $(error no qucs-conf, this will not work)
endif

include MakeList

all: ${PLUGINS:%=%.so} ${PLUGINS_QT:%=%.so}

${PLUGINS:%=%.so}: %.so: %.cc
	${QUCS_CXX} ${QUCS_CPPFLAGS} ${CPPFLAGS} ${QUCS_CXXFLAGS} -fPIC -shared $< -o $@

${PLUGINS_QT:%=%.so}: %.so: %.cc
	${QUCS_CXX} ${QT_CPPFLAGS} ${QUCS_CPPFLAGS} ${CPPFLAGS} ${QUCS_CXXFLAGS} -fPIC -shared $< -o $@
