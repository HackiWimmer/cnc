#ifndef PATH_GENERATOR_STORE_H
#define PATH_GENERATOR_STORE_H

#include <wx/string.h>
#include <wx/combobox.h>
#include <wx/treectrl.h>
#include <wx/stc/stc.h>
#include "PathGeneratorBase.h"
#include "CncCommon.h"


///////////////////////////////////////////////////////////////////////////
struct TreeItemInfo {
	enum Type {TIT_Unkonwn, TIT_TEMPLATE, TIT_PARENT};
	
	Type			itemType 	= TIT_Unkonwn;
	wxString 		itemName	= "";
	wxTreeItemId 	itemId;		// invalid
	
	//////////////////////////////////////////////////////////////////////
	TreeItemInfo(Type t, const wxString& n, wxTreeItemId id) {
		itemType = t;
		itemName = n;
		itemId   = id;
	}
};

typedef std::vector<TreeItemInfo> TreeIndex;
typedef std::map<unsigned int, PathGeneratorBase*> GeneratorMap;
typedef std::map<wxString, unsigned int> IndexMap;

class PathGeneratorStore {
	
	private:
		GeneratorMap generatorMap;
		IndexMap indexMap;
		
		///////////////////////////////////////////////////////////////////
		wxPGProperty* createProperty(const PathGeneratorBase::ParameterInfo& pi) {
			if ( pi.propertyType == wxPG_VARIANT_TYPE_BOOL ) {
				return new wxBoolProperty();
				
			} else if ( pi.propertyType == wxPG_VARIANT_TYPE_LIST ) {
				return new wxEnumProperty();
				
			} else if ( pi.propertyType == wxPG_VARIANT_TYPE_STRING ) {
				if ( pi.withButton == true ) 	return new wxLongStringProperty();
				else							return new wxStringProperty();
				
			} else {
				return new wxStringProperty();
				
			}
			
			wxASSERT(NULL);
			return NULL;
		}
		
		///////////////////////////////////////////////////////////////////
		wxTreeItemId appendTreeItem(wxTreeCtrl* tree, TreeIndex& treeIndex, const wxString& name, const wxString& treePath) {
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
							// redirect parent
							parent = tree->AppendItem(id, name, 3);
							TreeItemInfo tii(TreeItemInfo::Type::TIT_TEMPLATE, token, parent);
							treeIndex.push_back(tii);
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
		
	public:
		
		struct RegenerateParameter {
			struct In {
				wxStyledTextCtrl* editControl	= NULL;
				wxString cncPattern				= _T("");
				double toolDiameter				= 0.0;
			} in;
			
			struct Out {
				wxString errorInfo				= _T("");
				wxString resultigSvgFragment	= _T("");
			} out;
		};
		
		///////////////////////////////////////////////////////////////////
		PathGeneratorStore();
		virtual ~PathGeneratorStore();
		
		///////////////////////////////////////////////////////////////////
		int getGenertorCount() {
			return generatorMap.size();
		}
		
		///////////////////////////////////////////////////////////////////
		void initPathGenerator(int id) {
			PathGeneratorBase* pgb = getPathGenerator(id);
			wxASSERT(pgb);
			initPathGenerator(pgb);
		}
		
		///////////////////////////////////////////////////////////////////
		void initPathGenerator(PathGeneratorBase* pgb) {
			wxASSERT(pgb);
			pgb->clearParameters();
			pgb->initParameters();
		}
		
		///////////////////////////////////////////////////////////////////
		void registerPathGenerator(PathGeneratorBase* pgb) {
			if ( pgb == NULL )
				return;
				
			initPathGenerator(pgb);
			unsigned int id = generatorMap.size();
			generatorMap[id] = pgb;
			
			wxASSERT(pgb->getName().IsEmpty() == false);
			wxASSERT(getPathGenerator(pgb->getName()) == NULL);
			indexMap[pgb->getName()] = id;
		}
		
		///////////////////////////////////////////////////////////////////
		void setupSelector(wxComboBox* selector) {
			if ( selector == NULL )
				return;
				
			selector->Clear();
			for (GeneratorMap::iterator it=generatorMap.begin(); it!=generatorMap.end(); ++it) {
				PathGeneratorBase* pgb = it->second;
				if ( pgb != NULL ) {
					wxString item;
					if ( pgb->getTreePath().IsEmpty())	item.assign(wxString::Format("%03d - %s", it->first, pgb->getName()));
					else								item.assign(wxString::Format("%03d - %s\\%s", it->first, pgb->getTreePath(), pgb->getName()));
					pgb->setSelectorIndex(selector->Append(item));
				}
			}
		}
		
		///////////////////////////////////////////////////////////////////
		void setupSelectorTree(wxTreeCtrl* tree, TreeIndex& treeIndex, wxImageList* imageList) {
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
					
					// register tree index
					if ( newItem ) {
						/*
						pgb->setSelectorIndex(treeIndex.size());
						 * */
					}
				}
			}
			
