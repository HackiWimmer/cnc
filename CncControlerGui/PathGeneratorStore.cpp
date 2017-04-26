#include "PathGeneratorStore.h"

///////////////////////////////////////////////////////////////////
bool PathGeneratorStore::regenerateSvgBlock(RegenerateParameter& rp) {
///////////////////////////////////////////////////////////////////
	rp.out.errorInfo.clear();
	
	PathGeneratorBase::XmlPatternResult result;
	if ( PathGeneratorBase::decodeXmlPattern(rp.in.cncPattern, result) == false ) {
		rp.out.errorInfo << result.errorInfo << "\n";
		return false;
	}
	 
	PathGeneratorBase* pGen = getPathGenerator(result.tplName);
	if ( pGen == NULL) {
		rp.out.errorInfo << "Cant find Template Generator: " << result.tplName << "\n";
		return false;
	}
	
	if ( pGen->getCheckSumAsString() != result.checkSum ) {
		rp.out.errorInfo << "Invalid CheckSum detected: " << pGen->getCheckSumAsString() << "!=" << result.checkSum  << "\n";
		return false;
	}
	
	// set default values
	result.commonValues.configBlock 	= true;
	result.commonValues.referenceCross 	= true;
	result.commonValues.xmlPattern		= true;
	result.setToolDiameter(rp.in.toolDiameter);
	pGen->setCommonValues(result.commonValues);
	
	result.transformValues.copyCanValues(pGen->getTransformValues());
	pGen->setTransformValues(result.transformValues);
	
	result.cncParameterValues.copyCanValues(pGen->getCncParameterValues());
	pGen->setCncParameterValues(result.cncParameterValues);
	
	// generate 
	rp.out.resultigSvgFragment.assign(pGen->generatePath(rp.in.toolDiameter, result.pMap));
	SvgGeneratorBase::finalizeSvgFragment(rp.out.resultigSvgFragment, false);
	
	return true;
}
///////////////////////////////////////////////////////////////////
void PathGeneratorStore::setupParameter(unsigned int id, wxPGProperty* parent) {
///////////////////////////////////////////////////////////////////
	if ( parent == NULL )
		return;
	
	PathGeneratorBase* pgb = getPathGenerator(id);
	if ( pgb != NULL ) {
		
		for (unsigned int i=0; i<pgb->getParameterCount(); i++) {
			PathGeneratorBase::ParameterInfo* pi = pgb->getParameterInfo(i);
			if ( pi != NULL ) {
				
				wxPGProperty* newProp = parent->AppendChild(createProperty(pi->propertyType));;
				newProp->Hide(false);
				newProp->Enable(true);
				newProp->SetLabel(pi->label);
				newProp->SetHelpString(pi->help);
				
				// specialize and set value
				if ( pi->propertyType == wxPG_VARIANT_TYPE_DOUBLE ) {
					wxFloatingPointValidator<float> validator(pi->precision, NULL, wxNUM_VAL_DEFAULT);
					wxString format("%."); format << pi->precision; format << "lf";
					
					validator.SetRange(pi->minRange, pi->maxRange);
				
					wxVariant defaultValue(0);
					newProp->SetDefaultValue(defaultValue);
					newProp->SetValidator(validator);
					newProp->SetValue(wxString::Format("%.3lf", pi->value.GetDouble()));
					
				} else if ( pi->propertyType == wxPG_VARIANT_TYPE_LIST ) {
					wxPGChoices items(pi->enumItems);
					if ( items.GetCount() <= 0 ) {
						items.Add("???");
						newProp->SetValue(0);
					} else {
						newProp->SetChoices(items);
						newProp->SetValue(pi->value);
					}
					
				} else {
					newProp->SetValue(pi->value);
				}
			}
		}
	}
}