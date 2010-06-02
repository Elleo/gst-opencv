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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>

#include "gstcvsmooth.h"

GST_DEBUG_CATEGORY_STATIC (gst_cv_smooth_debug);
#define GST_CAT_DEFAULT gst_cv_smooth_debug

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("video/x-raw-rgb, depth=(int)24, bpp=(int)24")
    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("video/x-raw-rgb, depth=(int)24, bpp=(int)24")
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
  PROP_SMOOTH_TYPE,
  PROP_PARAM1,
  PROP_PARAM2,
  PROP_PARAM3,
  PROP_PARAM4
};

#define GST_TYPE_CV_SMOOTH_TYPE (gst_cv_smooth_type_get_type ())
static GType
gst_cv_smooth_type_get_type (void)
{
  static GType cv_smooth_type_type = 0;

  static const GEnumValue smooth_types[] = {
    {CV_BLUR_NO_SCALE, "CV Blur No Scale", "blur-no-scale"},
    {CV_BLUR, "CV Blur", "blur"},
    {CV_GAUSSIAN, "CV Gaussian", "gaussian"},
    {CV_MEDIAN, "CV Median", "median"},
    {CV_BILATERAL, "CV Bilateral", "bilateral"},
    {0, NULL, NULL},
  };

  if (!cv_smooth_type_type) {
    cv_smooth_type_type =
        g_enum_register_static ("GstCvSmoothTypeType", smooth_types);
  }
  return cv_smooth_type_type;
}

#define DEFAULT_CV_SMOOTH_TYPE CV_GAUSSIAN
#define DEFAULT_PARAM1 3
#define DEFAULT_PARAM2 0.0
#define DEFAULT_PARAM3 0.0
#define DEFAULT_PARAM4 0.0

GST_BOILERPLATE (GstCvSmooth, gst_cv_smooth, GstOpencvBaseTransform,
    GST_TYPE_OPENCV_BASE_TRANSFORM);

static void gst_cv_smooth_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_cv_smooth_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static GstFlowReturn gst_cv_smooth_transform_ip (GstOpencvBaseTransform * filter,
    GstBuffer * buf, IplImage * img);
static GstFlowReturn gst_cv_smooth_transform (GstOpencvBaseTransform * filter,
    GstBuffer * buf, IplImage * img, GstBuffer * outbuf, IplImage * outimg);

/* Clean up */
static void
gst_cv_smooth_finalize (GObject * obj)
{
  G_OBJECT_CLASS (parent_class)->finalize (obj);
}


/* GObject vmethod implementations */

static void
gst_cv_smooth_base_init (gpointer gclass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (gclass);

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&src_factory));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&sink_factory));

  gst_element_class_set_details_simple (element_class,
      "cvsmooth",
      "Transform/Effect/Video",
      "Applies cvSmooth OpenCV function to the image",
      "Thiago Santos<thiago.sousa.santos@collabora.co.uk>");
}

