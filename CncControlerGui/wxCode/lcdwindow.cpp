#include "lcdwindow.h"
#include <iostream>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include "CncCommon.h"

#define LCD_NUMBER_SEGMENTS 8

BEGIN_EVENT_TABLE( wxLCDWindow, wxWindow )
	EVT_PAINT( wxLCDWindow::OnPaint )
	EVT_SIZE( wxLCDWindow::OnSize )
END_EVENT_TABLE()

/////////////////////////////////////////////////////////////////////////////
wxLCDWindow::wxLCDWindow( wxWindow *parent, wxPoint pos, wxSize size ) 
: wxWindow( parent, -1, pos, size )
, mSegmentLen		(40)
, mSegmentWidth		(10)
, mSpace			(5)
, mNumberDigits		(6)
, mValue			("")
, lastFloating		(DBL_MAX)
/////////////////////////////////////////////////////////////////////////////
{
	// This has to be done to use wxAutoBufferedPaintDC 
	// on EVT_PAINT events correctly
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	
	mLightColourBeforeDecimal	= *wxWHITE;
	mLightColourDecimalPlaces	= *wxWHITE;
	
	SetBackgroundColour( wxColour( 0, 0, 0 ) );
	
	mLightColour				= mLightColourBeforeDecimal;
	mGrayColour					= wxColour(  0,  64,   0);
	mGrayColour					= mGrayColour.ChangeLightness(25);
}
/////////////////////////////////////////////////////////////////////////////
wxLCDWindow::~wxLCDWindow(){
/////////////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////////////
void wxLCDWindow::OnSize( wxSizeEvent &event ) {
/////////////////////////////////////////////////////////////////////////////
	event.Skip();
	return;
}
/////////////////////////////////////////////////////////////////////////////
void wxLCDWindow::OnPaint( wxPaintEvent &event ) {
/////////////////////////////////////////////////////////////////////////////
	wxAutoBufferedPaintDC paintDC(this);
	paintDC.Clear();
	
	wxGCDC dc(paintDC);
	
	const int dw = GetClientSize().GetWidth();
	const int dh = GetClientSize().GetHeight();

	const int bw = GetBitmapWidth();
	const int bh = GetBitmapHeight();

	const double xs = ( double ) dw / bw;
	const double ys = ( double ) dh / bh;

	const double as = xs > ys? ys : xs;

	dc.SetUserScale( as, as );
	dc.SetDeviceOrigin( ( ( dw - bw * as ) / 2 ), ( ( dh - bh * as ) / 2 ) );
	
	DoDrawing( &dc );
	return;
}
/////////////////////////////////////////////////////////////////////////////
void wxLCDWindow::DoDrawing( wxDC *dc ) {
/////////////////////////////////////////////////////////////////////////////
	const int  buflen  = mValue.Len();
	int  ac            = buflen - 1;
	char current       = '\0';
	char next          = '\0';
	mLightColour       = mLightColourDecimalPlaces;
	
	for ( int c = 0; c < mNumberDigits; c++ ) {
		ReadString:
		current = ( ac >= 0 ?                    (char)mValue.GetChar( ac )     : ' ' );
		next    = ( ac >= 0 && ac < buflen - 1 ? (char)mValue.GetChar( ac + 1 ) : ' ' );

		if ( current == '.' ) {
			ac--;
			goto ReadString;
		}

		digitData.value = current;
		digitData.comma = false;
		digitData.comma = ( next == '.' );
		
		if ( digitData.comma )
			mLightColour = mLightColourBeforeDecimal;
		
		DrawDigit( dc, c, &digitData);
		ac--;
	}
	
	return ;
}
/////////////////////////////////////////////////////////////////////////////
void wxLCDWindow::DrawDigit( wxDC *dc, int digit, wxDigitData *data ) {
/////////////////////////////////////////////////////////////////////////////
	if ( data->value == ':' ) {
		DrawTwoDots( dc, digit );
		return;
	}
	
	unsigned char dec = Decode( data->value );
	for( int c = 0; c < LCD_NUMBER_SEGMENTS - 1; c++ ) 
		DrawSegment( dc, digit, c, ( dec >> c ) & 1 );
	
	DrawSegment( dc, digit, 7, data->comma );
	
	return;
}
/////////////////////////////////////////////////////////////////////////////
void wxLCDWindow::DrawTwoDots( wxDC *dc, int digit ) {
/////////////////////////////////////////////////////////////////////////////
	const int sl = mSegmentLen;
	const int sw = mSegmentWidth;
	
	int x = DigitX( digit );
	int y = DigitY( digit );

	x += ( sl / 2 ) - sw;
	y += ( sl / 2 ) - sw;

	dc->SetBrush( wxBrush( mLightColour, wxSOLID ) );
	dc->SetPen( wxPen( GetBackgroundColour(), 1, wxSOLID ) );

	dc->DrawEllipse( x, y, 2 * sw, 2 * sw );

	y += sl;

	dc->DrawEllipse( x, y, 2 * sw, 2 * sw );

	return;
}
/////////////////////////////////////////////////////////////////////////////
void wxLCDWindow::DrawSegment( wxDC *dc, int digit, int segment, bool state ) {
/////////////////////////////////////////////////////////////////////////////
	static wxPoint points[4];
	static wxPoint p6[6];
	
	const int sl = mSegmentLen;
	const int sw = mSegmentWidth;
	
	dc->SetBrush( state ? wxBrush( mLightColour, wxSOLID ) : wxBrush( mGrayColour, wxSOLID ) );
	dc->SetPen( wxPen( GetBackgroundColour(), 1, wxSOLID ) );
	
	int x = DigitX( digit );
	int y = DigitY( digit );
	
	switch( segment ) {
		case 5:
					y += 2 * sl - sw;
					points[0].x = x + sw;
					points[0].y = y;
					points[1].x = x + sl - sw;
					points[1].y = y;
					points[2].x = x + sl;
					points[2].y = y + sw;
					points[3].x = x;
					points[3].y = y + sw;
					break;
		case 4:
					y += sl;
					x += sl - sw;
					points[0].x = x;
					points[0].y = y + sw / 2;
					points[1].x = x + sw;
					points[1].y = y;
					points[2].x = x + sw;
					points[2].y = y + sl;
					points[3].x = x;
					points[3].y = y + sl - sw;
					break;	
		case 3:
					y += sl;
					points[0].x = x;
					points[0].y = y;
					points[1].x = x;
					points[1].y = y + sl;
					points[2].x = x + sw;
					points[2].y = y + sl - sw;
					points[3].x = x + sw;
					points[3].y = y + sw - sw / 2;
					break;
		case 2:
					x += sl - sw;
					points[0].x = x;
					points[0].y = y + sw;
					points[1].x = x + sw;
					points[1].y = y;
					points[2].x = x + sw;
					points[2].y = y + sl;
					points[3].x = x;
					points[3].y = y + sl - sw / 2;
					break;
		case 1:
					points[0].x = x;
					points[0].y = y;
					points[1].x = x;
					points[1].y = y + sl;
					points[2].x = x + sw;
					points[2].y = y + sl - sw / 2;
					points[3].x = x + sw;
					points[3].y = y + sw;
					break;
		case 0:
					points[0].x = x;
					points[0].y = y;
					points[1].x = x + sl;
					points[1].y = y;
					points[2].x = x + sl - sw;
					points[2].y = y + sw;
					points[3].x = x + sw;
					points[3].y = y + sw;
					break;
		case 6:
		default:	;
	}

	if( segment < 6 )
		dc->DrawPolygon( 4, points );
	
	if( segment == 6 ) {
		y += sl - sw / 2;
		
		p6[0].x = x;
		p6[0].y = y + sw / 2;
		p6[1].x = x + sw;
		p6[1].y = y;
		p6[2].x = x + sl - sw;
		p6[2].y = y;
		p6[3].x = x + sl;
		p6[3].y = y + sw / 2;
		p6[4].x = x + sl - sw;
		p6[4].y = y + sw;
		p6[5].x = x + sw;
		p6[5].y = y + sw;

		dc->DrawPolygon( 6, p6 );
	}

	if( segment == 7 ) {
		y += 2 * sl;
		x += sl;

		dc->DrawEllipse( x + 1, y - sw, sw, sw );
	}

	return ;
}


// Protected functions that calculate sizes.
// Needed by OnPaint

int wxLCDWindow::GetDigitWidth( void )
{
	return mSegmentLen + mSegmentWidth + mSpace;
}


int wxLCDWindow::GetDigitHeight( void )
{
	return ( 2 * mSegmentLen ) + ( 2 * mSpace );
}


int wxLCDWindow::GetBitmapWidth( void )
{
	return ( mNumberDigits * GetDigitWidth() ) + mSpace;
}


int wxLCDWindow::GetBitmapHeight( void )
{
	return GetDigitHeight();
}


int wxLCDWindow::DigitX( int digit )
{
	return GetBitmapWidth() - ( ( digit + 1 ) * GetDigitWidth() );
}


int wxLCDWindow::DigitY( int digit )
{
	return mSpace;
}


// Public functions accessible by the user.

void wxLCDWindow::SetNumberDigits( int ndigits )
{
	mNumberDigits = ndigits;
	Refresh( false );

	return ;
}


void wxLCDWindow::SetValue(const wxString& value)
{
	static const wxColour negCol(255, 128, 128);
	static const wxColour posCol(  0, 255,   0);
	
	mValue.assign(value);
	mValue.Replace(",", ".");
	
	double d;
	const bool b = mValue.ToDouble(&d);
	
	if ( b == false )
		mValue.assign("0");
		
	// if nothing changed no further action necessary
	if ( b == true && cnc::dblCmp::eq(lastFloating, d) )
		return;
		
	//CNC_PRINT_FUNCT_A("%lf, %lf", lastFloating, d)
		
	lastFloating = d;
	
	if ( mValue.Contains("-") ) mLightColourBeforeDecimal = negCol;
	else						mLightColourBeforeDecimal = posCol;
	
	mLightColourDecimalPlaces = mLightColourBeforeDecimal.ChangeLightness(50);
	Refresh( false );
	
	return ;
}


wxString wxLCDWindow::GetValue( void )
{
	return mValue;
}


int wxLCDWindow::GetNumberDigits( void )
{
	return mNumberDigits;
}


void wxLCDWindow::SetLightColour( wxColour c )
{
	mLightColour = c;

	return ;
}


void wxLCDWindow::SetGrayColour( wxColour c )
{
	mGrayColour = c;

	return ;
}


wxColour wxLCDWindow::GetLightColour( void )
{
	return mLightColour;
}


wxColour wxLCDWindow::GetGrayColour( void )
{
	return mGrayColour;
}


int wxLCDWindow::GetDigitsNeeded( wxString value )
{
	wxString tst = value;

	while( tst.Replace( wxT("."), wxT("") ) );

	return tst.Len();
}


// The decoder function. The heart of the wxLCDWindow.


//      ***0***
//     *       *
//     1       2
//     *       *
//      ***6***
//     *       *  
//     3       4
//     *       *
//      ***5***

// A 10
// B 11
// C 12
// D 13
// E 14
// F 15

//     8421 8421
//     -654 3210
//---------------------
// 0 : 0011.1111 = 0x3F
// 1 : 0001.0100 = 0x14
// 2 : 0110.1101 = 0x6D
// 3 : 0111.0101 = 0x75
// 4 : 0101.0110 = 0x56
// 5 : 0111.0011 = 0x73
// 6 : 0111.1011 = 0x7B
// 7 : 0001,0101 = 0x15
// 8 : 0111.1111 = 0x7F
// 9 : 0111.0111 = 0x77
//   : 0000.0000 = 0x00
// - : 0100.0000 = 0x40
// E : 0110.1011 = 0x6B
// r : 0100.1000 = 0x48
// o : 0111.1000 = 0x78
// ^ : 0100.0111 = 0x47
// C : 0010.1011 = 0x2B


unsigned char wxLCDWindow::Decode( char c )
{
	unsigned char ret = 0;
	
	struct DecodedDisplay
	{
		char ch;
		unsigned char value;
	};

	DecodedDisplay dec[] = 
	{
		{ wxT('0'), 0x3F }, 
		{ wxT('1'), 0x14 },
		{ wxT('2'), 0x6D },
		{ wxT('3'), 0x75 },
		{ wxT('4'), 0x56 },
		{ wxT('5'), 0x73 },
		{ wxT('6'), 0x7B },
		{ wxT('7'), 0x15 },
		{ wxT('8'), 0x7F },
		{ wxT('9'), 0x77 },
		{ wxT(' '), 0x00 },
		{ wxT('-'), 0x40 },
		{ wxT('E'), 0x6B },
		{ wxT('r'), 0x48 },
		{ wxT('o'), 0x78 },
		{ wxT('^'), 0x47 },
		{ wxT('C'), 0x2B },
		{ 0, 0 }
	};

	for( int d = 0; dec[d].ch != 0; d++ )
	{
		if( dec[d].ch == c )
		{
			ret = dec[d].value;
			break;
		}
	}

	return ret;
}

