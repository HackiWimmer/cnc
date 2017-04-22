#ifndef PATHGENERATORFRAME_H
#define PATHGENERATORFRAME_H

#include <wx/string.h>
#include "wxcrafter.h"
#include "CncCommon.h"
#include "PathGenerators.h"

///////////////////////////////////////////////////////////////////////////
typedef std::map<unsigned int, PathGenertorBase*> GeneratorMap;
class PathGenertorStore {
	
	private:
		GeneratorMap generatorMap;
		
	public:
		///////////////////////////////////////////////////////////////////
		PathGenertorStore() {
		}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PathGenertorStore() {
			// delete all items
			for (GeneratorMap::iterator it=generatorMap.begin(); it!=generatorMap.end(); ++it) {
				PathGenertorBase* pgb = it->second;
				if ( pgb != NULL ) {
					delete pgb;
					it->second = NULL;
				}
			}
		}
		
		///////////////////////////////////////////////////////////////////
		void registerPathGenerator(PathGenertorBase* pgb) {
			if ( pgb == NULL )
				return;
				
			pgb->initParameters();
			unsigned int id = generatorMap.size();
			generatorMap[id] = pgb;
		}
		
		///////////////////////////////////////////////////////////////////
		void setupSelector(wxComboBox* selector) {
			if ( selector == NULL )
				return;
				
			selector->Clear();
			for (GeneratorMap::iterator it=generatorMap.begin(); it!=generatorMap.end(); ++it) {
				PathGenertorBase* pgb = it->second;
				wxString name;
				if ( pgb != NULL ) 
					name = pgb->getName();
				
				wxString item(wxString::Format("%03d - %s", it->first, name));
				selector->Append(item);
			}
		}
		
		///////////////////////////////////////////////////////////////////
		void setupParameter(unsigned int id, unsigned int paraIdx, wxPGProperty* property) {
			if ( property == NULL )
				return;
			
			PathGenertorBase* pgb = getPathGenerator(id);
			if ( pgb != NULL ) {
				
				PathGenertorBase::ParameterInfo* pi = pgb->getParameterInfo(paraIdx);
				if ( pi != NULL ) {
					
					wxFloatingPointValidator<float> validator(pi->precision, NULL,wxNUM_VAL_DEFAULT );
					validator.SetRange(pi->minRange, pi->maxRange);
					
					wxVariant defaultValue(0);
					
					property->Hide(false);
					property->Enable(true);
					property->SetLabel(pi->label);
					property->SetValidator(validator);
					property->SetDefaultValue(defaultValue);
					property->SetHelpString(pi->help);
					property->SetValue(pi->value);
				}
			}
		}
		
		///////////////////////////////////////////////////////////////////
		void setCommonValues(unsigned int id, const PathGenertorBase::CommonValues& cv) {
			PathGenertorBase* pgb = getPathGenerator(id);
			if ( pgb != NULL ) 
				pgb->setCommonValues(cv);
		}
		
		///////////////////////////////////////////////////////////////////
		void setTransformValues(unsigned int id, const PathGenertorBase::TransformValues& tv) {
			PathGenertorBase* pgb = getPathGenerator(id);
			if ( pgb != NULL ) 
				pgb->setTransformValues(tv);
		}
		
		///////////////////////////////////////////////////////////////////
		void setParameterValue(unsigned int id, unsigned int paraIdx, wxPGProperty* property) {
			if ( property == NULL )
				return;
			
			PathGenertorBase* pgb = getPathGenerator(id);
			if ( pgb != NULL ) {
				
				PathGenertorBase::ParameterInfo* pi = pgb->getParameterInfo(paraIdx);
				if ( pi != NULL ) {
					pi->value = property->GetValue();
				}
			}
		}
		
		///////////////////////////////////////////////////////////////////
		PathGenertorBase* getPathGenerator(unsigned int id) {
			GeneratorMap::iterator it;
			it = generatorMap.find(id);
			if ( it != generatorMap.end() )
				return it->second;
				
			return NULL;
		}
		
		///////////////////////////////////////////////////////////////////
		const wxString& generatePath(unsigned int id) {
			static wxString s;
			s = "";
			
			PathGenertorBase* pgb = getPathGenerator(id);
			if ( pgb != NULL )
				s = pgb->generate();
			
			return s;
		}
};

///////////////////////////////////////////////////////////////////////////
class PathGeneratorFrame : public PathGeneratorFrameBase
{
	private:
		long previewHeight;
		long previewWidth;
		
		long viewBoxX;
		long viewBoxY;
		long viewBoxW;
		long viewBoxH;
		
		PathGenertorStore pathGeneratorStore;
		
		wxString errorText;

		void closeWindow();
		void updatePreview();
		
		int getPathSelection();
		
		void initControls();
		void setupPathSelector();
		
		void clearParameters();
		void clearParameter(wxPGProperty* p);
		
		
		bool generateSymetricCirclePoints(std::vector<wxRealPoint>& pl, double xOffset, double yOffset, double sections, double radius);
		
		void generatePolygon();
		void generateKnob();
		void generatePocketWhole();
		
		void setPath(wxString path);
		
	protected:
    virtual void updateSvgValues(wxCommandEvent& event);
		virtual void selectPathSelector(wxCommandEvent& event);
		virtual void clearView(wxCommandEvent& event);
		virtual void onCloseWindowFromButton(wxCommandEvent& event);
		virtual void onCloseWindow(wxCloseEvent& event);
		virtual void copyPath(wxCommandEvent& event);
		virtual void generatePath(wxCommandEvent& event);
		
	public:
		PathGeneratorFrame(wxWindow* parent);
		virtual ~PathGeneratorFrame();
		
		wxTextCtrl* getPathTrace() { return m_generatedPath; }
};


#endif 