/* initialize the cvsmooth's class */
static void
gst_cv_smooth_class_init (GstCvSmoothClass * klass)
{
  GObjectClass *gobject_class;
  GstOpencvBaseTransformClass *gstopencvbasefilter_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
  gstopencvbasefilter_class = (GstOpencvBaseTransformClass *) klass;

  parent_class = g_type_class_peek_parent (klass);

  gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_cv_smooth_finalize);
  gobject_class->set_property = gst_cv_smooth_set_property;
  gobject_class->get_property = gst_cv_smooth_get_property;

  gstopencvbasefilter_class->cv_trans_ip_func = gst_cv_smooth_transform_ip;
  gstopencvbasefilter_class->cv_trans_func = gst_cv_smooth_transform;

  g_object_class_install_property (gobject_class, PROP_SMOOTH_TYPE,
      g_param_spec_enum ("type",
          "type",
          "Smooth Type",
          GST_TYPE_CV_SMOOTH_TYPE,
          DEFAULT_CV_SMOOTH_TYPE, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)
      );
  g_object_class_install_property (gobject_class, PROP_PARAM1,
      g_param_spec_int ("param1", "param1",
          "Param1 (Check cvSmooth OpenCV docs)", 1, G_MAXINT, DEFAULT_PARAM1,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_PARAM2,
      g_param_spec_int ("param2", "param2",
          "Param2 (Check cvSmooth OpenCV docs)", 0, G_MAXINT, DEFAULT_PARAM2,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_PARAM3,
      g_param_spec_double ("param3", "param3",
          "Param3 (Check cvSmooth OpenCV docs)", 0, G_MAXDOUBLE, DEFAULT_PARAM3,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_PARAM4,
      g_param_spec_double ("param4", "param4",
          "Param4 (Check cvSmooth OpenCV docs)", 0, G_MAXDOUBLE, DEFAULT_PARAM4,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_cv_smooth_init (GstCvSmooth * filter, GstCvSmoothClass * gclass)
{
  filter->type = DEFAULT_CV_SMOOTH_TYPE;
  filter->param1 = DEFAULT_PARAM1;
  filter->param2 = DEFAULT_PARAM2;
  filter->param3 = DEFAULT_PARAM3;
  filter->param4 = DEFAULT_PARAM4;

  gst_base_transform_set_in_place (GST_BASE_TRANSFORM (filter), FALSE);
}

static void
gst_cv_smooth_change_type (GstCvSmooth * filter, gint value)
{
  GST_DEBUG_OBJECT (filter, "Changing type from %d to %d", filter->type, value);
  if (filter->type == value)
    return;

  filter->type = value;
  switch (value) {
    case CV_GAUSSIAN:
    case CV_BLUR:
      gst_base_transform_set_in_place (GST_BASE_TRANSFORM (filter), TRUE);
      break;
    default:
      gst_base_transform_set_in_place (GST_BASE_TRANSFORM (filter), FALSE);
      break;
  }
}

static void
gst_cv_smooth_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstCvSmooth *filter = GST_CV_SMOOTH (object);

  switch (prop_id) {
    case PROP_SMOOTH_TYPE:
      gst_cv_smooth_change_type (filter, g_value_get_enum (value));
      break;
    case PROP_PARAM1:{
      gint prop = g_value_get_int (value);

      if (prop % 2 == 1) {
        filter->param1 = prop;
      } else {
        GST_WARNING_OBJECT (filter, "Ignoring value for param1, not odd"
            "(%d)", prop);
      }
    }
      break;
    case PROP_PARAM2:{
      gint prop = g_value_get_int (value);

      if (prop % 2 == 1 || prop == 0) {
        filter->param1 = prop;
      } else {
        GST_WARNING_OBJECT (filter, "Ignoring value for param2, not odd"
            " nor zero (%d)", prop);
      }
    }
      break;
    case PROP_PARAM3:
      filter->param3 = g_value_get_double (value);
      break;
    case PROP_PARAM4:
      filter->param4 = g_value_get_double (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_cv_smooth_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstCvSmooth *filter = GST_CV_SMOOTH (object);

  switch (prop_id) {
    case PROP_SMOOTH_TYPE:
      g_value_set_enum (value, filter->type);
      break;
    case PROP_PARAM1:
      g_value_set_int (value, filter->param1);
      break;
    case PROP_PARAM2:
      g_value_set_int (value, filter->param2);
      break;
    case PROP_PARAM3:
      g_value_set_double (value, filter->param3);
      break;
    case PROP_PARAM4:
      g_value_set_double (value, filter->param4);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static GstFlowReturn
gst_cv_smooth_transform (GstOpencvBaseTransform * base, GstBuffer * buf,
    IplImage * img, GstBuffer * outbuf, IplImage * outimg)
{
  GstCvSmooth *filter = GST_CV_SMOOTH (base);

  cvSmooth (img, outimg, filter->type, filter->param1, filter->param2,
      filter->param3, filter->param4);

  return GST_FLOW_OK;
}

static GstFlowReturn
gst_cv_smooth_transform_ip (GstOpencvBaseTransform * base, GstBuffer * buf,
    IplImage * img)
{
  GstCvSmooth *filter = GST_CV_SMOOTH (base);

  cvSmooth (img, img, filter->type, filter->param1, filter->param2,
      filter->param3, filter->param4);

  return GST_FLOW_OK;
}

gboolean
gst_cv_smooth_plugin_init (GstPlugin * plugin)
{
  GST_DEBUG_CATEGORY_INIT (gst_cv_smooth_debug, "cvsmooth", 0, "cvsmooth");

  return gst_element_register (plugin, "cvsmooth", GST_RANK_NONE,
      GST_TYPE_CV_SMOOTH);
}
