#ifndef CNC_FLOAT_VALIDATOR_H
#define CNC_FLOAT_VALIDATOR_H

#include <wx/event.h>
#include <wx/utils.h> 
#include <wx/textentry.h>
#include <wx/valnum.h>

class CncFloatingPointValidator : public wxFloatingPointValidator<float>  {
	
	public:
		CncFloatingPointValidator(int precision, float *value = NULL, int style = wxNUM_VAL_DEFAULT)
		: wxFloatingPointValidator( precision, value, style)
		{}
		
		~CncFloatingPointValidator()
		{}
};

#endif