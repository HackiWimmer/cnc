#include <iostream>
#include <cmath>
#include <ctgmath>
#include <wx/debug.h>
#include <wx/string.h>
#include "SerialCircularBuffer.h"
#include "SerialEndPoint.h"

/////////////////////////////////////////////////////////////////////
SerialEndPoint::SerialEndPoint(SerialCircularBuffer* rb, SerialCircularBuffer* wb)
: BaseEndPoint()
, rdBuffer(rb)
, wtBuffer(wb)
/////////////////////////////////////////////////////////////////////
{
}
/////////////////////////////////////////////////////////////////////
SerialEndPoint::~SerialEndPoint() {
/////////////////////////////////////////////////////////////////////
	purge();
}
/////////////////////////////////////////////////////////////////////
void SerialEndPoint::begin(uint32_t baud) {
/////////////////////////////////////////////////////////////////////
	purge();
}
/////////////////////////////////////////////////////////////////////
void SerialEndPoint::end() {
/////////////////////////////////////////////////////////////////////
	purge();
}
/////////////////////////////////////////////////////////////////////
void SerialEndPoint::setTimeout(uint32_t timeout) {
/////////////////////////////////////////////////////////////////////
	rdBuffer->setTimeout(timeout);
	wtBuffer->setTimeout(timeout);
}
/////////////////////////////////////////////////////////////////////
void SerialEndPoint::purge() {
/////////////////////////////////////////////////////////////////////
	wxASSERT ( rdBuffer != NULL );
	rdBuffer->clear();

	wxASSERT ( wtBuffer != NULL );
	wtBuffer->clear();
}
/////////////////////////////////////////////////////////////////////
void SerialEndPoint::flush() {
/////////////////////////////////////////////////////////////////////
	// ????
}
/////////////////////////////////////////////////////////////////////
int SerialEndPoint::available(void) {
/////////////////////////////////////////////////////////////////////
	wxASSERT ( rdBuffer != NULL );
	return rdBuffer->size();
}
/////////////////////////////////////////////////////////////////////
int SerialEndPoint::peek(void) {
/////////////////////////////////////////////////////////////////////
	wxASSERT ( rdBuffer != NULL );
	return rdBuffer->peak();
}
/////////////////////////////////////////////////////////////////////
int SerialEndPoint::read(void) {
/////////////////////////////////////////////////////////////////////
	wxASSERT ( rdBuffer != NULL );
	return rdBuffer->read();
}
/////////////////////////////////////////////////////////////////////
int SerialEndPoint::readBytes(byte* b, int size) {
/////////////////////////////////////////////////////////////////////
	wxASSERT ( rdBuffer != NULL );
	return rdBuffer->read(b, size);
}
/////////////////////////////////////////////////////////////////////
int SerialEndPoint::write(byte b) {
/////////////////////////////////////////////////////////////////////
	wxASSERT ( wtBuffer != NULL );
	return wtBuffer->write(b);
}
/////////////////////////////////////////////////////////////////////
int SerialEndPoint::write(byte* b, int size) {
/////////////////////////////////////////////////////////////////////
	wxASSERT ( wtBuffer != NULL );
	return wtBuffer->write(b, size);
}
/////////////////////////////////////////////////////////////////////
int SerialEndPoint::write(const char* str) {
/////////////////////////////////////////////////////////////////////
	wxASSERT ( wtBuffer != NULL );

	if ( str == NULL )
		return 0;

	wxString s(str);
	for ( unsigned int i = 0; i< s.length(); i++ ) {
		wtBuffer->write(str[i]);
	}

	return s.length();
}


/////////////////////////////////////////////////////////////////////
size_t SerialEndPoint::print(const char str[]) 					{ return write(str);										}
size_t SerialEndPoint::print(char c) 							{ return write(c);											}
size_t SerialEndPoint::print(unsigned char b, int base) 		{ return print((unsigned long) b, base); 					}
size_t SerialEndPoint::print(int n, int base)					{ return print((long) n, base); 							}
size_t SerialEndPoint::print(unsigned int n, int base)			{ return print((unsigned long) n, base); 					}
size_t SerialEndPoint::print(double n, int digits)				{ return printFloat(n, digits); 							}

size_t SerialEndPoint::println(void)							{ return write("\r\n"); }
size_t SerialEndPoint::println(const char c[])					{ size_t n = print(c); 			n += println(); return n; 	}
size_t SerialEndPoint::println(char c)							{ size_t n = print(c); 			n += println(); return n; 	}
size_t SerialEndPoint::println(unsigned char b, int base) 		{ size_t n = print(b, base); 	n += println(); return n; 	}
size_t SerialEndPoint::println(int num, int base)				{ size_t n = print(num, base);  n += println(); return n; 	}
size_t SerialEndPoint::println(unsigned int num, int base)  	{ size_t n = print(num, base);  n += println(); return n; 	}
size_t SerialEndPoint::println(long num, int base)				{ size_t n = print(num, base);	n += println();	return n; 	}
size_t SerialEndPoint::println(unsigned long num, int base) 	{ size_t n = print(num, base);	n += println();	return n; 	}
size_t SerialEndPoint::println(double num, int digits)			{ size_t n = print(num, digits);n += println(); return n; 	}

/////////////////////////////////////////////////////////////////////
size_t SerialEndPoint::print(long n, int base) {
/////////////////////////////////////////////////////////////////////

	if ( base == 0 ) {
		return write(n);

	} else if (base == 10) {

		if (n < 0) {
			int t = print('-');
			n = -n;
			return printNumber(n, 10) + t;
		}

		return printNumber(n, 10);

	} else {
		return printNumber(n, base);
	}
}
/////////////////////////////////////////////////////////////////////
size_t SerialEndPoint::print(unsigned long n, int base) {
/////////////////////////////////////////////////////////////////////
	if (base == 0) 	return write(n);
	else 			return printNumber(n, base);
}
/////////////////////////////////////////////////////////////////////
size_t SerialEndPoint::printNumber(unsigned long n, uint8_t base) {
/////////////////////////////////////////////////////////////////////

	char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
	char *str = &buf[sizeof(buf) - 1];

	*str = '\0';

	// prevent crash if called with base == 1
	if (base < 2) base = 10;

	do {
		char c = n % base;
		n /= base;

		*--str = c < 10 ? c + '0' : c + 'A' - 10;
	} while(n);

	return write(str);
}
/////////////////////////////////////////////////////////////////////
size_t SerialEndPoint::printFloat(double number, uint8_t digits) {
/////////////////////////////////////////////////////////////////////
	size_t n = 0;

	if (std::isnan(number)) 	return print("nan");
	if (std::isinf(number)) 	return print("inf");
	if (number > 4294967040.0) 	return print ("ovf");  // constant determined empirically
	if (number <-4294967040.0) 	return print ("ovf");  // constant determined empirically

	// Handle negative numbers
	if (number < 0.0) {
		n += print('-');
		number = -number;
	}

	// Round correctly so that print(1.999, 2) prints as "2.00"
	double rounding = 0.5;
	for (uint8_t i=0; i<digits; ++i)
		rounding /= 10.0;

	number += rounding;

	// Extract the integer part of the number and print it
	unsigned long int_part = (unsigned long)number;
	double remainder = number - (double)int_part;
	n += print(int_part);

	// Print the decimal point, but only if there are digits beyond
	if (digits > 0)
		n += print('.');


	// Extract digits from the remainder one at a time
	while (digits-- > 0) {
		remainder *= 10.0;
		unsigned int toPrint = (unsigned int)(remainder);
		n += print(toPrint);
		remainder -= toPrint;
	}

	return n;
}


