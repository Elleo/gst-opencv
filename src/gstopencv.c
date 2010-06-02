/* GStreamer
 * Copyright (C) <2009> Kapil Agrawal <kapil@mediamagictechnologies.com>
 *
 * gstopencv.c: plugin registering
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
#include "config.h"
#endif

#include "gstcvdilate.h"
#include "gstcvequalizehist.h"
#include "gstcverode.h"
#include "gstcvlaplace.h"
#include "gstcvsmooth.h"
#include "gstcvsobel.h"
#include "gstedgedetect.h"
#include "gstfaceblur.h"
#include "gstfacedetect.h"
#include "gstpyramidsegment.h"
#include "gsttemplatematch.h"
#include "gsttextwrite.h"

static gboolean
plugin_init (GstPlugin * plugin)
{
  if (!gst_cv_dilate_plugin_init (plugin))
    return FALSE;

  if (!gst_cv_equalize_hist_plugin_init (plugin))
    return FALSE;

  if (!gst_cv_erode_plugin_init (plugin))
    return FALSE;

  if (!gst_cv_laplace_plugin_init (plugin))
    return FALSE;

  if (!gst_cv_smooth_plugin_init (plugin))
    return FALSE;

  if (!gst_cv_sobel_plugin_init (plugin))
    return FALSE;

  if (!gst_edgedetect_plugin_init (plugin))
    return FALSE;

  if (!gst_faceblur_plugin_init (plugin))
    return FALSE;

  if (!gst_facedetect_plugin_init (plugin))
    return FALSE;

  if (!gst_pyramidsegment_plugin_init (plugin))
    return FALSE;

  if (!gst_templatematch_plugin_init (plugin))
    return FALSE;

  if (!gst_textwrite_plugin_init (plugin))
    return FALSE;

  return TRUE;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "opencv",
    "GStreamer OpenCV Plugins",
    plugin_init, VERSION, "LGPL", "OpenCv", "http://opencv.willowgarage.com")
