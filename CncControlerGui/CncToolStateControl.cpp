#include "wxCrafterImages.h"
#include "CncToolStateControl.h"

//////////////////////////////////////////////////////////////////
void CncToolStateControl::setState(CncToolStateControl::State s) {
//////////////////////////////////////////////////////////////////
	currentState = neutral;
	
	if ( control == NULL )
		return;
		
	currentState = s;
	
	wxBitmap bmp;
	switch ( s ) {
		case neutral:	bmp = ImageLib24().Bitmap(trafficLightNeutral); break;
		case red:		bmp = ImageLib24().Bitmap(trafficLightRed); 	break;
		case green:		bmp = ImageLib24().Bitmap(trafficLightGreen); 	break;
		case yellow:	bmp = ImageLib24().Bitmap(trafficLightYellow); 	break;
	}
	
	control->SetBitmap(bmp);
	//control->Refresh();
	//control->Update();
}
