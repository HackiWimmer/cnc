
#include "GamepadEvent.h"

///////////////////////////////////////////////////////////////////
std::ostream& GamepadEvent::Data::trace(std::ostream &ostr, const GamepadEvent::Data &data) {
///////////////////////////////////////////////////////////////////
	ostr << "Button state(A;B;X;Y)          : " 
		<< data.buttonA 				<< "; " 
		<< data.buttonB 				<< "; " 
		<< data.buttonX 				<< "; " 
		<< data.buttonY 
		<< std::endl;
		
	ostr << "Button state(L;R;U;D)          : " 
		<< data.buttonLeft				<< "; " 
		<< data.buttonRight 			<< "; " 
		<< data.buttonUp 				<< "; " 
		<< data.buttonDown
		<< std::endl; 
		
	ostr << "Button (Start;Back)            : " 
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
			
	return ostr;
}
