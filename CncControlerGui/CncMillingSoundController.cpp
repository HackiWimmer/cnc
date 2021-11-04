#include <wx/sound.h>
#include <wx/filename.h>
#include "CncConfig.h"
#include "CncCommon.h"
#include "CncFileNameService.h"
#include "CncMillingSoundController.h"

/////////////////////////////////////////////////////////////
namespace CncSpindleSound 
{
	bool	active         = true;
	bool	playing        = false;
	double	curSpeed_U_MIN = 0.0;
};

/////////////////////////////////////////////////////////////
void CncSpindleSound::activate(bool state) {
/////////////////////////////////////////////////////////////
	CncSpindleSound::active = state;
	wxSound::Stop();
}
/////////////////////////////////////////////////////////////
void CncSpindleSound::deactivate() { 
/////////////////////////////////////////////////////////////
	CncSpindleSound::activate(false); 
}
/////////////////////////////////////////////////////////////
void CncSpindleSound::play(double speed_U_MIN) {
/////////////////////////////////////////////////////////////
	if ( CncSpindleSound::active == false )
		return;
	
	if ( CncSpindleSound::playing == false || cnc::dblCmp::eq(speed_U_MIN, curSpeed_U_MIN) == false )
	{
		const double& max = THE_CONFIG->getSpindleSpeedMax();
		
		int index = 1;
		if      ( cnc::between(speed_U_MIN,        0.0, max * 0.15) ) index = 1;
		else if ( cnc::between(speed_U_MIN, max * 0.15, max * 0.30) ) index = 2;
		else if ( cnc::between(speed_U_MIN, max * 0.30, max * 0.45) ) index = 3;
		else if ( cnc::between(speed_U_MIN, max * 0.45, max * 0.60) ) index = 4;
		else if ( cnc::between(speed_U_MIN, max * 0.60, max * 0.75) ) index = 5;
		else if ( cnc::between(speed_U_MIN, max * 0.75, max * 0.90) ) index = 6;
		else if ( cnc::between(speed_U_MIN, max * 0.90, max * 1.00) ) index = 7;
		
		const wxFileName fn(wxString::Format("%sSound%cmc%d.wav", CncFileNameService::getBaseDir(), wxFileName::GetPathSeparator(), index));
		if ( fn.Exists() ) 
		{
			wxSound(fn.GetFullPath()).Play(wxSOUND_ASYNC | wxSOUND_LOOP);
			CncSpindleSound::playing        = true;
			CncSpindleSound::curSpeed_U_MIN = speed_U_MIN;
		}
		else
		{
			std::cerr << CNC_LOG_FUNCT_A(wxString::Format(" Can't found: %s\n", fn.GetFullPath()));
		}
	}
}
/////////////////////////////////////////////////////////////
void CncSpindleSound::adjust(double speed_U_MIN) {
/////////////////////////////////////////////////////////////
	if ( CncSpindleSound::isPlaying() == false )
		return;
		
	CncSpindleSound::play(speed_U_MIN);
}
/////////////////////////////////////////////////////////////
void CncSpindleSound::stop() {
/////////////////////////////////////////////////////////////
	wxSound::Stop();
	CncSpindleSound::playing = false;
}
/////////////////////////////////////////////////////////////
bool CncSpindleSound::isPlaying() {
/////////////////////////////////////////////////////////////
	return CncSpindleSound::playing;
}
/////////////////////////////////////////////////////////////
bool CncSpindleSound::isActivated() {
/////////////////////////////////////////////////////////////
	return CncSpindleSound::active;
}
