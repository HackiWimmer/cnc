#ifndef CNC_OPENGL_OBSERVER_LIST_CTRL_H
#define CNC_OPENGL_OBSERVER_LIST_CTRL_H

#include <wx/textctrl.h>
#include "CncLargeScaleListCtrl.h"

class CncOpenGLCurrentContextObserverListCtrl : public CncLargeScaledListCtrl {
	
	protected:
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxString OnGetItemText(long item, long column) const;
		virtual bool isItemValid(long item) const;
		
	public:
		static const int COL_KEY 			= 0;
		static const int COL_VAL 			= 1;
		static const int TOTAL_COL_COUNT	= 2;
		
		CncOpenGLCurrentContextObserverListCtrl(wxWindow *parent, long style);
		virtual ~CncOpenGLCurrentContextObserverListCtrl();
		
		wxDECLARE_NO_COPY_CLASS(CncOpenGLCurrentContextObserverListCtrl);
};

class CncOpenGLRegisteredContextObserverListCtrl : public CncLargeScaledListCtrl {
	
	protected:
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxString OnGetItemText(long item, long column) const;
		virtual bool isItemValid(long item) const;
		
	public:
		static const int COL_KEY 			= 0;
		static const int COL_VAL 			= 1;
		static const int TOTAL_COL_COUNT	= 2;
		
		CncOpenGLRegisteredContextObserverListCtrl(wxWindow *parent, long style);
		virtual ~CncOpenGLRegisteredContextObserverListCtrl();
		
		wxDECLARE_NO_COPY_CLASS(CncOpenGLRegisteredContextObserverListCtrl);
};

class CncOpenGLContextObserverHistoryListCtrl : public CncLargeScaledListCtrl {
	
	private:
		wxListItemAttr defaultItemAttr;
		wxListItemAttr warningItemAttr;
		wxListItemAttr errorItemAttr;
		wxListItemAttr switchItemAttr;
		
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
	protected:
		wxMenu* 	popupMenu;
		wxTextCtrl*	detailInfo;
		
		void onRightDown(wxMouseEvent& event);
		
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);
		
	public:
		static const int COL_TIME 			= 0;
		static const int COL_TYPE 			= 1;
		static const int COL_MODE 			= 2;
		static const int COL_CTX 			= 3;
		static const int COL_FUNCT 			= 4;
		static const int COL_MSG 			= 5;
		static const int TOTAL_COL_COUNT	= 6;
		
		CncOpenGLContextObserverHistoryListCtrl(wxWindow *parent, long style);
		virtual ~CncOpenGLContextObserverHistoryListCtrl();
		
		void setDetaiInfoControl(wxTextCtrl* tcl) { detailInfo = tcl; }
		bool appendHistoryItem(const CncColumContainer& cc);
		
		wxDECLARE_NO_COPY_CLASS(CncOpenGLContextObserverHistoryListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif