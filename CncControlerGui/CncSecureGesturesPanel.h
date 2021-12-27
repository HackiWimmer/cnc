#ifndef CNC_SECURE_GESTURES_PANEL_H
#define CNC_SECURE_GESTURES_PANEL_H

#include "wx/wx.h"
#include "CncCommon.h"

// ----------------------------------------------------------------------------
#define CNC_SECURE_GESTURES_PANEL_DEFAUTL_CALLBACK_ID -1

// ----------------------------------------------------------------------------

template <class T> 
class CncRangeTranslator {
	
	private:
		T				min;
		T				max;
		T				rng;
		CncEdge			edge;
		unsigned int	len;
		
		// ------------------------------------------------------------
		T shift(T value)
		{
			return value - min;
		}
		
	public:
		
		const static int errorValueBase = -420000;
		
		CncRangeTranslator(T a, T b)
		: CncRangeTranslator(a, b, cncLeft, abs(b - a))
		{}

		CncRangeTranslator(T a, T b, CncEdge e)
		: CncRangeTranslator(a, b, e, abs(b - a))
		{}
		
		CncRangeTranslator(T a, T b, CncEdge e, unsigned int l)
		: min	(a)
		, max	(b)
		, rng	(T(0))
		, edge	(e)
		, len	(l)
		{
			if ( min < max ) 
				rng = max - min;
				
			// else rng = T(0)
		}
		
		T getMin()						const { return min; }
		T getMax()						const { return max; }
		T getRange()					const { return rng; }
		
		bool good()						const { return ( rng != T(0) ); }
		bool isInRange(T value)			const { return ( value >= min && value <= max ); }
		
		static
		bool isErrorValue(int value)		  { return ( value <  0 ); }
		
		// ------------------------------------------------------------
		char getEdgeAsChar(CncEdge edge) const 
		{
			switch ( edge )
			{
				case cncLeft:	return 'L';
				case cncTop:	return 'T'; 
				case cncRight:	return 'R';
				case cncBottom:	return 'B';
				case cncCenter:	return 'C';
			}
			
			return '?';
		}
		
		// ------------------------------------------------------------
		friend std::ostream &operator<< (std::ostream &ostr, const CncRangeTranslator &t) 
		{
			ostr	<< "CncRangeTranslator[good=" << t.good() << "]"
					<< " min=" << t.min
					<< " max=" << t.max
					<< " rng=" << t.rng
					<< " len=" << t.len
					<< " edg=" << t.getEdgeAsChar(t.edge)
			;
			
			return ostr;
		}
		
		// ------------------------------------------------------------
		static float ratioFromIndex(unsigned int index, unsigned int totCnt)
		{
			if ( totCnt < index )
			{
				std::cerr << CNC_LOG_FUNCT <<  ": Invalid index:" << index << " tot= " << totCnt << std::endl;
				return 0.0f;
			}
				
			const float distance = 1.0 / totCnt;
			
			// +0.5 to reach approximately the middle of the region
			return ( index + 0.5 ) * distance;
		}
		
		// ------------------------------------------------------------
		static int indexFromRatio(float ratio, unsigned int totCnt)
		{
			if ( totCnt <= 0 )
			{
				std::cerr << CNC_LOG_FUNCT <<  ": Invalid total count:" << totCnt << std::endl;
				return -1;
			}
			
			if ( ratio < 0.0f )
			{
				std::cerr << CNC_LOG_FUNCT <<  ": Invalid ratio:" << ratio << std::endl;
				return -1;
			}
			
			const float distance = 1.0 / totCnt;
			
			if ( ratio < distance )
				return 0;
				
			for ( unsigned int i = 1; i <= totCnt; i++)
			{
				if ( distance * i >= ratio )
					return i - 1;
			}
			
			std::cerr << CNC_LOG_FUNCT <<  ": Cant determine index! ratio:" << ratio << std::endl;
			return -1;
		}
		
		// ------------------------------------------------------------
		T valueFromRatio(float ratio)
		{
			T value = T(0);
			if ( good() )
			{
				if ( ratio < 0.0 )
				{
					if ( min < T(0) )
						value = min + ( abs(rng * ratio) );
				}
				else if ( ratio >= 0.0 )
				{
					if ( max > T(0) ) 
						value = min + ( abs(rng * ratio) );
						
					// consider special centre behaviour 
					if ( cnc::fltCmp::nu(ratio) && edge == cncCenter )
						value = T(0);
				}
			}
			
			return value;
		}
		
