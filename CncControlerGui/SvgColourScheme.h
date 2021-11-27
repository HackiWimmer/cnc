#ifndef SVG_COLOUR_SCHEME
#define SVG_COLOUR_SCHEME

#include <iostream>
#include <wx/gdicmn.h>
#include <wx/colour.h>

// ----------------------------------------------------------------------------
class SvgColourDecoder {
	
	private:
		wxColour	currentTest;
		
	public:
		SvgColourDecoder();
		explicit SvgColourDecoder(const wxColour& col);
		
		~SvgColourDecoder();
		
		const wxColour& getColour()	const { return currentTest; }
		
		bool valid() 				const { return currentTest.IsOk(); }
		unsigned int getRed()		const { return currentTest.Red(); }
		unsigned int getGreen()		const { return currentTest.Green(); }
		unsigned int getBlue()		const { return currentTest.Blue(); }
		
		bool matchesWhite() 		const;
		bool matchesBlack()			const;
		bool matchesRed()			const; 
		bool matchesBlue()			const; 
		bool matchesGray()			const; 
		
		bool setColour(const wxColour& col);
		bool setColour(const wxString& str);
		
		
		void trace(std::ostream &ostr) const;
		friend std::ostream &operator<< (std::ostream &ostr, const SvgColourDecoder &cs) {
			cs.trace(ostr);
			return ostr;
		}

};

// ----------------------------------------------------------------------------
class SvgStyleAttributeDecoder {
	
	protected:
		virtual bool notifyNextParameter(const wxString& key, const wxString& val) = 0;
		
	public:
		SvgStyleAttributeDecoder();
		virtual ~SvgStyleAttributeDecoder();
		
		unsigned int decode(const wxString& attrStr);
};

// ----------------------------------------------------------------------------
class SvgColourAttributeDecoder : public SvgStyleAttributeDecoder {
	
	private:
		bool				fillFound;
		bool				strokeFound;
		wxColour			fillCol;
		wxColour			strokeCol;
		wxColour			defFillCol;
		wxColour			defStrokeCol;
		
		SvgColourDecoder	decoder;
		
	protected:
		virtual bool notifyNextParameter(const wxString& key, const wxString& val);
		
	public:
		SvgColourAttributeDecoder()
		: SvgStyleAttributeDecoder()
		, fillFound		(false)
		, strokeFound	(false)
		, fillCol		(wxNullColour)
		, strokeCol		(wxNullColour)
		, defFillCol	(wxNullColour)
		, defStrokeCol	(wxNullColour)
		, decoder		()
		{}
		
		virtual ~SvgColourAttributeDecoder()
		{}
		
		void reset();
		
		const wxColour& getFillColour()		const { return fillCol; }
		const wxColour& getStrokeColour()	const { return strokeCol; }
		
		void setDefaultFillColour(const wxColour& col)		{ defFillCol   = col; fillCol   = col; }
		void setDefaultStrokeColour(const wxColour& col)	{ defStrokeCol = col; strokeCol = col;}
};

#endif