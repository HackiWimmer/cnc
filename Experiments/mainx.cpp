#include <chrono>
#include <wx/init.h>
#include <wx/string.h>
#include "CncArduino.h"
#include "CncTimeFunctions.h"
#include "CncSpeedSimulator.h"

int32_t pointA[3], pointB[3];
CncSpeedSimulator* cfmp = NULL;

///////////////////////////////////////////////////////////////////
bool provideMove(int32_t dx, int32_t dy, int32_t dz, void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( cfmp == NULL )
		return false;

	if ( abs(dx) > 0 ) cfmp->simulateSteppingX(dx);
	if ( abs(dy) > 0 ) cfmp->simulateSteppingY(dy);
	if ( abs(dz) > 0 ) cfmp->simulateSteppingZ(dz);

	cfmp->performCurrentOffset(false);

	// copy point A into point B
	memcpy(&pointB, &pointA, sizeof(pointA));
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool renderMove(int32_t dx , int32_t dy , int32_t dz, void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( cfmp == NULL )
		return false;

	cfmp->setNextMove(dx, dy, dz);

	// initialize
	int i, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
	memset(&pointA, 0, sizeof(pointA));
	memset(&pointB, 0, sizeof(pointB));
	
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
			
			if ( provideMove(pointA[0] - pointB[0], pointA[1] - pointB[1], pointA[2] - pointB[2], buffer, nbByte) == false )
				return false;
			
			if (err_1 > 0) {
				pointA[1] += y_inc;
				err_1     -= dx2;
			}
			if (err_2 > 0) {
				pointA[2] += z_inc;
				err_2     -= dx2;
			}
			err_1     += dy2;
			err_2     += dz2;
			pointA[0] += x_inc;
		}
	
	// -------------------------------------------------------------
	} else if ((m >= l) && (m >= n)) {
		err_1 = dx2 - m;
		err_2 = dz2 - m;
		for (i = 0; i < m; i++) {
			
			if ( provideMove(pointA[0] - pointB[0], pointA[1] - pointB[1], pointA[2] - pointB[2], buffer, nbByte) == false )
				return false;
			
			if (err_1 > 0) {
				pointA[0] += x_inc;
				err_1     -= dy2;
			}
			if (err_2 > 0) {
				pointA[2] += z_inc;
				err_2     -= dy2;
			}
			err_1     += dx2;
			err_2     += dz2;
			pointA[1] += y_inc;
		}
		
	// -------------------------------------------------------------
	} else {
		err_1 = dy2 - n;
		err_2 = dx2 - n;
		for (i = 0; i < n; i++) {
			
			if ( provideMove(pointA[0] - pointB[0], pointA[1] - pointB[1], pointA[2] - pointB[2], buffer, nbByte) == false )
				return false;
			
			if (err_1 > 0) {
				pointA[1] += y_inc;
				err_1     -= dz2;
			}
			if (err_2 > 0) {
				pointA[0] += x_inc;
				err_2     -= dz2;
			}
			err_1     += dy2;
			err_2     += dx2;
			pointA[2] += z_inc;
		}
	}
	
	// -------------------------------------------------------------
	if ( provideMove(pointA[0] - pointB[0], pointA[1] - pointB[1], pointA[2] - pointB[2], buffer, nbByte) == false )
		return false;
		
	return true;
}


///////////////////////////////////////////////////////////////////
void test1(CncSpeedSimulator& cfm) {
	std::clog << "::Test1 start ----------------------------------------" << std::endl;

	int32_t stepX = 1;
	int32_t stepY = 1;
	int32_t stepZ = 1;
	
	cfm.setFeedSpeed(600.0);
	
	CncNanoTimestamp t0 = CncTimeFunctions::getNanoTimestamp();
	renderMove(stepX, stepY, stepZ, NULL, 0);
	cfm.performCurrentOffset(true);
	CncNanoTimestamp t1 = CncTimeFunctions::getNanoTimestamp();
	
	double x = stepX * 0.015;
	double y = stepY * 0.015;
	double z = stepZ * 0.005;
	double t = cfm.getCurrentDistance();
	
	clog << x << ", " << y << ", " << z << endl;
	clog << t1 - t0 << ", " << t << ", " << (t / ((t1 - t0) / (1000.0 * 1000.0 * 1000.0))) * 60 << endl;
	//clog << t1 - t0 << ", " << t << ", " << ((t * 1000.0 * 1000.0 * 1000.0) / (t1 - t0) ) * 60 << endl;
	clog << "Total Accumulated Offset      [us] = " << cfm.getTotalAccumulatedOffset() << endl;
	clog << "Total Accumulated Offset X    [us] = " << cfm.getTotalAccumulatedOffsetX() << endl;
	clog << "Total Accumulated Offset Y    [us] = " << cfm.getTotalAccumulatedOffsetY() << endl;
	clog << "Total Accumulated Offset Z    [us] = " << cfm.getTotalAccumulatedOffsetZ() << endl;
	clog << "Total step counter X           [#] = " << cfm.getStepCounterX() << endl;
	clog << "Total step counter Y           [#] = " << cfm.getStepCounterY() << endl;
	clog << "Total step counter Z           [#] = " << cfm.getStepCounterZ() << endl;
	
	cfm.reset();
	
	std::clog << "::Test1 end  ----------------------------------------" << std::endl;
}

