#pragma once

#include <string>
#include "wx/string.h"

class CncSimpleCalculator
{
	public:
		CncSimpleCalculator();
		bool calculate(const std::string& expression, double& result);
		bool calculate(const wxString& expression, double& result);
};
