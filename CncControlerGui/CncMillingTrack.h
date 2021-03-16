#ifndef CNC_MILLING_TRACK_H
#define CNC_MILLING_TRACK_H

#include <list>
#include "CncVector.h"

class CncMillingTrackCreator {
	
	public: 
	
		enum State { MTCS_UNKNOWN, MTCS_STARTED, MTCS_PROCESSING, MTCS_FINISHED, MTCS_ERROR };
		typedef std::list<CncFloatVector> Track;
		
	private:
		
		struct Data {
			
			CncFloatVector	pnt	= { 0.0f, 0.0f, 0.0f };
			CncFloatVector	dir	= { 0.0f, 0.0f, 0.0f };
			CncFloatVector	rd1	= { 0.0f, 0.0f, 0.0f };
			CncFloatVector	rd2	= { 0.0f, 0.0f, 0.0f };
			
			void reset() { *this = Data(); }
			
			const Data& operator= (const Data& v) {
				pnt = v.pnt;
				dir = v.dir;
				rd1 = v.rd1;
				rd2 = v.rd2;
				
				return *this;
			}

		};
		
		State					state;
		Track					track;
		size_t					posCount;
		
		CncFloatVector			first;
		Data					dp0;
		Data					dp1;
		Data					dp2;
		
		float					toolRadius;
		const CncFloatVector	topDir;
		
		bool calculate(float px, float py, float pz);
		void skip(float px, float py, float pz);
		
	public: 
		
		CncMillingTrackCreator(float toolDiameter);
		~CncMillingTrackCreator();
		
		bool start (float px, float py, float pz);
		bool next  (float px, float py, float pz);
		bool finish();
		
		void reset();
		
		const char* getStateAsStr() const;
		const Track& getMillingTrack() const;
	
};

#endif