		// ------------------------------------------------------------
		float ratioFromValue(T value)
		{
			float ratio = 0.0;
			if ( good() )
			{
				if ( isInRange(value) == false )
				{
					std::cerr << CNC_LOG_FUNCT <<  ": Value out of range:" << min << " < " << value << " > " << max << std::endl;
				}
				else
				{
					if ( value < T(0) )
					{
						if ( min < T(0) )
							ratio = float(value - min) / abs(rng);
					}
					else if ( value >= T(0) )
					{
						if ( max > T(0) )
							ratio = float(value - min) / abs(rng);
						
						// consider special centre behaviour 
						if ( value == T(0) && edge == cncCenter )
							ratio = 0.0;
					}
				}
			}
			
			return ratio;
		}
		
		// ------------------------------------------------------------
		int calcByRatio(float ratio)
		{
			if ( good() == false )
				return errorValueBase - 1;
				
			return calcByValue(valueFromRatio(ratio));
		}
		
		// ------------------------------------------------------------
		int calcByValue(T value)
		{
			if ( good() == false )
				return errorValueBase - 1;
			
			if ( isInRange(value) == false )
			{
				std::cerr << CNC_LOG_FUNCT <<  ": Value out of range:" << min << " < " << value << " > " << max << std::endl;
				return errorValueBase - 2;
			}
				
			const T maxS = shift(max);
			const T valS = shift(value);
			
			int ret = wxRound(double(valS * len ) / maxS);
			
			switch ( edge )
			{
				case cncLeft:
				case cncTop:
				case cncCenter:
					break;
					
				case cncRight:
				case cncBottom:
					ret = len - ret;
					break;
			}
			
			return ret;
		}
};

// ----------------------------------------------------------------------------
class CncSecureGesturesPanelEvent;
wxDECLARE_EVENT(wxEVT_CNC_SECURE_GESTURES_PANEL, CncSecureGesturesPanelEvent);

// ----------------------------------------------------------------------------
class CncSecureGesturesPanelEvent : public wxCommandEvent {
	
	public:
		
		enum Id {
			CSGP_STARTING 		= 100,
			CSGP_POS_CHANGED	= 200, 
			CSGP_POS_HELD		= 201
		};
		
		static const char* getEventIdAsString(int id)
		{
			return getEventIdAsString(Id(id));
		}
		
		static const char* getEventIdAsString(Id id) 
		{
			switch ( id ) {
				case CSGP_STARTING:				return "CSGP_STARTING";
				case CSGP_POS_CHANGED:			return "CSGP_POS_CHANGED";
				case CSGP_POS_HELD:				return "CSGP_POS_HELD";
			}
			
			return "CSGP_???";
		}
		
		struct Data 
		{
			wxDateTime tsLast	= wxDateTime::Now();
			
			int cbId			= CNC_SECURE_GESTURES_PANEL_DEFAUTL_CALLBACK_ID;
			
			bool userTriggered	= false;
			
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
			bool isRatioChanged	= false;
			bool isAngleChanged	= false;
			
			bool isChanged()	const	{ return (isTimerChanged || isRangeChanged || isRatioChanged || isAngleChanged); }
			bool isZero()		const	{ return range == 0; }
			
			/*
			void reset(bool deep = false)
			{
				const Data prev = *this;
				*this = Data();
				
				if ( deep == false )
				{
					cbId = prev.cbId;
					
					isRangeChanged = prev.range != range;
					isRatioChanged = cnc::dblCmp::eq(prev.ratio, ratio) == false;
					isAngleChanged = cnc::dblCmp::eq(prev.angle, angle) == false;
				}
			}
			*/
			
			friend std::ostream &operator<< (std::ostream &ostr, const Data &d) 
			{
				ostr << wxString::Format("CID: %2d ",			d.cbId);
				ostr << wxString::Format("User: %d ",			d.userTriggered);
				ostr << wxString::Format("RAST: [%d%d%d%d] ",	d.isRatioChanged, d.isAngleChanged, d.isRangeChanged, d.isTimerChanged);
				ostr << wxString::Format("Pos: (%4d,%4d); ",	d.xPos, d.yPos);
				ostr << wxString::Format("Val: (%4d,%4d); ",	d.xVal, d.yVal);
				ostr << wxString::Format("Max: (%4d,%4d); ",	d.xMax, d.yMax);
				ostr << wxString::Format("Range: (%2d); ",		d.range);
				ostr << wxString::Format("Ratio: (%.1lf); ",	d.ratio);
				ostr << wxString::Format("Radius: (%6.1lf); ",	d.radius);
				ostr << wxString::Format("Angle: (%8.6lf); ",	d.angle);
				
				return ostr;
			}
		};
		