			tree->ExpandAll();
		}
		
		///////////////////////////////////////////////////////////////////
		void setupParameter(unsigned int id, wxPGProperty* parent);
		
		///////////////////////////////////////////////////////////////////
		const PathGeneratorBase::CommonValues& getCommonValues(unsigned int id) {
			PathGeneratorBase* pgb = getPathGenerator(id);
			wxASSERT(pgb);
			return pgb->getCommonValues();
		}
		
		///////////////////////////////////////////////////////////////////
		void setCommonValues(unsigned int id, const PathGeneratorBase::CommonValues& cv) {
			PathGeneratorBase* pgb = getPathGenerator(id);
			wxASSERT(pgb); 
			pgb->setCommonValues(cv);
		}
		
		///////////////////////////////////////////////////////////////////
		const PathGeneratorBase::TransformValues& getTransformValues(unsigned int id) {
			PathGeneratorBase* pgb = getPathGenerator(id);
			wxASSERT(pgb);
			return pgb->getTransformValues();
		}

		///////////////////////////////////////////////////////////////////
		void setTransformValues(unsigned int id, const PathGeneratorBase::TransformValues& tv) {
			PathGeneratorBase* pgb = getPathGenerator(id);
			wxASSERT(pgb);
			pgb->setTransformValues(tv);
		}
		
		///////////////////////////////////////////////////////////////////
		const PathGeneratorBase::CncParameterValues& getCncParameterValues(unsigned int id) {
			PathGeneratorBase* pgb = getPathGenerator(id);
			wxASSERT(pgb);
			return pgb->getCncParameterValues();
		}
		
		///////////////////////////////////////////////////////////////////
		void setCncParameterValues(unsigned int id, const PathGeneratorBase::CncParameterValues& cv) {
			PathGeneratorBase* pgb = getPathGenerator(id);
			wxASSERT(pgb);
			pgb->setCncParameterValues(cv);
		}
		
		///////////////////////////////////////////////////////////////////
		void setParameterValue(unsigned int id, unsigned int paraIdx, wxPGProperty* property) {
			if ( property == NULL )
				return;
			
			PathGeneratorBase* pgb = getPathGenerator(id);
			if ( pgb != NULL ) {
				
				PathGeneratorBase::ParameterInfo* pi = pgb->getParameterInfo(paraIdx);
				if ( pi != NULL ) {
					pi->value = property->GetValue();
				}
			}
		}
		
		///////////////////////////////////////////////////////////////////
		PathGeneratorBase* getPathGenerator(unsigned int id) {
			GeneratorMap::iterator it;
			it = generatorMap.find(id);
			if ( it != generatorMap.end() )
				return it->second;
				
			return NULL;
		}
		
		///////////////////////////////////////////////////////////////////
		PathGeneratorBase* getPathGenerator(const wxString& name) {
			IndexMap::iterator it;
			it = indexMap.find(name);
			if ( it != indexMap.end() ) 
				return getPathGenerator(it->second);
				
			return NULL;
		}
		
		///////////////////////////////////////////////////////////////////
		const wxString& generatePath(unsigned int id) {
			static wxString s;
			s = "";
			
			PathGeneratorBase* pgb = getPathGenerator(id);
			if ( pgb != NULL )
				s = pgb->generatePath();
				
			return s;
		}
		
		///////////////////////////////////////////////////////////////////
		bool hasErrorInfo(unsigned int id) {
			PathGeneratorBase* pgb = getPathGenerator(id);
			if ( pgb != NULL )
				return pgb->hasErrorInfo();
				
			return false;
		}
		
		///////////////////////////////////////////////////////////////////
		void resetErrorInfo(unsigned int id) {
			PathGeneratorBase* pgb = getPathGenerator(id);
			if ( pgb != NULL )
				return pgb->resetErrorInfo();
		}
		
		///////////////////////////////////////////////////////////////////
		const wxString& getErrorInfo(unsigned int id) {
			PathGeneratorBase* pgb = getPathGenerator(id);
			if ( pgb != NULL )
				return pgb->getErrorInfo();
			
			return _T("");
		}
		
		///////////////////////////////////////////////////////////////////
		bool isReferencePointDefined(int id) {
			PathGeneratorBase* pgb = getPathGenerator(id);
			if ( pgb != NULL )
				return pgb->isReferencePointDefined();
				
			return false;
		}
		
		///////////////////////////////////////////////////////////////////
		const wxRealPoint& getReferencePoint(int id) {
			PathGeneratorBase* pgb = getPathGenerator(id);
			if ( pgb != NULL )
				return pgb->getReferencePoint();
			
			static wxRealPoint p(0.0, 0.0);
			return p;
		}
		
		///////////////////////////////////////////////////////////////////
		bool regenerateSvgBlock(RegenerateParameter& rp);
};

#endif