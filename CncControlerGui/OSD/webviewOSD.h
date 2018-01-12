#ifndef WEBVIEW_OSD_H
#define WEBVIEW_OSD_H

	#ifdef __WXMSW__
		#include <wx/webview.h>
	#endif

	#ifdef __WXGTK__
		#indclude "OSD/webviewDummy.h"
	#endif

#endif
