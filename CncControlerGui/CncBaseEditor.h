#ifndef CNC_BASE_EDITOR_H
#define CNC_BASE_EDITOR_H

#include <string>
#include <set>
#include <wx/stc/stc.h>
#include <wx/filename.h>
#include <wx/settings.h>
#include <wx/timer.h>
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

////////////////////////////////////////////////////////////////////
class CncAutoCompleteList {
	
	private:
		
		struct InsensitiveCompare { 
			bool operator() (const std::string& a, const std::string& b) const {
				return strcasecmp(a.c_str(), b.c_str()) < 0;
			}
		};
		
		typedef std::set<std::string, InsensitiveCompare> List; 
		
		List				list;
		char				separator;
		wxStyledTextCtrl*	stc;
	
	public:
		CncAutoCompleteList(wxStyledTextCtrl* stc, char separator);
		~CncAutoCompleteList();
		
		bool suggest(const wxString& prefix);
		void add(const wxString& token);
};

////////////////////////////////////////////////////////////////////
class CncBaseEditor : public wxStyledTextCtrl {
	
	public:
	
		struct FileInfo {
			
			// common variables
			CncTemplateFormat				format;
			wxFileName						fileName;
			GLContextBase::ModelType		modelType;
			cnc::LineNumberTranslater		lineNumberTranslater;
			CncDimensions					modelViewType;
			
			// binary format specials
			BinaryFileParser::ViewType		binaryViewType;
			CncTemplateFormat				binaryOrigFomat;
			
			FileInfo() { reset(); }
			
			void reset() {
				fileName.Clear();
				lineNumberTranslater.clear();
				
				format 				= TplUnknown;
				modelType			= GLContextBase::ModelType::MT_RIGHT_HAND;
				modelViewType		= CncDimensions::CncDimension3D;
				
				binaryOrigFomat		= TplUnknown;
				binaryViewType 		= BinaryFileParser::ViewType::ReadableMetric;
			}
		};
		
	public:
		CncBaseEditor(wxWindow *parent);
		virtual ~CncBaseEditor();
		
		virtual bool Enable(bool enable = true);
		virtual void clearContent();
		virtual void decorateParentTabName(bool changed) {}
		
		void gotoBegin();
		void gotoEnd();
		
		bool selectLineNumber(unsigned long ln, const char* searchKey = NULL);
		bool selectLineNumbers(unsigned long firstLine, unsigned long lastLine);
		
		void prepareNewTemplateFile(CncTemplateFormat tf);
		void setNewTemplateFileName(const wxString& fileName);
		bool openFile(const wxString& fileName);
		bool saveFile();
		bool saveFileAs(const wxString& fileName);
		
		void changeBinaryViewType(const BinaryFileParser::ViewType bvt);
		void setTryToSelectFlag(bool flag)				{ tryToSelectFlag = flag; }
		
		const CncDimensions getModelViewType()			{ return fileInfo.modelViewType; }
		const GLContextBase::ModelType getModelType() 	{ return fileInfo.modelType; }
		const FileInfo& getFileInfo()					{ return fileInfo; }
		
	protected:
		
		struct AutoCompleteInfo {
			enum Type { ACT_NONE, ACT_SVG_TOKEN, ACT_CNC_TOKEN };
			
			wxString	token		= "";
			Type		type		= ACT_NONE;
			
			void reset() { *this = AutoCompleteInfo(); }
		};
		
		struct Styles 
		{
			// Define used fonts
			//wxFont defaultFont		= wxFont(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
			wxFont defaultFont		= wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
			wxFont staticFont  		= wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
			
			//define colours
			wxColour clDefaultBck	= wxColour(0, 0, 0);
			wxColour clDisabledBck	= wxColour(32, 32, 32);
			wxColour clDefaultFgd	= wxColour(175, 175, 175);
			
			wxColour clCaretBck		= wxColour(64, 64, 64);
			wxColour clCaretFgd		= wxColour(*wxWHITE);
			
			wxColour clComment		= wxColour(150, 150, 150);
			wxColour clIdentifier	= wxColour(135, 206, 250);
			wxColour clAttribute	= wxColour(128, 139, 237);
			wxColour clNumber		= wxColour(128, 255, 128);
			wxColour clOperator		= wxColour(255, 128, 128);
		};
		
		struct Flags 
		{
			bool handleBreakpoints		= true;
			bool handleKeyCommands		= true;
		};
		
		class SelectEventBlocker 
		{
			private:
				CncBaseEditor* editor;
			public:
				SelectEventBlocker(CncBaseEditor* e) : editor(e) { if ( editor != NULL) editor->blockSelectEvent = true; }
				~SelectEventBlocker()							 { if ( editor != NULL) editor->blockSelectEvent = false; }
		};
		
		Styles				styles;
		Flags				flags;
		AutoCompleteInfo	lastAutoCompleteInfo;
		FileInfo			fileInfo;
		wxMenu*				svgPopupMenu;
		wxStaticText*		ctlEditMode;
		wxStaticText*		ctlColunmPostion;
		wxTextCtrl*			ctlStatus;
		wxTimer				clientIDTimer;
		wxTimer				suggestionTimer;
		long				firstClientIdToSel;
		long				lastClientIdToSel;
		
		bool				tryToSelectFlag;
		bool				blockSelectEvent;
		bool				fileLoadingActive;
		
		CncAutoCompleteList	svgBlockTypes;
		CncAutoCompleteList	cncBlockParameters;
		
		virtual void initialize() = 0;
		virtual bool hasEditMode() { return true; }
		
		virtual void registerClientIdsToSelect(long firstCID, long lastCID);
		
		virtual void notifyChange()	{}
		virtual void notifySave()	{}
		
		void setupDefaultStyle();
		void setupStyle();
		void setupTextStyle();
		void setupSvgStyle();
		void setupGcodeStyle();
		void setupBinaryStyle();
		void setupModelType();
		
		void suggest(AutoCompleteInfo::Type t, const wxString& token);
		
		bool selectLinesDefault(unsigned long firstLine, unsigned long lastLine);
		bool selectLinesBinary(unsigned long firstLine, unsigned long lastLine);
		bool selectLinesSvg(unsigned long firstLine, unsigned long lastLine);

		bool selectLineDefault(unsigned long ln);
		bool selectLineSvg(unsigned long ln, const char* searchKey);
		bool selectLineBinary(unsigned long ln);
		
		bool openTextFile();
		bool openBinaryFile();
		
		bool save();
		
		void onUpdateFilePosition(bool publishSelection);
		
		virtual void onMarginClick(wxStyledTextEvent& event);
		virtual void onSave(wxStyledTextEvent& event);
		virtual void onChange(wxStyledTextEvent& event);
		virtual void onKeyDown(wxKeyEvent& event);
		virtual void onKeyUp(wxKeyEvent& event);
		virtual void onLeftDown(wxMouseEvent& event);
		virtual void onLeftUp(wxMouseEvent& event);
		virtual void onLeftDClick(wxMouseEvent& event);
		virtual void onRightDown(wxMouseEvent& event);
		virtual void onClientIDTimer(wxTimerEvent& event);
		virtual void onSuggestionTimer(wxTimerEvent& event);
		virtual void onCharAdded(wxStyledTextEvent &event);
		virtual void onAutoCompleteSelected(wxStyledTextEvent &event);
		
		wxStaticText* 	getCtlEditMode() 	{ return ctlEditMode; }
		wxStaticText* 	getCtlColumnPos() 	{ return ctlColunmPostion; }
		wxTextCtrl*		getCtlStatus() 		{ return ctlStatus; }
		
		wxDECLARE_EVENT_TABLE();
};

#endif