MAINTAINERCLEANFILES = Makefile.in

EOLIAN_FLAGS = @DEPS_EOLIAN_FLAGS@ \
	       -I$(top_srcdir)/src/lib/elementary_combobox

include $(top_srcdir)/Makefile_Eolian_Helper.am

AM_CPPFLAGS = \
-I. \
-I$(top_builddir) \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib/elementary_combobox \
-I$(top_builddir)/src/bin/logger \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DLOCALE_DIR=\"@LOCALE_DIR@\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DEFL_EO_API_SUPPORT \
-DEFL_BETA_API_SUPPORT \
-DELM_INTERNAL_API_ARGESFSDFEFC \
-DCMB_BUILD \
@ELEMENTARY_CFLAGS@

noinst_LTLIBRARIES = libcmb.a

libcmb_a_SOURCES = \
elc_combobox.c

BUILT_SOURCES = \
elm_combobox.eo.c \
elm_combobox.eo.h

EXTRA_DIST = elm_combobox.eo \
elc_combobox.h \
elc_combobox_legacy.h \
elm_widget_combobox.h \
elm_priv.h \
Elementary_Combobox.h
