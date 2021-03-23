#ifndef CNC_MILLING_SOUND_H
#define CNC_MILLING_SOUND_H

namespace CncSpindleSound {
	
	void play(double speed_U_MIN = 0.0);
	void adjust(double speed_U_MIN = 0.0);
	void stop();

	void activate(bool state = true);
	void deactivate();
	
	bool isPlaying();
	bool isActivated();
};

#endif