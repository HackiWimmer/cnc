#ifndef PATH_GENERATOR_STORE_H
#define PATH_GENERATOR_STORE_H

#include <wx/string.h>
#include <wx/combobox.h>
#include <wx/stc/stc.h>
#include "CncCommon.h"
#include "PathGenerators.h"

///////////////////////////////////////////////////////////////////////////
typedef std::map<unsigned int, PathGeneratorBase*> GeneratorMap;
typedef std::map<wxString, unsigned int> IndexMap;
class PathGeneratorStore {
	
	private:
		GeneratorMap generatorMap;
		IndexMap indexMap;
		
		///////////////////////////////////////////////////////////////////
		wxPGProperty* createProperty(const wxString& type) {
			if 		( type == wxPG_VARIANT_TYPE_BOOL )		return new wxBoolProperty();
			else if ( type == wxPG_VARIANT_TYPE_LIST )		return new wxEnumProperty();
			else 											return new wxStringProperty();
			
			wxASSERT(NULL);
			return NULL;
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
		PathGeneratorStore() {
			registerPathGenerator(new PGenPoint());
			registerPathGenerator(new PGenLine());
			registerPathGenerator(new PGenLongWhole());
			registerPathGenerator(new PGenRoundPoketWhole());
			//...
			registerPathGenerator(new PGenTest());
		}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PathGeneratorStore() {
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
					wxString item(wxString::Format("%03d - %s", it->first, pgb->getName()));
					pgb->setSelectorIndex(selector->Append(item));
				}
			}
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