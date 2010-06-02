/*
 * GStreamer
 * Copyright (C) 2010 Thiago Santos <thiago.sousa.santos@collabora.co.uk>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * As cvdilate_erode and cverode are all the same, except for the transform function,
 * we hope this base class should keep maintenance easier.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>

#include "gstcvdilateerode.h"

/*
GST_DEBUG_CATEGORY_STATIC (gst_cv_dilate_erode_debug);
#define GST_CAT_DEFAULT gst_cv_dilate_erode_debug
*/

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("video/x-raw-rgb, depth=(int)24, bpp=(int)24;"
        "video/x-raw-gray")
    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("video/x-raw-rgb, depth=(int)24, bpp=(int)24;"
        "video/x-raw-gray")
    );

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};
enum
{
  PROP_0,
  PROP_ITERATIONS,
};

#define DEFAULT_ITERATIONS 1

static GstElementClass *parent_class = NULL;

static void gst_cv_dilate_erode_base_init (gpointer gclass);
static void gst_cv_dilate_erode_class_init (GstCvDilateErodeClass * klass);
static void gst_cv_dilate_erode_init (GstCvDilateErode * filter,
    GstCvDilateErodeClass * gclass);

static void gst_cv_dilate_erode_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_cv_dilate_erode_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

GType
gst_cv_dilate_erode_get_type (void)
{
  static volatile gsize opencv_dilate_erode_type = 0;

  if (g_once_init_enter (&opencv_dilate_erode_type)) {
    GType _type;
    static const GTypeInfo opencv_dilate_erode_info = {
      sizeof (GstCvDilateErodeClass),
      (GBaseInitFunc) gst_cv_dilate_erode_base_init,
      NULL,
      (GClassInitFunc) gst_cv_dilate_erode_class_init,
      NULL,
      NULL,
      sizeof (GstCvDilateErode),
      0,
      (GInstanceInitFunc) gst_cv_dilate_erode_init,
    };

    _type = g_type_register_static (GST_TYPE_OPENCV_BASE_TRANSFORM,
        "GstCvDilateErode", &opencv_dilate_erode_info,
        G_TYPE_FLAG_ABSTRACT);
/*
    GST_DEBUG_CATEGORY_INIT (gst_cv_dilate_erode_debug, "cvdilateerode", 0,
        "cvdilateerode");
*/
    g_once_init_leave (&opencv_dilate_erode_type, _type);
  }
  return opencv_dilate_erode_type;
}

/* Clean up */
static void
gst_cv_dilate_erode_finalize (GObject * obj)
{
  G_OBJECT_CLASS (parent_class)->finalize (obj);
}


/* GObject vmethod implementations */

static void
gst_cv_dilate_erode_base_init (gpointer gclass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (gclass);

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&src_factory));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&sink_factory));
}

/* initialize the cvdilate_erode's class */
static void
gst_cv_dilate_erode_class_init (GstCvDilateErodeClass * klass)
{
  GObjectClass *gobject_class;
  GstOpencvBaseTransformClass *gstopencvbasefilter_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
  gstopencvbasefilter_class = (GstOpencvBaseTransformClass *) klass;

  parent_class = g_type_class_peek_parent (klass);

  gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_cv_dilate_erode_finalize);
  gobject_class->set_property = gst_cv_dilate_erode_set_property;
  gobject_class->get_property = gst_cv_dilate_erode_get_property;

  g_object_class_install_property (gobject_class, PROP_ITERATIONS,
      g_param_spec_int ("iterations", "iterations",
          "Number of iterations to run the algorithm", 1, G_MAXINT,
          DEFAULT_ITERATIONS, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_cv_dilate_erode_init (GstCvDilateErode * filter,
    GstCvDilateErodeClass * gclass)
{
  filter->iterations = DEFAULT_ITERATIONS;
  gst_base_transform_set_in_place (GST_BASE_TRANSFORM (filter), TRUE);
}

static void
gst_cv_dilate_erode_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstCvDilateErode *filter = GST_CV_DILATE_ERODE (object);

  switch (prop_id) {
    case PROP_ITERATIONS:
      filter->iterations = g_value_get_int (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_cv_dilate_erode_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstCvDilateErode *filter = GST_CV_DILATE_ERODE (object);

  switch (prop_id) {
    case PROP_ITERATIONS:
      g_value_set_int (value, filter->iterations);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}
