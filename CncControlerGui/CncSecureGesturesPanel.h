#ifndef CNC_SECURE_GESTURES_PANEL_H
#define CNC_SECURE_GESTURES_PANEL_H

#include "wx/wx.h"

class CncSecureGesturesPanel : public wxPanel
{
	private:
		
		static const int border					=  4;
		static const int defaultCallbackId		= -1;
		static const int updateTimerId			= 10;
		static const int observerTimerId		= 20;
		
	public:
		
		enum State	{ S_INACTIVE, S_STARTING, S_CONTINUING };
		enum Type	{ T_SWITCH, T_BUTTON };
		enum Mode	{ M_NEGATIVE, M_POSITIVE, M_BOTH };

		class CallbackInterface;
		typedef CallbackInterface CBI;
		
		struct Data 
		{
			wxDateTime tsLast	= wxDateTime::Now();
			
			int cbId			= defaultCallbackId;
			
			int xPos			= 0;
			int yPos			= 0;
			
			int xVal			= 0;
			int yVal			= 0;
			
			int xMax			= 0;
			int yMax			= 0;
			
			int range			= 0;
			
			double radius		= 0.0;
			double ratio		= 0.0;
			double angle		= 0.0;
			
			bool isTimerChanged = false;
			bool isRangeChanged	= false;
			bool isAngleChanged	= false;
			
			bool isChanged()	const	{ return (isTimerChanged || isRangeChanged || isAngleChanged); }
			bool isZero()		const	{ return range == 0; }
			void reset()				{ *this = Data(); }
			
			friend std::ostream &operator<< (std::ostream &ostr, const Data &d) 
			{
				ostr << wxString::Format("ID: %2d ",			d.cbId);
				ostr << wxString::Format("Pos: (%4d,%4d); ",	d.xPos, d.yPos);
				ostr << wxString::Format("Val: (%4d,%4d); ",	d.xVal, d.yVal);
				ostr << wxString::Format("Max: (%4d,%4d); ",	d.xMax, d.yMax);
				ostr << wxString::Format("Range: (%2d); ",		d.range);
				ostr << wxString::Format("Ratio: (%.1lf); ",	d.ratio);
				ostr << wxString::Format("Radius: (%.1lf); ",	d.radius);
				ostr << wxString::Format("Angle: (%.6lf); ",	d.angle);
				
				return ostr;
			}
		};

		class CallbackInterface {
			
			public:
				
				CallbackInterface() {}
				virtual ~CallbackInterface() {}
				
				virtual void notifyStarting(const CncSecureGesturesPanel::State s) {}
				virtual void notifyPositionChanged(const CncSecureGesturesPanel::Data& d) {}
				virtual void notifyPositionHeld(const CncSecureGesturesPanel::Data& d) {}
		};
		
		CncSecureGesturesPanel(wxWindow* parent, wxOrientation o = wxBOTH, Type t = T_BUTTON, Mode m = M_BOTH, int sensitivity = 3);
		~CncSecureGesturesPanel();
		
		void init()										{ reset(); }
		void update()									{ calculateDimensions(); applyPosChange(false); }
		
		void setCenterBitmap(const wxBitmap& bmp)		{ centreBmp = bmp; }
		void setType(const Type& t )					{ type = t; }
		void setMode(const Mode& m )					{ mode = m; }
		void setOrientation(const wxOrientation& o)		{ orientation = o; }
		void setSensitivity(int s)						{ sensitivity = abs(s); }
		
		void setCallbackId(int id)						{ callbackId = id; }
		void setCallbackInterface(CallbackInterface* inf, int id = defaultCallbackId);
		
		int getCallbackId() const { return callbackId; }
		
	protected:
		
		void onPaint(wxPaintEvent& event);
		void onSize(wxSizeEvent& event);
		void onMouse(wxMouseEvent& event);
		void onLeave(wxMouseEvent& event);
		void onTimer(wxTimerEvent& event);
			
		void onPan(wxPanGestureEvent& event);
		void onZoom(wxZoomGestureEvent& event);
		void onRotate(wxRotateGestureEvent& event);
		void onTwoFingerTap(wxTwoFingerTapEvent& event);
		void onLongPress(wxLongPressEvent& event);
		void onPressAndTap(wxPressAndTapEvent& event);
		
	private:
		
		CallbackInterface*	caller;
		int					callbackId;
		
		State				state;
		
		wxTimer*			updateTimer;
		wxTimer*			observerTimer;
		
		wxDateTime			observerTs;
		
		wxOrientation		orientation;
		Type				type;
		Mode				mode;
		int					sensitivity;
		
		wxBitmap			centreBmp;
		wxPoint				centrePt;
		wxPoint				zeroPt;
		wxPoint				currentPt;
		wxRect				innerRect;
		wxRect				leftRect;
		wxRect				rightRect;
		wxRect				topRect;
		wxRect				bottomRect;
		int					regionLen;
		int					totalLen;
		wxSize				knobSize;
		wxColour			knobColour;
		wxColour			lineColour;
		wxColour			innerColour;
		bool				mouseDown;
		
		Data				lastData;
		
		wxPoint2DDouble		m_translateDistance;
		wxAffineMatrix2D	m_affineMatrix;
		double				m_lastZoomFactor;
		double				m_lastRotationAngle;
		
		void	reset();
		void	applyPosChange(bool useTimer);
		void	applyPosHeld();
		
		void	calculateZero();
		void	calculateDimensions();
		void	calculateCoordinates();
		
		void	startTimer();
		void	stopTimer();
		
		void	recalculate();
		
		CncSecureGesturesPanel::State	skipState();
};

#endif