#ifndef CNCTOOLMAGAZINE_H
#define CNCTOOLMAGAZINE_H
#include "wxcrafter.h"

class CncToolMagazine : public CncToolMagazineBase
{
	public:
		CncToolMagazine(wxWindow* parent);
		virtual ~CncToolMagazine();
		
	protected:
    virtual void cancle(wxCommandEvent& event);
		virtual void addTool(wxCommandEvent& event);
		virtual void editTool(wxCommandEvent& event);
		virtual void removeTool(wxCommandEvent& event);
		virtual void selectedTool(wxListEvent& event);
		
	private:
		long lastSelectedItem;
		void itemSelected(unsigned int index);
};
#endif // CNCTOOLMAGAZINE_H
