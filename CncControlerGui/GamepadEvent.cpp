
#include "GamepadEvent.h"

///////////////////////////////////////////////////////////////////
std::ostream& GamepadEvent::Data::trace(std::ostream &ostr, const GamepadEvent::Data &data) {
///////////////////////////////////////////////////////////////////
	ostr << "Button      state(A;B;X;Y)     : " 
		 << data.buttonA 				<< "; " 
		 << data.buttonB 				<< "; " 
		 << data.buttonX 				<< "; " 
		 << data.buttonY 
		 << std::endl;
		
	ostr << "Button      state(L;R;U;D)     : " 
		 << data.buttonLeft				<< "; " 
		 << data.buttonRight 			<< "; " 
		 << data.buttonUp 				<< "; " 
		 << data.buttonDown
		 << std::endl; 
		
	ostr << "Button      (Start;Back)       : " 
		 << data.buttonStart	<< "; " 
		 << data.buttonBack 
		 << std::endl; 
		
	ostr << "Stick       Button state(L;R)  : " 
		 << data.buttonLeftStick	<< "; " 
		 << data.buttonRightStick 
		 << std::endl; 
		
	ostr << "Shoulder    Button state(L;R)  : " 
		 << data.buttonLeftShoulder	<< "; " 
		 << data.buttonRightShoulder 
		 << std::endl; 
		
	ostr << "Trigger     state(L;R)         : " 
		 << wxString::Format("%1.3lf; ", data.leftTrigger)
		 << wxString::Format("%1.3lf",   data.rightTrigger)
		 << std::endl; 
		
	ostr << "Left Stick  state(X;Y)         : " 
		 << wxString::Format("%+1.1lf; ", data.leftStickX)
		 << wxString::Format("%+1.1lf",   data.leftStickY)
		 << std::endl; 
			
	ostr << "Right Stick state(X;Y)         : " 
		 << wxString::Format("%+1.1lf; ", data.rightStickX)
		 << wxString::Format("%+1.1lf",   data.rightStickY)
		 << std::endl; 
			
	ostr << std::endl; 

	ostr << "Evaluated Position Management\n"
		 << "  (Sticks, NaviXY, NaviZ)      : "; 
			switch ( data.posCtrlMode ) {
				case PCM_STICKS:	ostr << "Sticks";	break;
				case PCM_NAV_XY:	ostr << "Navi XZ";	break;
				case PCM_NAV_Z:		ostr << "Navi  Z";	break;
			}
	ostr << std::endl; 

	ostr << "Evaluated Movement(X;Y;Z)      : " 
		 << wxString::Format("%d; ", data.dx)
		 << wxString::Format("%d; ", data.dy)
		 << wxString::Format("%d",   data.dz)
		 << std::endl; 
		
	ostr << " Has empty Movement            : " 
		 << data.hasEmptyMovement
		 << std::endl; 

	ostr << " Is any Stick active           : " 
		 << data.isAnyStickActive
		 << std::endl; 
		 
	ostr << " Is left Stick active          : " 
		 << data.isLeftStickActive
		 << std::endl; 
		 
	ostr << " Is right Stick active         : " 
		 << data.isRightStickActive
		 << std::endl; 
		 
	ostr << " Is navigator active           : " 
		 << data.isNaviButtonActive
		 << std::endl; 
		
	return ostr;
}
