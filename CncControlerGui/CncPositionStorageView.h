#ifndef CNCPOSITIONSTORAGEVIEW_H
#define CNCPOSITIONSTORAGEVIEW_H

#include <vector>
#include "CncPosition.h"
#include "wxCrafterBaseDialogs.h"

class CncPositionStorageView : public CncPositionStorageViewBase {
	
	public:
		CncPositionStorageView(wxWindow* parent);
		virtual ~CncPositionStorageView();
		
		void			popProcessMode();
		void			pushProcessMode();

		bool init		(uint8_t sid, bool state);
		bool clear		(uint8_t sid);
		
		bool addPos		(uint8_t sid, const CncLongPosition& p);
		bool addPos		(uint8_t sid, const CncDoublePosition& p);
		bool addPos		(uint8_t sid, int32_t px, int32_t py, int32_t pz);
		bool addPos		(uint8_t sid, double px, double py, double pz);
		
		bool addMove	(uint8_t sid, int32_t dx, int32_t dy, int32_t dz);
		bool addMove	(uint8_t sid, double dx, double dy, double dz);
		
	protected:
		virtual void onCopyDetails(wxCommandEvent& event);
		virtual void onExportDetails(wxCommandEvent& event);
		virtual void onSaveDetails(wxCommandEvent& event);
		virtual void onCloseWindow(wxCloseEvent& event);
		virtual void onOverviewItemSelected(wxListEvent& event);
		void		 onOverviewItemChecked(wxListEvent& event);
		void		 onOverviewItemUnChecked(wxListEvent& event);
		
	private:
		
		// -----------------------------------------------------
		struct OverviewItemInfo {
			enum Format { LONG, DOUBLE };
			
			wxString label	= "";
			wxString help	= "";
			Format format	= LONG;
		};
		
		// -----------------------------------------------------
		struct Entry {
			typedef OverviewItemInfo::Format FMT;
			CncDoublePosition pos;
			
			Entry(const CncDoublePosition& p)
			: pos(p)
			{}
			
			const wxString& getX(wxString& ret, OverviewItemInfo::Format f) {
				switch ( f ) {
					case FMT::LONG:		ret.assign(wxString::Format("% 10ld", (long)pos.getX()));	break;
					case FMT::DOUBLE:	ret.assign(wxString::Format("% 10.3lf", pos.getX()));		break;
				}
				return ret; 
			}
			
			const wxString& getY(wxString& ret, OverviewItemInfo::Format f) {
				switch ( f ) {
					case FMT::LONG:		ret.assign(wxString::Format("% 10ld", (long)pos.getY()));	break;
					case FMT::DOUBLE:	ret.assign(wxString::Format("% 10.3lf", pos.getY()));		break;
				}
				return ret; 
			}
			
			const wxString& getZ(wxString& ret, OverviewItemInfo::Format f) {
				switch ( f ) {
					case FMT::LONG:		ret.assign(wxString::Format("% 10ld", (long)pos.getZ()));	break;
					case FMT::DOUBLE:	ret.assign(wxString::Format("% 10.3lf", pos.getZ()));		break;
				}
				return ret; 
			}
			
			const wxString& trace(wxString& ret, OverviewItemInfo::Format f) {
				switch ( f ) {
					case FMT::LONG:		ret.assign(wxString::Format("% 10ld,% 10ld,% 10ld", (long)pos.getX(), (long)pos.getY(), (long)pos.getZ()));	break;
					case FMT::DOUBLE:	ret.assign(wxString::Format("% 10.3lf,% 10.3lf,% 10.3lf", pos.getX(), pos.getY(), pos.getZ()));				break;
				}
				
				return ret; 
			}
		};
		
		typedef std::vector<Entry> DetailList;
		typedef std::map<uint8_t, DetailList> DetailStorage;
		
		typedef std::vector<OverviewItemInfo> Overview;
		Overview		overview;
		DetailStorage	storage;
		wxString		editorTool;
		wxString		exportTool;
		
		bool				addStorage(int8_t sid);
		
		bool				isItemValid(long item);
		void				onOverviewItemStateChange(long item);
		void				updateDetails();
		void				clearStorage();
		
		inline uint8_t		item2Sid(long item)			{ return (uint8_t)(item + 1); }
		inline long 		sid2Item(uint8_t sid)		{ return sid - 1; }
};

#endif // CNCPOSITIONSTORAGEVIEW_H
