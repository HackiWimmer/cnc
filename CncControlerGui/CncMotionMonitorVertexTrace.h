#ifndef CNC_MONITOR_VERTEX_LIST_CTRL_H
#define CNC_MONITOR_VERTEX_LIST_CTRL_H

#include "3D/GLOpenGLPathBufferStore.h"
#include "CncLargeScaleListCtrl.h"

namespace CncVertexTrace {

	class DataListCtrl : public CncLargeScaledListCtrl {
		
		private:
			wxListItemAttr				dataItemAttr;
			GLOpenGLPathBufferStore* 	vertexBufferStore;
			
			void updateColumnWidth();
			
			virtual wxString OnGetItemText(long item, long column) const;
			virtual int OnGetItemColumnImage(long item, long column) const;
			virtual wxListItemAttr *OnGetItemAttr(long item) const;
			
			void onSize(wxSizeEvent& event);
			void onSelectListItem(wxListEvent& event);
			void onActivateListItem(wxListEvent& event);
		
		protected:
			virtual bool isItemValid(long item) const;

		public:
		
			static const int COL_TYPE			=  0;
			static const int COL_BUF_ID			=  1;
			static const int COL_CLD_ID			=  2;
			static const int COL_T				=  3;
			static const int COL_X				=  4;
			static const int COL_Y				=  5;
			static const int COL_Z				=  6;
			static const int COL_R				=  7;
			static const int COL_G				=  8;
			static const int COL_B				=  9;
			static const int COL_A				= 10;
			static const int COL_COLOUR			= 11;
			
			static const int TOTAL_COL_COUNT	= 12;
			
			static const int COL_SEARCH			= COL_CLD_ID;
			static const int COL_STRECH			= COL_COLOUR;
			
			DataListCtrl(wxWindow *parent, long style);
			virtual ~DataListCtrl();
			
			void setVertexBufferStore(GLOpenGLPathBufferStore* vbs);
			bool searchReferenceById(const long id);
			
			wxDECLARE_NO_COPY_CLASS(DataListCtrl);
			wxDECLARE_EVENT_TABLE();

	};
	
	class IndexListCtrl : public CncLargeScaledListCtrl {
		
		private:
			wxListItemAttr 				indexItemAttr;
			GLOpenGLPathBufferStore* 	vertexBufferStore;
			
			void updateColumnWidth();
			
			virtual wxString OnGetItemText(long item, long column) const;
			virtual int OnGetItemColumnImage(long item, long column) const;
			virtual wxListItemAttr *OnGetItemAttr(long item) const;
			
			void onSize(wxSizeEvent& event);
			void onSelectListItem(wxListEvent& event);
			void onActivateListItem(wxListEvent& event);

		protected:
			virtual bool isItemValid(long item) const;

		public:
		
			static const int COL_TYPE			= 0;
			static const int COL_BUF_ID			= 1;
			static const int COL_CLD_ID			= 2;
			static const int COL_INFO			= 3;
			
			static const int TOTAL_COL_COUNT	= 4;
			
			static const int COL_SEARCH			= COL_CLD_ID;
			static const int COL_STRECH			= COL_INFO;
			
			IndexListCtrl(wxWindow *parent, long style);
			virtual ~IndexListCtrl();
			
			void setVertexBufferStore(GLOpenGLPathBufferStore* vbs);
			bool searchReferenceById(const long id);
			
			wxDECLARE_NO_COPY_CLASS(IndexListCtrl);
			wxDECLARE_EVENT_TABLE();

	};
}

#endif