#include <iostream>
#include <climits>
#include "CncLargeScaleListCtrl.h"

///////////////////////////////////////////////////////////////////
CncLargeScaledListCtrl::CncLargeScaledListCtrl(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, 
											   long style, const wxValidator &validator, const wxString &name)
: wxListCtrl(parent, id, pos, size, (style | wxLC_REPORT | wxLC_VIRTUAL), validator, name)
, dummyRow(NULL)
, rows()
, listType(CncLargeScaledListCtrl::ListType::NORMAL)
, lastSelection(wxNOT_FOUND)
///////////////////////////////////////////////////////////////////
{
	rows.reserve( 1024 * 1024);
}
///////////////////////////////////////////////////////////////////
CncLargeScaledListCtrl::CncLargeScaledListCtrl(wxWindow *parent, long style) 
: wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, (style | wxLC_REPORT | wxLC_VIRTUAL) )
, dummyRow(NULL)
, rows()
, listType(CncLargeScaledListCtrl::ListType::NORMAL)
, lastSelection(wxNOT_FOUND)
///////////////////////////////////////////////////////////////////
{
	rows.reserve( 1024 * 1024);
}
///////////////////////////////////////////////////////////////////
CncLargeScaledListCtrl::~CncLargeScaledListCtrl() {
///////////////////////////////////////////////////////////////////
	clear();
	
	if ( dummyRow != NULL )
		delete dummyRow;
}
///////////////////////////////////////////////////////////////////
bool CncLargeScaledListCtrl::setLastSelection(long item) {
///////////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return false;
		
	lastSelection = item;
	return true;
}
///////////////////////////////////////////////////////////////////
CncColumContainer* CncLargeScaledListCtrl::createDummyRow(long item) {
///////////////////////////////////////////////////////////////////
	if ( dummyRow != NULL )
		delete dummyRow;
	
	dummyRow = new CncColumContainer(GetColumnCount());
	for ( long i=0; i<GetColumnCount(); i++)
		dummyRow->updateItem(i, wxString::Format("!%ld.%ld", item, i));
		
	return dummyRow;
}
///////////////////////////////////////////////////////////////////
bool CncLargeScaledListCtrl::clear() {
///////////////////////////////////////////////////////////////////
	lastSelection = wxNOT_FOUND;
	
	rows.clear();
	return DeleteAllItems();
}
///////////////////////////////////////////////////////////////////
bool CncLargeScaledListCtrl::appendItem(const CncColumContainer& cc ) {
///////////////////////////////////////////////////////////////////
	if ( cc.getCount() != (unsigned int)GetColumnCount() ) {
		std::cerr << "CncLargeScaledListCtrl::appendItem: Invalid ColumnContainer size: " << cc.getCount();
		std::cerr << ", required: " << GetColumnCount() << std::endl;
		return false;
	}
		
	if ( rows.size() == LONG_MAX - 1 ) {
		std::cerr << "CncLargeScaledListCtrl::appendItem: Max size reached! Append was rejected" << std::endl;
		return false;
	}
		
	rows.push_back(cc);
	SetItemCount(rows.size());
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncLargeScaledListCtrl::appendItems(unsigned int nsize, const CncColumContainer* ccp) {
///////////////////////////////////////////////////////////////////
	if ( rows.size() == (size_t)(INT_MAX - 1 - nsize)) {
		std::cerr << "CncLargeScaledListCtrl::appendItems: Max size reached! Append was rejected" << std::endl;
		return false;
	}
	
	// over all containers
	for ( unsigned int i=0; i<nsize; i++ ) {
		if ( ccp[i].getCount() != (unsigned int)GetColumnCount() ) {
			std::cerr << "CncLargeScaledListCtrl::appendItems: Invalid ColumnContainer size: " << ccp[i].getCount();
			std::cerr << ", required: " << GetColumnCount() << std::endl;
			return false;
		}
		
		rows.push_back(ccp[i]);
	}
	
	SetItemCount(rows.size());
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncLargeScaledListCtrl::updateItem(long item, const CncColumContainer& cc) {
///////////////////////////////////////////////////////////////////
	if ( cc.getCount() != (unsigned int)GetColumnCount() ) {
		std::cerr << "CncLargeScaledListCtrl::updateItem: Invalid ColumnContainer size: " << cc.getCount();
		std::cerr << ", required: " << GetColumnCount() << std::endl;
		return false;
	}
	
	if ( item > (long)rows.size() - 1 || item < 0L) {
		std::cerr << "CncLargeScaledListCtrl::updateItem: item " << item << " out of bound. Update was rejected" << std::endl;
		return false;
	}
		
	rows[item] = cc;
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncLargeScaledListCtrl::updateItem(long item, long column, const wxString& value) {
///////////////////////////////////////////////////////////////////
	if ( column < 0L || column > GetColumnCount() -1 ) {
		std::cerr << "CncLargeScaledListCtrl::updateItem: Invalid column index: " << column;
		std::cerr << ", max: " << GetColumnCount() << std::endl;
		return false;
	}
	
	if ( item < 0L || item > (long)rows.size() - 1 ) {
		std::cerr << "CncLargeScaledListCtrl::updateItem: item " << item << " out of bound. Update was rejected" << std::endl;
		return false;
	}
		
	rows[item].updateItem(column, value);
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncLargeScaledListCtrl::isItemValid(long item) const {
///////////////////////////////////////////////////////////////////
	long size = rows.size();

	// out of item range
	if ( item < 0 )										return false;
		
	// to get from here on a null based index
	size -= 1;
		
	// out of item 
	if ( item > size )									return false;

	return true;
}
///////////////////////////////////////////////////////////////////
bool CncLargeScaledListCtrl::isColumnValid(long column) const {
///////////////////////////////////////////////////////////////////
	if ( GetColumnCount() == 0 )
		return false;
		
	if ( column < 0 || column > GetColumnCount() -1 )
		return false;
		
	return true;
}
///////////////////////////////////////////////////////////////////
void CncLargeScaledListCtrl::deselectAll() {
///////////////////////////////////////////////////////////////////
	SetItemState(lastSelection, 0, wxLIST_STATE_SELECTED);
	lastSelection = wxNOT_FOUND;
}
///////////////////////////////////////////////////////////////////
bool CncLargeScaledListCtrl::selectItem(long item, bool ensureVisible) {
///////////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return false;
		
	// deselect
	SetItemState(lastSelection, 0, wxLIST_STATE_SELECTED);
	
	// select
	SetItemState(item, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	lastSelection = item;
		
	if ( ensureVisible == true )
		EnsureVisible(item);
		
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncLargeScaledListCtrl::goForwardUnitlColumnChangeIntern(long item, long column) {
///////////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return false;
		
	if ( isColumnValid(column) == false )
		return false;
		
	// get the reference item
	wxString ref(getItemText(item, column));
	long count = listType == CncLargeScaledListCtrl::ListType::NORMAL ? item : translateItem(item);
	
	for (auto it = rows.begin() + count; it < rows.end(); ++it ) {
		if ( it->getItem(column) != ref ) {
			
			if ( listType == CncLargeScaledListCtrl::ListType::NORMAL )
				selectItem(count);
			else
				selectItem(translateItem(count));
				
			return true;
		}
		count++;
	}
	
	return false;
}
///////////////////////////////////////////////////////////////////
bool CncLargeScaledListCtrl::goBackwardUnitlColumnChangeIntern(long item, long column) {
///////////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return false;
		
	if ( isColumnValid(column) == false )
		return false;
		
	// get the reference item
	wxString ref(getItemText(item, column));
	long count = listType == CncLargeScaledListCtrl::ListType::NORMAL ? translateItem(item) : item;

	for (auto it = rows.rbegin() + count; it < rows.rend(); ++it ) {
		if ( it->getItem(column) != ref ) {

			if ( listType == CncLargeScaledListCtrl::ListType::NORMAL )
				selectItem(translateItem(count));
			else
				selectItem(count);
			
			return true;
		}
		count++;
	}
	
	return false;
}
///////////////////////////////////////////////////////////////////
bool CncLargeScaledListCtrl::goForwardUnitlColumnChange(long item, long column) {
///////////////////////////////////////////////////////////////////
	if ( listType == CncLargeScaledListCtrl::ListType::REVERSE )
		return goBackwardUnitlColumnChangeIntern(item, column);
		
	return goForwardUnitlColumnChangeIntern(item, column);
}
///////////////////////////////////////////////////////////////////
bool CncLargeScaledListCtrl::goBackwardUnitlColumnChange(long item, long column) {
///////////////////////////////////////////////////////////////////
	if ( listType == CncLargeScaledListCtrl::ListType::REVERSE )
		return goForwardUnitlColumnChangeIntern(item, column);
		
	return goBackwardUnitlColumnChangeIntern(item, column);
}
///////////////////////////////////////////////////////////////////
const CncColumContainer& CncLargeScaledListCtrl::getRow(long item) {
///////////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return *createDummyRow(item);

	return rows[translateItem(item)];
}
///////////////////////////////////////////////////////////////////
long CncLargeScaledListCtrl::translateItem(long item) const {
///////////////////////////////////////////////////////////////////
	if ( listType == CncLargeScaledListCtrl::ListType::REVERSE ) {
		if ( isItemValid(item) == false ) {
			std::cerr << "CncLargeScaledListCtrl::translateItem: Invalid index: " << item;
			std::cerr << ", Curren size: " << rows.size() << std::endl;
			return item;
		}
		
		return ( rows.size() -1 ) - item;
	}
	
	return item;
}
///////////////////////////////////////////////////////////////////
wxString CncLargeScaledListCtrl::getItemText(long item, long column) const {
///////////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return _(wxString::Format("!%ld.%ld", item, column));
		
	// out of column range
	if ( isColumnValid(column) == false )
		return _("");
		
	return rows[translateItem(item)].getItem(column);
}
///////////////////////////////////////////////////////////////////
wxString CncLargeScaledListCtrl::OnGetItemText(long item, long column) const {
///////////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return _(wxString::Format("!%ld.%ld", item, column));
		
	// out of column range
	if ( isColumnValid(column) == false )
		return _("");
		
	return rows[translateItem(item)].getItem(column);
}
///////////////////////////////////////////////////////////////////
int CncLargeScaledListCtrl::OnGetItemColumnImage(long item, long column) const {
///////////////////////////////////////////////////////////////////
	// default behaviour
	return -1;
}
///////////////////////////////////////////////////////////////////
wxListItemAttr* CncLargeScaledListCtrl::OnGetItemAttr(long item) const {
///////////////////////////////////////////////////////////////////
	// default behaviour
	return NULL;
}
///////////////////////////////////////////////////////////////////
wxListItemAttr* CncLargeScaledListCtrl::OnGetItemColumnAttr(long item, long column) const {
///////////////////////////////////////////////////////////////////
	// default behaviour
	return OnGetItemAttr(item);
}
///////////////////////////////////////////////////////////////////
long CncLargeScaledListCtrl::searchRow(const wxString& what, int searchColumn) {
///////////////////////////////////////////////////////////////////
	long ret     = -1;
	long counter = 0;
	for( auto it = rows.begin(); it != rows.end(); ++it ) {
		
		if ( it->getItem(searchColumn) == what ) {
			ret = translateItem(counter);
			break;
		}
		
		counter++;
	}
	
	return ret;
}



