#
# Regular cron jobs for the gst-opencv package
#
0 4	* * *	root	[ -x /usr/bin/gst-opencv_maintenance ] && /usr/bin/gst-opencv_maintenance
