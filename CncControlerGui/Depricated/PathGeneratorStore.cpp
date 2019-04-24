#include "PathGeneratorStore.h"

#include "PathGenerators/PGenRegularDiameterPolygon.h"
#include "PathGenerators/PGenRegularRadiusPolygon.h"
#include "PathGenerators/PGenEllipticalArcPolygon.h"
#include "PathGenerators/PGenRectanglePolygon.h"
#include "PathGenerators/PGenFreehandPolygon.h"
#include "PathGenerators/PGenSimplePoint.h"
#include "PathGenerators/PGenSimpleLine.h"
#include "PathGenerators/PGenFreehandPolyline.h"
#include "PathGenerators/PGenGenericPath.h"
#include "PathGenerators/PGenGenericElement.h"
#include "PathGenerators/PGenRotaryKnob.h"
#include "PathGenerators/PGenEllipticalRing.h"

///////////////////////////////////////////////////////////////////
PathGeneratorStore::PathGeneratorStore() {
	
	registerPathGenerator(new PGenSimplePoint());
	registerPathGenerator(new PGenSimpleLine());
	registerPathGenerator(new PGenFreehandPolyline());

	registerPathGenerator(new PGenRectanglePolygon());
	registerPathGenerator(new PGenEllipticalArcPolygon());
	registerPathGenerator(new PGenRegularRadiusPolygon());
	registerPathGenerator(new PGenRegularDiameterPolygon());
	registerPathGenerator(new PGenFreehandPolygon());
	registerPathGenerator(new PGenRotaryKnob());
	registerPathGenerator(new PGenEllipticalRing());
	registerPathGenerator(new PGenGenericPath());
	registerPathGenerator(new PGenGenericElement());
	
	//...
}
///////////////////////////////////////////////////////////////////
PathGeneratorStore::~PathGeneratorStore() {
///////////////////////////////////////////////////////////////////
	// delete all items
	for (GeneratorMap::iterator it=generatorMap.begin(); it!=generatorMap.end(); ++it) {
		PathGeneratorBase* pgb = it->second;
		if ( pgb != NULL ) {
			delete pgb;
			it->second = NULL;
		}
	}
}
///////////////////////////////////////////////////////////////////
wxTreeItemId PathGeneratorStore::appendTreeItem(wxTreeCtrl* tree, TreeIndex& treeIndex, const wxString& name, const wxString& treePath) {
///////////////////////////////////////////////////////////////////
	wxTreeItemId invalid;
	wxTreeItemIdValue cookie;
	
	if ( tree == NULL )
		// return an invalid item id
		return invalid;
	
	// no tree path available --> add item to root
	if ( treePath.IsEmpty() ) {
		wxTreeItemId prev 	 = tree->GetRootItem().GetID();
		wxTreeItemId current = tree->GetFirstChild(prev, cookie);
		
		while ( current ) {
			if ( tree->ItemHasChildren(current) == true ) {
				// register index and return the new item id
				wxTreeItemId newId = tree->InsertItem(tree->GetRootItem(), prev, name, 3);
				TreeItemInfo tii(TreeItemInfo::Type::TIT_TEMPLATE, name, newId);
				treeIndex.push_back(tii);
				return newId;
			}
			
			prev = current;
			current = tree->GetNextChild(prev, cookie).GetID();
		}
		
		// register index and return the new item id
		wxTreeItemId newId = tree->AppendItem(tree->GetRootItem(), name, 3);
		TreeItemInfo tii(TreeItemInfo::Type::TIT_TEMPLATE, name, newId);
		treeIndex.push_back(tii);
		return newId;
	}
	
	// set parent to root
	wxTreeItemId parent = tree->GetRootItem();
	
	// break down the given tree path
	wxStringTokenizer tokenizer(treePath, "\\");
	while ( tokenizer.HasMoreTokens() ) {
		wxString token = tokenizer.GetNextToken();
		
		bool found = false;
		wxTreeItemId id =  tree->GetFirstChild(parent, cookie).GetID();
		//  over all parent childreen
		while ( id ) {
			if ( tree->GetItemText(id) == token ) {
				// add only of no more tokens are available
				if ( tokenizer.HasMoreTokens() == false ) {
					id = tree->AppendItem(id, name, 3);
					TreeItemInfo tii(TreeItemInfo::Type::TIT_TEMPLATE, name, id);
					treeIndex.push_back(tii);
					
					// redirect parent
					parent = id;
				}
				
				found = true;
				break;
			}
			
			id = tree->GetNextChild(parent, cookie).GetID();
		}
		
		// if the current tree token (parent) doesn't exits
		if ( found == false ) {
			id = tree->AppendItem(parent, token, 1, 2);
			tree->SetItemBold(id);
			TreeItemInfo tii(TreeItemInfo::Type::TIT_PARENT, token, id);
			treeIndex.push_back(tii);
			
			// add only of no more tokens are available
			if ( tokenizer.HasMoreTokens() == false ) {
				id = tree->AppendItem(id, name, 3);
				TreeItemInfo tii(TreeItemInfo::Type::TIT_TEMPLATE, name, id);
				treeIndex.push_back(tii);
			}
			
			// redirect parent
			parent = id;
		}
	}
	
	// return the new item id
	return parent;
}
///////////////////////////////////////////////////////////////////
void PathGeneratorStore::setupSelectorTree(wxTreeCtrl* tree, TreeIndex& treeIndex, wxImageList* imageList) {
///////////////////////////////////////////////////////////////////
	if ( tree == NULL )
		return;
		
	treeIndex.clear();
		
	tree->DeleteAllItems();
	tree->AssignImageList(imageList);
	tree->AddRoot("Templates:", 0);
	tree->SetItemBold(tree->GetRootItem());
	
	for (GeneratorMap::iterator it=generatorMap.begin(); it!=generatorMap.end(); ++it) {
		PathGeneratorBase* pgb = it->second;
		if ( pgb != NULL ) {
			wxString name(wxString::Format("%03d - %s", it->first, pgb->getName()));
			wxTreeItemId newItem = appendTreeItem(tree, treeIndex, name, pgb->getTreePath());
			
			//append pre defined setups
			if ( pgb->hasPreDefinedParameterSetups() ) {
				wxTreeItemId preDefParent;
				wxColour colour(112,146,190);
				// append default
				wxString name(wxString::Format("%s%s", PRE_DEF_MARKER, PRE_DEF_DEFAULT_ITEM));
				preDefParent = tree->AppendItem(newItem, name, 4);
				tree->SetItemTextColour(preDefParent, colour);
				TreeItemInfo tii(TreeItemInfo::Type::TIT_PRE_DEF_PARA_SET, name, preDefParent);
				treeIndex.push_back(tii);
				
				// append configured setups
				wxArrayString sa;
				
				pgb->getPreDefinedParameterSetNames(sa);
				for ( unsigned int i=0; i<sa.GetCount(); i++ ) {
					wxString name(wxString::Format("%s%s", PRE_DEF_MARKER, sa[i]));
					preDefParent = tree->AppendItem(newItem, name, 4);
					tree->SetItemTextColour(preDefParent, colour);
					TreeItemInfo tii(TreeItemInfo::Type::TIT_PRE_DEF_PARA_SET, name, preDefParent);
					treeIndex.push_back(tii);
				}
			}
		}
	}
	
	tree->ExpandAll();
}
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
		rp.out.errorInfo << "Can't find Template Generator: " << result.tplName << "\n";
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
				
				wxPGProperty* newProp = parent->AppendChild(createProperty(*pi));
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
					newProp->SetValue(wxString::Format(format, pi->value.GetDouble()));
					
				} else if ( pi->propertyType == wxPG_VARIANT_TYPE_LIST ) {
					wxPGChoices items(pi->enumItems);
					if ( items.GetCount() <= 0 ) {
						items.Add("???");
						newProp->SetValue((int)items.GetCount()-1);
					} else {
						newProp->SetChoices(items);
						newProp->SetValue(pi->value);
					}
					
				} else if ( pi->propertyType == wxPG_VARIANT_TYPE_STRING ) {
					newProp->SetValueFromString(pi->value.GetString());
					
				} else {
					newProp->SetValue(pi->value);
					
				}
			}
		}
	}
}