///////////////////////////////////////////////////////////////////
void test2(CncSpeedSimulator& cfm, double speed, int32_t stepX, int32_t stepY, int32_t stepZ) {
	//std::clog << "::Test2 start ----------------------------------------" << std::endl;

	cfm.setFeedSpeed(speed);
	
	CncNanoTimestamp t0 = CncTimeFunctions::getNanoTimestamp();
	renderMove(stepX, stepY, stepZ, NULL, 0);
	cfm.performCurrentOffset(true);
	CncNanoTimestamp t1 = CncTimeFunctions::getNanoTimestamp();
	
	double t = cfm.getCurrentDistance();
	clog << t1 - t0 << ", " << t << ", " << (t / ((t1 - t0) / (1000.0 * 1000.0 * 1000.0))) * 60 << endl;
	cfm.reset();
	
	//std::clog << "::Test2 end  ----------------------------------------" << std::endl;
}

///////////////////////////////////////////////////////////////////
int main( int argc, char** argv ) {
///////////////////////////////////////////////////////////////////
	// initialize wxWidgets
	wxInitializer init;

	unsigned char p1[8];
	unsigned char p2[8];

	int32_t leLong = 123456;
	memcpy(p1, &leLong, 4);
	
	p1[4] = '\0';
	
	
	int32_t v  = htonl(leLong);
	memcpy(p2, &v, 4);

	p2[4] = '\0';
	
	
	clog << p1 << endl;
	clog << p2 << endl;
	

	return 0;

	
	CncTimeFunctions::init();
	clog << "OPC Frequency         [Hz]: " << CncTimeFunctions::getOPCFrequency() << endl;
	clog << "OPC Min Tick Interval [ns]: " << CncTimeFunctions::geMaxtQPCResolutionNS() << endl;
	
	CncSpeedSimulator cfm(40, 12.0, 800, 200, 12.0, 800, 200, 2.0, 400, 200);
	
	clog << wxString::Format("Max steps X         : % 5d", cfm.getMaxStepsX()) << std::endl;
	clog << wxString::Format("Max steps Y         : % 5d", cfm.getMaxStepsX()) << std::endl;
	clog << wxString::Format("Max steps Z         : % 5d", cfm.getMaxStepsX()) << std::endl;

	clog << wxString::Format("Max speed X         : %7.1lf", cfm.getMaxSpeedX_MM_MIN()) << std::endl;
	clog << wxString::Format("Max speed Y         : %7.1lf", cfm.getMaxSpeedY_MM_MIN()) << std::endl;
	clog << wxString::Format("Max speed Z         : %7.1lf", cfm.getMaxSpeedZ_MM_MIN()) << std::endl;
	
	clog << wxString::Format("Current offset X    : % 5d", cfm.getOffsetPerStepX()) << std::endl;
	clog << wxString::Format("Current offset Y    : % 5d", cfm.getOffsetPerStepY()) << std::endl;
	clog << wxString::Format("Current offset Z    : % 5d", cfm.getOffsetPerStepZ()) << std::endl;

	cfm.setFeedSpeed(600.0);
	cfm.setNextMove(67, 67, 200);
	clog << wxString::Format("Current offset X    : % 5d", cfm.getOffsetPerStepX()) << std::endl;
	clog << wxString::Format("Current offset Y    : % 5d", cfm.getOffsetPerStepY()) << std::endl;
	clog << wxString::Format("Current offset Z    : % 5d", cfm.getOffsetPerStepZ()) << std::endl;
	
	cfmp = &cfm;
	//test1(cfm);
	for ( int i = 0; i < 10; i++ ) {
		double s = 600.0;
		if ( i % 2 == 0 )
			s = 200.0;
			
		if ( i % 3 == 0 )
			s = 1200.0;

		test2(cfm, s,  67,  67, 200);
		test2(cfm, s, 167, 167, 200);
		test2(cfm, s,  67,  67, 200);
		test2(cfm, s, 167, 167, 200);
	}
}
