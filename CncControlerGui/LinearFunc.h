#ifndef LINEAR_FUNC_H
#define LINEAR_FUNC_H

#include <cmath>
#include <iostream>
#include <wx/gdicmn.h>

struct FunctionValue {
	public:
		bool valid;
		double value;
		
		FunctionValue() {
			valid = false;
			value = 0.0;
		}
		
		FunctionValue(const FunctionValue& v) {
			valid = v.valid;
			value = v.value;
		}
		
		friend bool operator== (const FunctionValue &a, const FunctionValue &b) {
			return ( a.valid == b.valid && a.value == b.value );
		}

		friend bool operator!= (const FunctionValue &a, const FunctionValue &b) {
			return (!operator== (a, b));
		}
};

struct IntersectionValue {
	bool valid 		= false;
	wxRealPoint p 	= {0,0};
	
	friend bool operator== (const IntersectionValue &a, const IntersectionValue &b) {
		return ( (a.valid == b.valid) && (a.p.x == b.p.x) && (a.p.y == b.p.y));
	}

	friend bool operator!= (const IntersectionValue &a, const IntersectionValue &b) {
		return (!operator== (a, b));
	}
	// ostream operator
	friend std::ostream &operator<< (std::ostream &ostr, const IntersectionValue &a) {
		if ( a.valid ) {
			ostr << "Intersection: " << a.p.x << ", " << a.p.y << std::endl;
		} else {
			ostr << "No intersection available" << std::endl;
		}
		
		return ostr;
	}		
};
		
class LinearFunc {

	private:
		FunctionValue _ret;
		
	protected:
		FunctionValue _m;
		FunctionValue _b;
		FunctionValue _constX;
		
	public:
		// default
		LinearFunc() {
			// all members per default correct initialized
		}
		
		// Copy
		LinearFunc(LinearFunc& f) {
			reInit(f);
		}
		
		// given is m and b from y = mx + b
		LinearFunc(double m, double b) {
			reInit(m, b);
		}
		
		// pure veretical line x = c
		LinearFunc(double c) {
			reInit(c);
		}

		// given is one absolute point and the x and y deltas to the next point
		LinearFunc(wxRealPoint p1, double dx, double dy) {
			reInit(p1, dx, dy);
		}

		// given are two absolute points
		LinearFunc(wxRealPoint p1, wxRealPoint p2)
		: LinearFunc(p1, p2.x - p1.x, p2.y - p1.y) {
		}
		
		~LinearFunc() {
		}
		
		// given is f
		void reInit(LinearFunc& f) {
			_m 			= f.m();
			_b 			= f.b();
			_constX 	= f.c();
		}

		// given are two absolute points
		void reInit(wxRealPoint p1, wxRealPoint p2) {
			reInit(p1, p2.x - p1.x, p2.y - p1.y);
		}
		
		// given is one absolute point and the x and y deltas to the next point
		void reInit(wxRealPoint p1, double dx, double dy) {
			if ( dx != 0 ) {
				_m.valid = true;
				_m.value = dy/dx;
				
				_b.valid = true;
				if ( dy != 0 )	_b.value = p1.y - (_m.value * p1.x);
				else			_b.value = p1.y;
				
				_constX.valid = false;
				_constX.value = 0.0;

			} else {
				_m.valid = false;
				_m.value = 0.0;

				_b.valid = false;
				_b.value = 0.0;

				_constX.valid  = true;
				_constX.value = p1.x;
			}
		}
		// given is m and b from y = mx + b
		void reInit(double m, double b) {
			_m.valid = true;
			_m.value = m;
			
			_b.valid = true;
			_b.value = b;

			_constX.valid = false;
			_constX.value = 0.0;
		}
		
		// given is x = c
		void reInit(double c) {
			_m.valid = false;
			_m.value = 0;

			_b.valid = false;
			_b.value = 0;

			_constX.valid = true;
			_constX.value = c;
		}
		
		// returns true if m = dy/dx is valid --> dx > 0
		const bool isValid()    const { return _m.valid; }
		// returns x = c for pure vertical lines
		const FunctionValue& c() const { return _constX; }
		// returns m from y = mx + b
		const FunctionValue& m() const { return _m; }
		// returns b from y = mx + b
		const FunctionValue& b() const { return _b; }
		// returns y from y = mx + b
		const FunctionValue& f(const FunctionValue& x) {
			if ( x.valid == false)
				return _ret;
				
			return f(x.value); 
		}
		// returns y from y = mx + b
		const FunctionValue& f(const double x) {
			if ( _m.valid == false )
				return _m;
			
			_ret.valid = true;
			_ret.value = _m.value * x + _b.value;
			
			return _ret;
		}
		// parallel shift of the currrent function, r == the distance betwenn the two lines
		void parallelShift(double r) {
			if ( isValid() == false ) {
				_constX.value += r;
				return;
			}
			
			_b.value += r / ( cos(atan(_m.value)) );
		}
		// Invert the function: from y = m x +b --> y = -1/m x -b (Mittelsenkrechte)
		void invert() {
			if ( isValid() == true ) {
				_m.value  = -1/_m.value;
				_b.value *= -1;
				
				_constX.valid = false;
				_constX.value = 0.0;
			} else {
				_b.valid = true;
				_b.value = _constX.value;
				
				_b.valid = true;
				_b.value = 0.0;
				
				_constX.valid = false;
				_constX.value = 0.0;
			}
		}
		// returns the intersection of this fuction with f2 if any exists
		IntersectionValue intersection(LinearFunc f2) {
			IntersectionValue iv;
			if ( isValid() && f2.isValid() ) {
				double md = _m.value - f2.m().value;
				if ( md != 0) {
					iv.p.x = (f2.b().value - b().value) / md;
					iv.p.y = f(iv.p.x).value;
					iv.valid = true;
				}
			} else if ( isValid() ) {
				iv.p.x = f2.c().value;
				iv.p.y = f(f2.c()).value;
				iv.valid = true;
			} else if ( f2.isValid() ) {
				iv.p.x = c().value;
				iv.p.y = f2.f(c()).value;
				iv.valid = true;
			}
			
			return iv;
		}
		// == operator
		friend bool operator== (const LinearFunc &a, const LinearFunc &b) {
			if ( a.isValid() == b.isValid() && a.isValid() == true )
				return (a.m() == b.m() && a.b() == b.b());
			
			if ( a.isValid() == b.isValid() && a.isValid() == false )
				return (a.c() == b.c());
			
			return false;
		}
		// != operator
		friend bool operator!= (const LinearFunc &a, const LinearFunc &b) {
			return (!operator== (a, b));
		}
		// ostream operator
		friend std::ostream &operator<< (std::ostream &ostr, const LinearFunc &a) {
			if ( a.isValid() ) {
				char sign = '+';
				if ( a.b().value < 0 )
					sign = '-';
					
				ostr << "y = " << a.m().value << "x " << sign << " " << abs(a.b().value) << std::endl;
			} else {
				ostr << "x = " << a.c().value  << std::endl;
			}
			
			return ostr;
		}		
};

#endif