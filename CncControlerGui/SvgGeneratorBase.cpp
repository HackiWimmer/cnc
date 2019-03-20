#include "SvgGeneratorBase.h"

const wxString SvgGeneratorBase::svgCommentStart				= wxString("!--");
const wxString SvgGeneratorBase::svgCommentEnd					= wxString("--");
const wxString SvgGeneratorBase::svgCommentPlaceholderStart		= wxString("$SVG_PH_S");
const wxString SvgGeneratorBase::svgCommentPlaceholderEnd		= wxString("$SVG_PH_E");
const wxString SvgGeneratorBase::svgConfigPlaceholderBlock		= wxString("\n<!-- $CONFIGURATION_BLOCK -->");
