#include "PathGeneratorFrame.h"

///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::setHelpInfos() {
///////////////////////////////////////////////////////////////////
	
	
	//todo
	//m_pgPropCorrection->SetHelpString("adsdadsadd");
	//m_pgPropCncZDepthMode->SetHelpString("wdadadadadasdasd");
	
	m_pgCatCommon->SetHelpString("Common template parameters");
	m_pgPropToolDiameter->SetHelpString("Determine the used tool diameter.");
	//m_pgPropCorrection->SetHelpString("Will be considered by the dedicated template generator to correct length values if it makes sence. If false no correction takes place.");
	m_pgPropPathColour->SetHelpString("Determine the path colour to use.");
	
	m_pgPropCncInclude->SetHelpString("Determine if a <CncParameterBlock/> will be included.");
	
	m_pgPropSvgWidth->SetHelpString("SVG Dimension <width>");
	m_pgPropSvgHeight->SetHelpString("SVG Dimension <height>");
	m_pgCatSvg->SetHelpString("Categorie: SVG parameters");
	
	m_pgCatViewBox->SetHelpString("Categorie: SVG viewBox");
	m_pgCatTransform->SetHelpString("Categorie: SVG transform parameters");
	m_pgPropTransformMode->SetHelpString("Allows the path templates to set the SVG transform parameters if this value is true. Otherwise the SVG transform parameters have to set manually.");
	
	m_pgCatTranslate->SetHelpString("translate(<x> [<y>])\nThis transform definition specifies a translation by x and y.");
	m_pgCatScale->SetHelpString("scale(<x> [<y>])\nThis transform definition specifies a scale operation by x and y.");
	m_pgCatRotate->SetHelpString("rotate(<a> [<x> <y>])\nThis transform definition specifies a rotation by a degrees about a given point.");
	m_pgCatSkrew->SetHelpString("skewX(<a>)\nThis transform definition specifies a skew transformation along the x axis by a degrees. skewY(<a>)\nThis transform definition specifies a skew transformation along the y axis by a degrees.");
}