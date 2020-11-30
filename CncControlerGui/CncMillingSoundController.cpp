#include <wx/sound.h>
#include <wx/filename.h>
#include "CncConfig.h"
#include "CncFileNameService.h"
#include "CncMillingSoundController.h"

/////////////////////////////////////////////////////////////
namespace CncMillingSound {
	
	bool active  = true;
	bool playing = false;
};

/////////////////////////////////////////////////////////////
void CncMillingSound::activate(bool state) {
/////////////////////////////////////////////////////////////
	active = state;
	wxSound::Stop();
}
/////////////////////////////////////////////////////////////
void CncMillingSound::deactivate() { 
/////////////////////////////////////////////////////////////
	activate(false); 
}
/////////////////////////////////////////////////////////////
void CncMillingSound::play(double speed_MM_MIN) {
/////////////////////////////////////////////////////////////
	if ( active == false )
		return;
	
	const double& max = THE_CONFIG->getMaxSpeedXYZ_MM_MIN();
	
	int index = 1;
	if      ( cnc::between(speed_MM_MIN,        0.0, max * 0.15) ) index = 1;
	else if ( cnc::between(speed_MM_MIN, max * 0.15, max * 0.30) ) index = 2;
	else if ( cnc::between(speed_MM_MIN, max * 0.30, max * 0.45) ) index = 3;
	else if ( cnc::between(speed_MM_MIN, max * 0.45, max * 0.60) ) index = 4;
	else if ( cnc::between(speed_MM_MIN, max * 0.60, max * 0.75) ) index = 5;
	else if ( cnc::between(speed_MM_MIN, max * 0.75, max * 0.90) ) index = 6;
	else if ( cnc::between(speed_MM_MIN, max * 0.90, max * 1.00) ) index = 7;
	
	const wxFileName fn(wxString::Format("%sSound%cmc%d.wav", CncFileNameService::getBaseDir(), wxFileName::GetPathSeparator(), index));
	if ( fn.Exists() ) {
		wxSound(fn.GetFullPath()).Play(wxSOUND_ASYNC | wxSOUND_LOOP);
		playing = true;
	}
	else {
		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(" Can't found: %s\n", fn.GetFullPath()));
	}
}
/////////////////////////////////////////////////////////////
void CncMillingSound::adjust(double speed_MM_MIN) {
/////////////////////////////////////////////////////////////
	if ( isPlaying() == false )
		return;
		
	play(speed_MM_MIN);
}
/////////////////////////////////////////////////////////////
void CncMillingSound::stop() {
/////////////////////////////////////////////////////////////
	wxSound::Stop();
	playing = false;
}
/////////////////////////////////////////////////////////////
bool CncMillingSound::isPlaying() {
/////////////////////////////////////////////////////////////
	return playing;
}
/////////////////////////////////////////////////////////////
bool CncMillingSound::isActivated() {
/////////////////////////////////////////////////////////////
	return active;
}
