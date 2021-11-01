AM_CPPFLAGS = -I$(top_srcdir)/src

sublibdir = $(libdir)/libyogrt
sublib_LTLIBRARIES = libyogrt-lsf.la
libyogrt_lsf_la_SOURCES = internal.c
libyogrt_lsf_la_LIBADD = $(FLUX_LIBADD)
libyogrt_lsf_la_CPPFLAGS = $(AM_CPPFLAGS) $(FLUX_CPPFLAGS)
libyogrt_lsf_la_LDFLAGS = $(FLUX_LDFLAGS) \
	-no-undefined \
	-export-symbols-regex "^internal_.*" \
	-version-info $(LIBYOGRT_LT_CURRENT):$(LIBYOGRT_LT_REVISION):$(LIBYOGRT_LT_AGE)
