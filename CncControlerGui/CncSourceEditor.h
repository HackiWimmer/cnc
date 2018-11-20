#ifndef CNC_SOURCE_EDITOR_H
#define CNC_SOURCE_EDITOR_H

#include <wx/stc/stc.h>
#include <wx/filename.h>
#include "3D/GLContextBase.h"
#include "BinaryFileParser.h"
#include "CncCommon.h"



////////////////////////////////////////////////////////////////////
// user defined scintila style options
enum {
		MARGIN_LINE_NUMBERS, 
		MARGIN_EDIT_TRACKER,
		MARGIN_BREAKPOINT,
		MARGIN_FOLD
};

enum {
		TE_DEFAULT_STYLE = 242,
		TE_BREAKPOINT_STYLE,
		TE_LINE_MODIFIED_STYLE,
		TE_LINE_SAVED_STYLE
};

class CncSourceEditor : public wxStyledTextCtrl {
	
	public:
	
		struct FileInfo {
			
			// common variables
			TemplateFormat				format;
			wxFileName					fileName;
			GLContextBase::ModelType 	modelType;
			cnc::LineNumberTranslater	lineNumberTranslater;
			
			// binary format specials
			BinaryFileParser::ViewType 	binaryViewType;
			TemplateFormat				binaryOrigFomat;
			
			FileInfo() { reset(); }
			
			void reset() {
				fileName.Clear();
				lineNumberTranslater.clear();
				
				format 				= TplUnknown;
				modelType			= GLContextBase::ModelType::MT_RIGHT_HAND;
				
				binaryOrigFomat		= TplUnknown;
				binaryViewType 		= BinaryFileParser::ViewType::ReadableMetric;
			}
		};
		
	public:
		CncSourceEditor(wxWindow *parent);
		virtual ~CncSourceEditor();
		
		virtual bool Enable(bool enable = true);
		
		void gotoBegin();
		void gotoEnd();
		bool selectLineNumber(unsigned long ln, const char* searchKey = NULL);
		
		bool openFile(const wxString& fileName);
		void changeBinaryViewType(const BinaryFileParser::ViewType bvt);
		
		void prepareNewTemplateFile(TemplateFormat tf);
		
		const GLContextBase::ModelType getModelType() 	{ return fileInfo.modelType; }
		const FileInfo& getFileInfo()					{ return fileInfo; }
	
		static TemplateFormat evaluateTemplateFormatFromFileName(const wxString& fileName);
		static TemplateFormat evaluateTemplateFromExtention(const wxString& ext);
		
	protected:
		
		struct Styles {
			// Define used fonts
			wxFont defaultFont		= wxFont(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
			wxFont staticFont  		= wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
			
			//define colours
			wxColour clDefaultBck	= wxColour(0, 0, 0);
			wxColour clDefaultFgd	= wxColour(175, 175, 175);
			
			wxColour clCaretBck		= wxColour(64, 64, 64);
			wxColour clCaretFgd		= wxColour(*wxWHITE);
			
			wxColour clComment		= wxColour(150, 150, 150);
			wxColour clIdentifier	= wxColour(135, 206, 250);
			wxColour clAttribute	= wxColour(128, 139, 237);
			wxColour clNumber		= wxColour(128,255,128);
			wxColour clOperator		= wxColour(255,128,128);
		};
		
		Styles styles;
		FileInfo fileInfo;
		
		void setupDefaultStyle();
		void setupStyle();
		void setupSvgStyle();
		void setupGcodeStyle();
		void setupBinaryStyle();
		void setupModelType();
		
		bool selectLineDefault(unsigned long ln);
		bool selectLineSvg(unsigned long ln, const char* searchKey);
		bool selectLineBinary(unsigned long ln);
		
		bool openTextFile();
		bool openBinaryFile();
		
		void onUpdateFilePosition();
		
		virtual void onMarginClick(wxStyledTextEvent& event);
		virtual void onChange(wxStyledTextEvent& event);
		virtual void onKeyDown(wxKeyEvent& event);
		virtual void onKeyUp(wxKeyEvent& event);
		virtual void onLeftDown(wxMouseEvent& event);
		virtual void onLeftUp(wxMouseEvent& event);
		virtual void onLeftDClick(wxMouseEvent& event);
		virtual void onRightDown(wxMouseEvent& event);
		
		wxDECLARE_EVENT_TABLE();
};

#endif