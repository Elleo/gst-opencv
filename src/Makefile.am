SUBDIRS = basicfilters edgedetect faceblur facedetect pyramidsegment templatematch textwrite

# plugindir is set in configure

plugin_LTLIBRARIES = libgstopencv.la

# sources used to compile this plug-in
libgstopencv_la_SOURCES = gstopencv.c gstopencvbasetrans.c gstopencvutils.c

# flags used to compile this facedetect
# add other _CFLAGS and _LIBS as needed
libgstopencv_la_CFLAGS = $(GST_CFLAGS) $(GST_BASE_CFLAGS) $(OPENCV_CFLAGS) \
								 -I${top_srcdir}/src \
								 -I${top_srcdir}/src/basicfilters \
								 -I${top_srcdir}/src/edgedetect \
								 -I${top_srcdir}/src/faceblur \
								 -I${top_srcdir}/src/facedetect \
								 -I${top_srcdir}/src/pyramidsegment \
								 -I${top_srcdir}/src/templatematch \
								 -I${top_srcdir}/src/textwrite 

libgstopencv_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(OPENCV_LIBS) \
								 $(top_builddir)/src/basicfilters/libgstbasicfilters.la \
								 $(top_builddir)/src/edgedetect/libgstedgedetect.la \
								 $(top_builddir)/src/faceblur/libgstfaceblur.la \
								 $(top_builddir)/src/facedetect/libgstfacedetect.la \
								 $(top_builddir)/src/pyramidsegment/libgstpyramidsegment.la \
								 $(top_builddir)/src/templatematch/libgsttemplatematch.la \
								 $(top_builddir)/src/textwrite/libgsttextwrite.la

libgstopencv_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

libgstopencv_la_DEPENDENCIES = \
								 $(top_builddir)/src/basicfilters/libgstbasicfilters.la \
								 $(top_builddir)/src/edgedetect/libgstedgedetect.la \
								 $(top_builddir)/src/faceblur/libgstfaceblur.la \
								 $(top_builddir)/src/facedetect/libgstfacedetect.la \
								 $(top_builddir)/src/pyramidsegment/libgstpyramidsegment.la \
								 $(top_builddir)/src/templatematch/libgsttemplatematch.la \
								 $(top_builddir)/src/textwrite/libgsttextwrite.la

# headers we need but don't want installed
noinst_HEADERS = gstopencvbasetrans.h gstopencvutils.h