		CncSecureGesturesPanelEvent(int id = 0) 
		: wxCommandEvent(wxEVT_CNC_SECURE_GESTURES_PANEL, id)
		, data			()
		{}
		
		
		explicit CncSecureGesturesPanelEvent(const CncSecureGesturesPanelEvent& event) 
		: wxCommandEvent(event)
		, data			(event.data)
		{}
	
		virtual ~CncSecureGesturesPanelEvent() 
		{}
		
		virtual wxEvent *Clone() const 
		{
			return new CncSecureGesturesPanelEvent(*this);
		}
		
		friend std::ostream &operator<< (std::ostream &ostr, const CncSecureGesturesPanelEvent &e) 
		{
			const wxString context(CncSecureGesturesPanelEvent::getEventIdAsString(e.GetId()));
			
			ostr	<< "[" << context << "]" << wxString(20 - context.length() , ' ') << ": "
					<< e.data;
			
			return ostr;
		}
		
		Data data;
};

typedef CncSecureGesturesPanelEvent::Data GestureData;

// ----------------------------------------------------------------------------
class CncSecureGesturesPanel : public wxPanel
{
	public:
		
		static const int defaultCallbackId		= CNC_SECURE_GESTURES_PANEL_DEFAUTL_CALLBACK_ID;
		static const int border					=  4;
		static const int updateTimerId			= 10;
		static const int observerTimerId		= 20;
		
		enum State	{ S_INACTIVE, S_STARTING, S_CONTINUING };
		enum Type	{ T_SWITCH, T_BUTTON };
		enum Mode	{ M_NEGATIVE, M_POSITIVE, M_BOTH };
		
		CncSecureGesturesPanel(wxWindow* parent, wxOrientation o = wxBOTH, Type t = T_BUTTON, Mode m = M_BOTH, int sensitivity = 3);
		virtual ~CncSecureGesturesPanel();
		
		virtual bool Show(bool show = true);
		virtual bool Layout();
		virtual bool Enable(bool enable=true);
		
		void setShowRange(bool b)						{ displayRange = b; }
		void setShowRatio(bool b)						{ displayRatio = b; }
		
		void init()										{ reset(); }
		void update()									{ calculateDimensions(); applyPosChange(false); }
		
		void setCenterBitmap(const wxBitmap& bmp)		{ behaviorChanged = true; centreBmp = bmp; }
		void setType(const Type& t )					{ behaviorChanged = true; type = t; }
		void setMode(const Mode& m )					{ behaviorChanged = true; mode = m; }
		void setOrientation(const wxOrientation& o)		{ behaviorChanged = true; orientation = o; }
		void setSensitivity(int s)						{ behaviorChanged = true; sensitivity = abs(s); }
		
		int getSensitivity()					const	{ return sensitivity; }
		
		void setCallbackId(int id)						{ callbackId = id; }
		int getCallbackId()						const	{ return callbackId; }
		
		void setValueByRatio(float ratio, float angle = 0.0f);
		
		char getModeAsCharacter(Mode m) const
		{
			switch ( m )
			{
				case M_NEGATIVE:	return 'N';
				case M_POSITIVE:	return 'P';
				case M_BOTH:		return 'B';
			}
			
			return '?';
		}
		
		char getOrientationAsCharacter(wxOrientation o) const
		{
			switch ( o )
			{
				case wxHORIZONTAL:	return 'H';
				case wxVERTICAL:	return 'V';
				case wxBOTH:		return 'B';
			}
			
			return '?';
		}	protected:
		
		void onPaint(wxPaintEvent& event);
		void onSize(wxSizeEvent& event);
		void onShow(wxShowEvent& event);
		void onInitPanel(wxInitDialogEvent& event);
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
		
		typedef CncSecureGesturesPanelEvent CSGPEvent;
		
		
		int					callbackId;
		
		State				state;
		
		wxTimer*			updateTimer;
		wxTimer*			observerTimer;
		
		wxDateTime			observerTs;
		
		bool				displayRange;
		bool				displayRatio;
		
		bool				behaviorChanged; 
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
		
		CSGPEvent*			lastEvent;
		
		wxPoint2DDouble		translatedDistance;
		wxAffineMatrix2D	m_affineMatrix;
		
		void	reset();
		void	applyPosChange(bool useTimer);
		void	applyPosHeld();
		
		void	calculateDimensions();
		void	calculateCoordinates();
		
		void	startTimer();
		void	stopTimer();
		
		void	recalculate();
		
		void	trace(std::ostream& o, const wxString& context, const wxString& more=wxEmptyString);
		
		CncSecureGesturesPanelEvent&	prepareEvent(int id);
		CncSecureGesturesPanel::State	skipState();
};

#endif