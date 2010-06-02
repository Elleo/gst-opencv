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

#include "gstcvdilate.h"

GST_DEBUG_CATEGORY_STATIC (gst_cv_dilate_debug);
#define GST_CAT_DEFAULT gst_cv_dilate_debug

GST_BOILERPLATE (GstCvDilate, gst_cv_dilate, GstCvDilateErode,
    GST_TYPE_CV_DILATE_ERODE);

static GstFlowReturn gst_cv_dilate_transform_ip (GstOpencvBaseTransform *
    filter, GstBuffer * buf, IplImage * img);
static GstFlowReturn gst_cv_dilate_transform (GstOpencvBaseTransform * filter,
    GstBuffer * buf, IplImage * img, GstBuffer * outbuf, IplImage * outimg);

/* GObject vmethod implementations */
static void
gst_cv_dilate_base_init (gpointer gclass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (gclass);

  gst_element_class_set_details_simple (element_class,
      "cvdilate",
      "Transform/Effect/Video",
      "Applies cvDilate OpenCV function to the image",
      "Thiago Santos<thiago.sousa.santos@collabora.co.uk>");
}

/* initialize the cvdilate's class */
static void
gst_cv_dilate_class_init (GstCvDilateClass * klass)
{
  GstOpencvBaseTransformClass *gstopencvbasefilter_class;

  gstopencvbasefilter_class = (GstOpencvBaseTransformClass *) klass;

  parent_class = g_type_class_peek_parent (klass);

  gstopencvbasefilter_class->cv_trans_ip_func = gst_cv_dilate_transform_ip;
  gstopencvbasefilter_class->cv_trans_func = gst_cv_dilate_transform;
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_cv_dilate_init (GstCvDilate * filter, GstCvDilateClass * gclass)
{
}

static GstFlowReturn
gst_cv_dilate_transform (GstOpencvBaseTransform * base, GstBuffer * buf,
    IplImage * img, GstBuffer * outbuf, IplImage * outimg)
{
  GstCvDilateErode *filter = GST_CV_DILATE_ERODE (base);

  /* TODO support kernel as a parameter */
  cvDilate (img, outimg, NULL, filter->iterations);

  return GST_FLOW_OK;
}

static GstFlowReturn
gst_cv_dilate_transform_ip (GstOpencvBaseTransform * base, GstBuffer * buf,
    IplImage * img)
{
  GstCvDilateErode *filter = GST_CV_DILATE_ERODE (base);

  cvDilate (img, img, NULL, filter->iterations);

  return GST_FLOW_OK;
}

gboolean
gst_cv_dilate_plugin_init (GstPlugin * plugin)
{
  GST_DEBUG_CATEGORY_INIT (gst_cv_dilate_debug, "cvdilate", 0, "cvdilate");

  return gst_element_register (plugin, "cvdilate", GST_RANK_NONE,
      GST_TYPE_CV_DILATE);
}
