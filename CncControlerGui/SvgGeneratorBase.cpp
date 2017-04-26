#include "SvgGeneratorBase.h"


wxString SvgGeneratorBase::svgCommentStart				= _T("!--");
wxString SvgGeneratorBase::svgCommentEnd				= _T("--");
wxString SvgGeneratorBase::svgCommentPlaceholderStart	= _T("$SVG_PH_S");
wxString SvgGeneratorBase::svgCommentPlaceholderEnd		= _T("$SVG_PH_E");
wxString SvgGeneratorBase::svgConfigPlaceholderBlock	= _T("\n<!-- $CONFIGURATION_BLOCK -->");


