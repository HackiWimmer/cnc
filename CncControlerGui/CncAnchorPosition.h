#ifndef CNCANCHORPOSITION_H
#define CNCANCHORPOSITION_H

#include "CncAnchorPosListCtrl.h"
#include "wxCrafterReferencePosition.h"

class CncAnchorPosition : public CncAnchorPositionBase 
						, public CncAnchorPosListCtrl::Interface {
	
	private:
		CncAnchorPosListCtrl*	anchorList;
		bool					canAdd;
		bool					canMod;
		bool					canDel;
		
		virtual void notifySelection(const CncAnchorPosListCtrl::AnchorInfo& ai);
		virtual void notifyActivation(const CncAnchorPosListCtrl::AnchorInfo& ai);
		
		void load();
		void update();
		void close();
		void provide();
		
		void processType();
		
	public:
		CncAnchorPosition(wxWindow* parent);
		virtual ~CncAnchorPosition();
		
	protected:
		virtual void onSelectType(wxCommandEvent& event);
		virtual void onCloseWindow(wxCloseEvent& event);
		virtual void onInitDialog(wxInitDialogEvent& event);
		virtual void onShow(wxShowEvent& event);
		virtual void onUpdateName(wxCommandEvent& event);
		virtual void onClose(wxCommandEvent& event);
		virtual void onAdd(wxCommandEvent& event);
		virtual void onDel(wxCommandEvent& event);
		virtual void onMod(wxCommandEvent& event);
};

#endif // CNCANCHORPOSITION_H
