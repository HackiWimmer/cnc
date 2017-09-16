#ifndef GCODE_PATH_H
#define GCODE_PATH_H

#include "GCodeBlock.h"

class GCodePath {
	
	private:
		bool linearMoveExists;
		wxString path;
		
		/////////////////////////////////////////////////////
		bool addPathFragment(const char* fragment) {
			wxString s(fragment);
			return addPathFragment(s);
		}
		/////////////////////////////////////////////////////
		bool addPathFragment(wxString& fragment) {
			if ( path.IsEmpty() == false )
				path << " ";
				
			path << fragment;
			return true;
		}
		
	public:
		/////////////////////////////////////////////////////
		GCodePath()
		: linearMoveExists(false)
		, path("")
		{}
		/////////////////////////////////////////////////////
		~GCodePath() {
		}
		/////////////////////////////////////////////////////
		const unsigned int available() const {
			return path.length();
		}
		/////////////////////////////////////////////////////
		const bool hasLinearMove() const {
			return linearMoveExists;
		}
		/////////////////////////////////////////////////////
		void initNextPath() {
			path.clear();
			linearMoveExists = false;
		}
		/////////////////////////////////////////////////////
		const wxString& getPath() {
			return path;
		}
		/////////////////////////////////////////////////////
		bool addGCB_AsLinearMove(CncDoublePosition& curPxPos, GCodeBlock& gcb) {
			
			//todo
			if ( gcb.hasZ() )
				//clog << gcb.block << endl;
			
			if ( gcb.hasOneOf_XY() == false )
				return true;
				
			linearMoveExists = true;
				
			char cmd  = gcb.isAbsolutePositioningXYZ() ? 'L' : 'l';
			double mx = curPxPos.getX();
			double my = curPxPos.getY();
			double x  = 0.0d;
			double y  = 0.0d;
			
			if ( gcb.hasX() ) 
				x = gcb.getXAsPx();

			if ( gcb.hasY() )
				y = gcb.getYAsPx();
				
			bool ret = false;
			if ( available() > 0 ) 
				ret = addPathFragment(wxString::Format(" %c%f,%f", cmd, x, y));
			else
				ret = addPathFragment(wxString::Format(" M%f,%f %c%f,%f", mx, my, cmd, x, y));
				
			return ret;
		}
		/////////////////////////////////////////////////////
		bool addGCB_AsClockwiseArcMove(CncDoublePosition& curPxPos, GCodeBlock& gcb) {
			return addGCB_AsArcMove(curPxPos, gcb, false);
		}
		/////////////////////////////////////////////////////
		bool addGCB_AsCounterClockwiseArcMove(CncDoublePosition& curPxPos, GCodeBlock& gcb) {
			return addGCB_AsArcMove(curPxPos, gcb, true);
		}
		/////////////////////////////////////////////////////
		bool addGCB_AsArcMove(CncDoublePosition& curPxPos, GCodeBlock& gcb, bool sweep) {
			if ( hasLinearMove() == false )
				path.clear();
				
			linearMoveExists = true;
			
			double mx = curPxPos.getX();
			double my = curPxPos.getY();
			double r  = gcb.getAsPx(sqrt(pow(gcb.i, 2) + pow(gcb.j, 2))); // todo relative vs. absolute
			double ex = 0.0d;
			double ey = 0.0d;
			char cmd  ='\0';
			
			if ( gcb.isAbsolutePositioningXYZ() ) {
				cmd = 'A';
				ex = gcb.getXAsPx();
				ey = gcb.getYAsPx();
			} else {
				cmd = 'a';
				ex = curPxPos.getX() + gcb.getXAsPx();
				ey = curPxPos.getY() + gcb.getYAsPx();
			}
			
			bool ret = false;
			if ( available() > 0 ) 
				ret = addPathFragment(wxString::Format(" %c%f,%f 0 0 %d %f,%f", cmd, r, r, (int)sweep, ex, ey));
			else
				ret = addPathFragment(wxString::Format(" M%f,%f %c%f,%f 0 0 %d %f,%f", mx, my, cmd, r, r, (int)sweep, ex, ey));
				
			return ret;
		}
		//.....

};

#endif