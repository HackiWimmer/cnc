#include <stdio.h>
#include <cstdlib>
#include <stdint.h>
#include <iostream>
#include <cstring>

static const short POINT_LENGTH = 3;
static const short PIX  = 0;
static const short PIY  = 1;
static const short PIZ  = 2;

int pointA[POINT_LENGTH], pointB[POINT_LENGTH];

///////////////////////////////////////////////////////////////////
bool provideMove(int32_t dx , int32_t dy , int32_t dz, unsigned char *buffer, unsigned int nbByte, bool force=false) {
///////////////////////////////////////////////////////////////////
	std::cout << "   provideMove: " << nbByte << " - " << dx << ", " << dy << ", " << dz << std::endl;
	
	// copy point A into point B
	memcpy(pointB, pointA, sizeof(pointA));
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool renderMove(int32_t dx , int32_t dy , int32_t dz, unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( dx == 0 && dy == 0 && dz == 0 ) 
		return true;
	
	
	
	// update speed simulator values
	/*
	if ( GBL_CONFIG->isProbeMode() == false ) {
		wxASSERT( speedSimulator != NULL );
		speedSimulator->setNextMove(dx, dy, dz);
	}*/
	
	bool render = true;
	
	//if ( dx == 0 && dz == 0 )  render = false;
	//if ( dy == 0 && dz == 0 )  render = false;
	//if ( dx == 0 && dy == 0 )  render = false;
	
	
	if ( render == true ) {
		// initialize
		int i, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
		memset(pointA, 0, sizeof(pointA));
		memset(pointB, 0, sizeof(pointB));
		
		x_inc = (dx < 0) ? -1 : 1;
		l = abs(dx);
		
		y_inc = (dy < 0) ? -1 : 1;
		m = abs(dy);
		
		z_inc = (dz < 0) ? -1 : 1;
		n = abs(dz);
		
		dx2 = l << 1;
		dy2 = m << 1;
		dz2 = n << 1;

		// -------------------------------------------------------------
		if ((l >= m) && (l >= n)) {
			err_1 = dy2 - l;
			err_2 = dz2 - l;
			for (i = 0; i < l; i++) {
				
				if ( provideMove(pointA[PIX] - pointB[PIX], pointA[PIY] - pointB[PIY], pointA[PIZ] - pointB[PIZ], buffer, 1) == false )
					return false;
				
				if (err_1 > 0) { pointA[1] += y_inc; err_1 -= dx2; }
				if (err_2 > 0) { pointA[2] += z_inc; err_2 -= dx2; }
				
				err_1     += dy2;
				err_2     += dz2;
				pointA[0] += x_inc;
			}
		
		// -------------------------------------------------------------
		} else if ((m >= l) && (m >= n)) {
			err_1 = dx2 - m;
			err_2 = dz2 - m;
			for (i = 0; i < m; i++) {
				
				if ( provideMove(pointA[PIX] - pointB[PIX], pointA[PIY] - pointB[PIY], pointA[PIZ] - pointB[PIZ], buffer, 2) == false )
					return false;
				
				if (err_1 > 0) { pointA[0] += x_inc; err_1 -= dy2; }
				if (err_2 > 0) { pointA[2] += z_inc; err_2 -= dy2; }
				
				err_1     += dx2;
				err_2     += dz2;
				pointA[1] += y_inc;
			}
			
		// -------------------------------------------------------------
		} else {
			err_1 = dy2 - n;
			err_2 = dx2 - n;
			for (i = 0; i < n; i++) {
				
				if ( provideMove(pointA[PIX] - pointB[PIX], pointA[PIY] - pointB[PIY], pointA[PIZ] - pointB[PIZ], buffer, 3) == false )
					return false;
				
				if (err_1 > 0) { pointA[1] += y_inc; err_1 -= dz2; }
				if (err_2 > 0) { pointA[0] += x_inc; err_2 -= dz2; }
				
				err_1     += dy2;
				err_2     += dx2;
				pointA[2] += z_inc;
			}
		}
		
		dx = pointA[PIX] - pointB[PIX];
		dy = pointA[PIY] - pointB[PIY];
		dz = pointA[PIZ] - pointB[PIZ];
	}
	
	// -------------------------------------------------------------
	if ( provideMove(dx, dy, dz, buffer, 4, true) == false )
		return false;
	
	// perform any rest offset
	/*
	if ( GBL_CONFIG->isProbeMode() == false ) {
		wxASSERT( speedSimulator != NULL );
		speedSimulator->performCurrentOffset(true);
		speedSimulator->finalizeMove();
	}*/
	return true;
}

///////////////////////////////////////////////////////////////////
bool W_renderMove(int32_t dx , int32_t dy , int32_t dz, unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	std::cout << "renderMove(" << dx << ", " << dy << ", " << dz << ")\n{" << std::endl;
		bool ret = renderMove(dx , dy , dz, buffer, nbByte);
	std::cout << "}" << std::endl;
	
	return ret;
}
///////////////////////////////////////////////////////////////////
int mainX(int argc, char **argv) {
///////////////////////////////////////////////////////////////////

	unsigned char *buffer = NULL;
	unsigned int nbByte   = 0;

	
	W_renderMove(   0,   0,   0, buffer, nbByte);
	W_renderMove(   5,   0,   0, buffer, nbByte);
	W_renderMove(   0,   5,   0, buffer, nbByte);
	W_renderMove(   0,   0,   5, buffer, nbByte);
	
	W_renderMove(   5,   5,   0, buffer, nbByte);
	W_renderMove(   5,   5,   5, buffer, nbByte);
	W_renderMove(  10,   5,   5, buffer, nbByte);
	
	W_renderMove( -10,   5,  -5, buffer, nbByte);
	
	W_renderMove(-100,   5,  -5, buffer, nbByte);
	
	
	
	
	
	printf("\nbye bye\n");
	return 0;
}

template <class T>
T minimum ( T v1, T v2, T v3) {
	return std::min( v3, std::min(v1, v2));
}

template <class T>
T maximum ( T v1, T v2, T v3) {
	return std::max( v3, std::max(v1, v2));
}

template <class T>
inline T absolute(T val) {
	if ( val < 0.0 )
	  val *= -1;
	  
	return val;
}

template <>
inline int8_t absolute(int8_t x) {
	const int8_t a = x >> 7;
	return ((x ^ a) - a);
}

template <>
inline int16_t absolute(int16_t x) {
	const int16_t a = x >> 15;
	return ((x ^ a) - a);
}

template <>
inline int32_t absolute(int32_t x) {
	const int32_t a = x >> 31;
	return ((x ^ a) - a);
}

class CncSpeedController {
	
	protected:
	
		struct Axis {
			char				axis				= '?';
			unsigned int		steps				=   1;
			double				pitch				= 1.0;
			unsigned int		offsetRendering 	=   0;
			unsigned int		offsetStepping 		=   0;
			unsigned int		totalPulsWidth 		=   0;
			
			unsigned int		totalOffset			=   0;
			unsigned int		maxStepsPerSecond   =   0;
			unsigned int		maxDistPerSecond    =   0;
			unsigned int		maxDistPerMinute	=   0;
			
			unsigned int 		synthSpeedDelay  	=   0;
			
			double				gearing			 	= 0.0;
			
			unsigned int		stepsPerSec			=   0;
			unsigned int		microSecPerStep		=   0;
			unsigned int		rpm					= 0;
			
			//////////////////////////////////////////////////////////////////
			Axis(const char a)
			: axis(a)
			{}
			
			//////////////////////////////////////////////////////////////////
			void setup(unsigned int s, double p, unsigned int oR, unsigned int oS, unsigned int tPW) {
				steps 				= (s ==  0 ?   1 : s);
				pitch 				= (p < 0.0 ? 0.0 : p);
				offsetRendering 	= oR;
				offsetStepping 		= oS;
				totalPulsWidth 		= tPW;
				gearing 			= pitch / steps;
				totalOffset			= offsetRendering + offsetStepping + totalPulsWidth;
				maxStepsPerSecond	= (1000 * 1000) / totalOffset;
				maxDistPerSecond	= maxStepsPerSecond * gearing;
				maxDistPerMinute	= maxDistPerSecond * 60;
			}
			
			//////////////////////////////////////////////////////////////////
			bool calculateStatics(double speed_MM_SEC) {
				if ( speed_MM_SEC <= 0.0 )
					return false;
				
				stepsPerSec			= speed_MM_SEC     / gearing; 
				microSecPerStep		= (1000 * 1000 )   / stepsPerSec;
				rpm					= stepsPerSec * 60 / steps;
				synthSpeedDelay     = (microSecPerStep > totalOffset ? microSecPerStep - totalOffset : 0);
				
				return true;
			}
			
			//////////////////////////////////////////////////////////////////
			friend std::ostream &operator<< (std::ostream &ostr, const Axis &a) {
				const char* o = "   ";
				ostr << " Axis: " << a.axis << ':' 													<< std::endl;
				ostr << o << "Steps                            [steps] : " << a.steps 				<< std::endl;
				ostr << o << "Pitch                           [mm/360] : " << a.pitch				<< std::endl;
				ostr <<	o << "Gearing                        [mm/step] : " << a.gearing				<< std::endl;
				ostr << o << "Offset Rendering                  [usec] : " << a.offsetRendering		<< std::endl;
				ostr << o << "Offset Stepping                   [usec] : " << a.offsetStepping		<< std::endl;
				ostr << o << "Total Puls Width                  [usec] : " << a.totalPulsWidth		<< std::endl;
				ostr << o << "Total Offset                      [usec] : " << a.totalOffset			<< std::endl;
				ostr << o << "Max steps per Second (theo.) [steps/sec] : " << a.maxStepsPerSecond	<< std::endl;
				ostr << o << "Max Feed Speed       (theo.)    [mm/sec] : " << a.maxDistPerSecond 	<< std::endl;
				ostr << o << "Max Feed Speed       (theo.)    [mm/min] : " << a.maxDistPerMinute 	<< std::endl;
				ostr << o 	 																		<< std::endl;
				ostr << o << "As function of configured speed:"										<< std::endl;
				ostr << o << "   Rounds per Minute               [rpm] : " << a.rpm					<< std::endl;
				ostr << o << "   Steps per Second           [step/sec] : " << a.stepsPerSec			<< std::endl;
				ostr << o << "   Microseconds per Step     [usec/step] : " << a.microSecPerStep		<< std::endl;
				ostr << o << "   Synthetically Speed Delay [usec/step] : " << a.synthSpeedDelay		<< std::endl;
				
				return ostr;
			}
		};
		
		double feedSpeed_MM_SEC;
		
	public:
		
		Axis X, Y, Z;
		
		//////////////////////////////////////////////////////////////////
		CncSpeedController()
		: feedSpeed_MM_SEC(0.0)
		, X('X')
		, Y('Y')
		, Z('Z')
		{}
		
		//////////////////////////////////////////////////////////////////
		double getMaxFeedSpeed_MM_MIN() const { return minimum(X.maxDistPerMinute, Y.maxDistPerMinute, Z.maxDistPerMinute); }
		double getMaxFeedSpeed_MM_SEC() const { return minimum(X.maxDistPerSecond, Y.maxDistPerSecond, Z.maxDistPerSecond); }
		
		//////////////////////////////////////////////////////////////////
		double getConfiguredFeedSpeed_MM_MIN() const { return feedSpeed_MM_SEC * 60; }
		double getConfiguredFeedSpeed_MM_SEC() const { return feedSpeed_MM_SEC; }
		
		//////////////////////////////////////////////////////////////////
		void setFeedSpeed_MM_MIN(double s) {
			feedSpeed_MM_SEC = ( s < 0.0 ? 0.0 : s / 60 );
			
			X.calculateStatics(feedSpeed_MM_SEC);
			Y.calculateStatics(feedSpeed_MM_SEC);
			Z.calculateStatics(feedSpeed_MM_SEC);
		}
		
		
		
		//////////////////////////////////////////////////////////////////
		void simulateSteps(int32_t dx, int32_t dy, int32_t dz) {
			const uint32_t ax       = absolute<int32_t>(dx);
			const uint32_t ay       = absolute<int32_t>(dy);
			const uint32_t az       = absolute<int32_t>(dz);
			
			const uint32_t tx 		= ax * (X.synthSpeedDelay + X.totalOffset);
			const uint32_t ty 		= ay * (Y.synthSpeedDelay + Y.totalOffset);
			const uint32_t tz 		= az * (Z.synthSpeedDelay + Z.totalOffset);
			const uint32_t t		= tx + ty + tz;
			
			const uint32_t totalCountOfSteps    = ax + ay + az;
			
			const double totalDistance			= ax * X.gearing
			                                    + ay * Y.gearing
			                                    + az * Z.gearing;
			
			const double speed                  = (1000 *1000 * totalDistance) / t * 60; 
			
			
			std::cout << std::endl << speed << std::endl;

											 
											 
		}
		
		//////////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const CncSpeedController &c) {
			ostr << "CncSpeedController Axis: " << std::endl;
			ostr << c.X;
			//ostr << c.Y;
			//ostr << c.Z;
			
			ostr  																							<< std::endl;
			ostr << "CncSpeedController Parameters: " 														<< std::endl;
			ostr << " Configured FeedSpeed XYZ:         [mm/sec] : " << c.getConfiguredFeedSpeed_MM_SEC() 	<< std::endl;
			ostr << " Configured FeedSpeed XYZ:         [mm/min] : " << c.getConfiguredFeedSpeed_MM_MIN() 	<< std::endl;
			ostr << " Max FeedSpeed XYZ:                [mm/sec] : " << c.getMaxFeedSpeed_MM_SEC() 			<< std::endl;
			ostr << " Max FeedSpeed XYZ:                [mm/min] : " << c.getMaxFeedSpeed_MM_MIN() 			<< std::endl;
			
			
			return ostr;
		}
		
};


///////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
///////////////////////////////////////////////////////////////////

	CncSpeedController sc;
	sc.X.setup(800, 12.0, 12+50, 30, 50);
	sc.Y.setup(800, 12.0, 12+50, 30, 50);
	sc.Z.setup(800, 12.0, 12+50, 30, 50);
	
	sc.setFeedSpeed_MM_MIN(3600);
	std::cout << sc;
	
	sc.simulateSteps(10, 5, 2);
	
	printf("\nbye bye\n");
	return 0;
}
