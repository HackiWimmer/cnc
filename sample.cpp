#include <cstring>
#include <stdint.h>
#include <string>
#include <iostream>
#include <cmath>



	#define SKETCH_COMPILE = TRUE
	typedef unsigned char byte;
	#define A0 	0
	#define A1 	1
	#define A2 	2
	#define A3 	3
	#define A4 	4
	#define A5 	5
	#define A6 	6
	#define A7 	7
	
	//#include "./Arduino/StepperEnvironment/ArdoVal.h"
	#include "./Arduino/StepperEnvironment/CncRndr.ino"
	
	
	
	// support the arduino runtime code
	/*
	#include "./Arduino/StepperEnvironment/MainLoop.ino"
	#include "./Arduino/StepperEnvironment/CncCtrl.ino"
	#include "./Arduino/StepperEnvironment/CncStep.ino"
	#include "./Arduino/StepperEnvironment/CncRndr.ino"
	#include "./Arduino/StepperEnvironment/CncPwmr.ino"
	#include "./Arduino/StepperEnvironment/ArdoErr.ino"
	*/


class CncPulsWidthCtrl : public ArduinoPositionRenderer {
	
	public:
	
		class Axis {
		
			protected:
				Axis(const Axis&);
					
			public:
				enum AID { X=0, Y=1, Z=2 }; 
				
				// -----------------------------------------------------------------------------------
				struct Stepper {
					uint16_t	minPW 	= 500;	// us
					uint16_t 	steps	= 200;	// #
					double		pitch 	= 1.0;	// mm/360
					
					double curF_MMMin 	= 0.0;	// mm/min
					
					bool	isOk()			{ return isStaOk() && isDynOk(); }
					
					bool	isStaOk()		{ return ( 	steps 		> 0 
													&&  pitch 		> 0.0
													&&	minPW 		> 0
												 ); }
										
					bool	isDynOk()		{ return ( 		isStaOk() 	> 0 
													&&  curF_MMMin 	> 0.0
													&&	minPW 		< curF_PW() 
												 ); }
					
					double feedrate() 	{ return isStaOk() ? pitch / (double)steps 				: 0.0;    }
					double maxF_STSec() { return isStaOk() ? 1 / ((double)minPW / 1000 / 1000) 	: 0.0;    }	
					double maxF_MMSec() { return isStaOk() ? maxF_STSec() * feedrate() 			: 0.0;    }	
					double maxF_MMMin() { return isStaOk() ? maxF_MMSec() * 60						: 0.0;    }
					double curF_STMin() { return isStaOk() ? cnvF_MMMin_2_STMin(curF_MMMin)		: 0.0;    }
					double curF_STSec() { return isStaOk() ? curF_STMin() / 60 					: 0.0;    }
					double curF_PW()    { return isStaOk() ? 1 / curF_STSec() * 1000 * 1000		: minPW;  }
					
					// --------------------------------------------------------------------------------	
					void reset() {
						// reset the dynamic part only
						Stepper s;
						curF_MMMin = s.curF_MMMin;
					}
					
					// --------------------------------------------------------------------------------	
					void traceState(std::ostream& o, int indent=0) {
						if ( isDynOk() == false ) {
							o << "Stepper isn't OK. Here the parameters:" << std::endl;
							trace(o, indent);
						}
					}
					
					// --------------------------------------------------------------------------------	
					void trace(std::ostream& o, int indent=0) {
						traceStaticValues (o, indent);
						traceDynamicValues(o, indent);
					}
					
					// --------------------------------------------------------------------------------	
					void traceStaticValues(std::ostream& o, int indent=0) {
						std::string prefix(indent, ' ');
						o << prefix << "Stepper.steps           = " << steps 			<< " [#]" 			<< std::endl;
						o << prefix << "Stepper.pitch           = " << pitch 			<< " [mm/360']" 	<< std::endl;
						o << prefix << "Stepper.minPW           = " << minPW 			<< " [us]"		 	<< std::endl;
						o << prefix << "Stepper.feedrate        = " << feedrate() 		<< " [mm/step]" 	<< std::endl;
						o << prefix << "Stepper.max F           = " << maxF_STSec() 	<< " [steps/sec]" 	<< std::endl;
						o << prefix << "Stepper.max F           = " << maxF_MMSec() 	<< " [mm/sec]" 		<< std::endl;
						o << prefix << "Stepper.max F           = " << maxF_MMMin() 	<< " [mm/min]" 		<< std::endl;
					}
					
					// --------------------------------------------------------------------------------	
					void traceDynamicValues(std::ostream& o, int indent=0) {
						std::string prefix(indent, ' ');
						o << prefix << "Stepper.cur F           = " << curF_STMin() 	<< " [steps/min]"	<< std::endl;
						o << prefix << "Stepper.cur F           = " << curF_STSec() 	<< " [steps/sec]"	<< std::endl;
						o << prefix << "Stepper.cur F PW        = " << curF_PW() 	    << " [us]"			<< std::endl;
					}
					
					// --------------------------------------------------------------------------------	
					double cnvF_MMMin_2_STMin(double value ) { return value / feedrate(); }
					double cnvF_MMSec_2_STSec(double value ) { return value / feedrate(); }
				};
				
				// ------------------------------------------------------------------------------------
				struct Acceleration {
					double startF			= 0.0;	// mm/min
					double stopF			= 0.0;	// mm/min
					
					// f(step) = A * (step)^2 + B * step + C
					double	sfA				=  0.0;
					double	sfB				= 10.0;
					double	sfC				=  0.0;
					
					bool 	active			= false;
					int32_t	defaultValue	= -1;
					
					bool	isOk()			{ return isStaOk() && isDynOk(); }
					
					bool	isStaOk()		{ return 	startF 			> 0.0
													&& 	stopF 			> 0.0
											;}
					bool	isDynOk()		{ return 	isStaOk()
													&& 	defaultValue	> 0
											;}
					
					double	startF_MMMin()	{ return startF;              }
					double	startF_MMSec()	{ return startF_MMMin() / 60; }
					double	stopF_MMMin()	{ return stopF;               }
					double	stopF_MMSec()	{ return stopF_MMMin() / 60;  }
					
					//---------------------------------------------------------------------------------
					void reset() {
						active = Acceleration().active;
					}
					
					// --------------------------------------------------------------------------------	
					void traceState(std::ostream& o, int indent=0) {
						if ( isStaOk() == false ) {
							o << "Acceleration isn't OK. Here the parameters:" << std::endl;
							trace(o, indent);
						}
					}
					
					// --------------------------------------------------------------------------------	
					void trace(std::ostream& o, int indent=0) {
						traceStaticValues (o, indent);
						traceDynamicValues(o, indent);
					}
					
					//---------------------------------------------------------------------------------
					void traceStaticValues(std::ostream& o, int indent) {
						std::string prefix(indent, ' ');
						
						o << prefix << "Accel.startThreshold    = " << startF_MMMin() 	<< " [mm/min]" 		<< std::endl;
						o << prefix << "Accel.startThreshold    = " << startF_MMSec() 	<< " [mm/sec]" 		<< std::endl;
						o << prefix << "Accel.stopThreshold     = " << stopF_MMMin() 	<< " [mm/min]" 		<< std::endl;
						o << prefix << "Accel.stopThreshold     = " << stopF_MMSec() 	<< " [mm/sec]" 		<< std::endl;
					}
					
					void traceDynamicValues(std::ostream& o, int indent) {
						std::string prefix(indent, ' ');
						
						o << prefix << "Accel.active            = " << active 			<< " [<bool>]"		<< std::endl;
						o << prefix << "Accel.defaultValue      = " << defaultValue 	<< " [us]"			<< std::endl;
					}
					
					void determineDefaultValue(double feedrate) {
						if ( feedrate <= 0.0 ) {
							defaultValue = Acceleration().defaultValue;
							return;
						}
						
						defaultValue = round(1000 * 1000 / ( startF_MMSec() / feedrate) ); // us
					}
				};
				
				Stepper 		stepper;
				Acceleration	acceleration;
				
				// -----------------------------------------------------------------------------------
				Axis()
				: stepper		()
				, acceleration	()
				{}
				
				~Axis() 
				{}
				
				// -----------------------------------------------------------------------------------
				void reset() {
					stepper.		reset();
					acceleration.	reset();
				}
				
				// -----------------------------------------------------------------------------------
				bool isOk() 	{ return stepper.isOk() && acceleration.isOk();			}
				bool isStaOk()	{ return stepper.isStaOk() && acceleration.isStaOk();	}
				bool isDynOk()	{ return stepper.isDynOk() && acceleration.isDynOk();	}
				
				// -----------------------------------------------------------------------------------
				void traceStates(std::ostream& o, int indent) {
					int nextIndent = indent++;
		
					stepper			.traceState(o, nextIndent);
					acceleration	.traceState(o, nextIndent);
				}
				
				// -----------------------------------------------------------------------------------
				void trace(std::ostream& o, int indent) {
					int nextIndent = indent++;
		
					stepper			.trace(o, nextIndent);
					acceleration	.trace(o, nextIndent);
				}
				
				// -----------------------------------------------------------------------------------
				bool determineAcceleration(uint32_t distance) {
					
					if ( distance < 50 )
						1/ stepper.cnvF_MMMin_2_STMin(acceleration.startF_MMMin());
					
					//stepper.
					double t1 = stepper.cnvF_MMMin_2_STMin(acceleration.startF_MMMin());
					double t2 = stepper.cnvF_MMMin_2_STMin(acceleration.stopF_MMMin());
					
					
					#warning
					
					
					
					
					std::cout << "t1: " << t1 << std::endl;
					std::cout << "t2: " << t2 << std::endl;
					
					
					return false;
				}
				
				// -----------------------------------------------------------------------------------
				int32_t getMinimalDelay()  { return stepper.isStaOk()	? stepper.minPW							: 0.0; }
				int32_t getCurSpeedDelay() { return stepper.isDynOk()	? stepper.curF_PW() - stepper.minPW		: 0.0; }
				
				// -----------------------------------------------------------------------------------
				int32_t getCurAccelDelay(uint32_t step) {
					if ( isStaOk() == false )
						return 0;
					
					if ( stepper.cnvF_MMMin_2_STMin(acceleration.startF_MMMin()) >= stepper.curF_STSec() )
						return 0;
					
					#warning
					
				
					return 0; 
				}
				
				
				
		};
		
	protected:
		
		// -----------------------------------------------------------------------------------
		struct TestData {
			
			struct IN {
				int32_t	dxSteps			= 0;
				int32_t	dySteps			= 0;
				int32_t	dzSteps			= 0;
				double  f				= 0.0;
				
				bool	realtime		= false;
				
				int32_t getdxyzSteps()  { return ArdoObj::absolute(dxSteps) + ArdoObj::absolute(dySteps) + ArdoObj::absolute(dzSteps); }
				double  getdxyzVector() { return sqrt(dxSteps * dxSteps + dySteps * dySteps + dzSteps * dzSteps); }
				
			} input;
			
			struct OUT {
				struct Axis {
					uint32_t cntSteps		= 0;
				
					uint32_t minimalDelay	= 0;
					uint32_t curSpeedDelay	= 0;
					uint32_t curAccelDelay	= 0;
				};
				
				Axis 	 X, Y, Z;
				char 	 last				= '\0';
				uint32_t xyzIndex 			= 0;
				uint32_t xyzDelay			= 0;
				
			} output;
			
			void reset() {
				*this = TestData();
			}
			
			// -----------------------------------------------------------------------------------
			void report(std::ostream& o, int indent=0) {
				std::string prefix(indent, ' ');
				
				auto reportAxis = [&](char a, OUT::Axis A) {
					o << prefix << "Test." << a << ".cntStepsX               = " << A.cntSteps      << " [#]" 	<< std::endl;
					o << prefix << "Test." << a << ".minimalDelay            = " << A.minimalDelay  << " [us]" 	<< std::endl;
					o << prefix << "Test." << a << ".speedDelay              = " << A.curSpeedDelay << " [us]" 	<< std::endl;
					
				};
				
				o << prefix << "Test.F                  	= " << input.f          		<< " [mm/min]" 	<< std::endl;
				o << prefix << "Test.Distance[X,Y,Z]    	= " << input.dxSteps    		<< ","
																<< input.dySteps    		<< "," 
																<< input.dzSteps    		<< " [steps]" 	<< std::endl;
				o << prefix << "Test.Distance[|X|+|Y|+|Z|]	= " << input.getdxyzSteps()		<< " [steps]" 	<< std::endl;
				o << prefix << "Test.Distance[V->]      	= " << input.getdxyzVector()	<< " [steps]" 	<< std::endl;
				reportAxis('X', output.X);
				reportAxis('Y', output.Y);
				reportAxis('Z', output.Z);
			}
			
		};
		
		bool		initialized;
		Axis 		axis[3];
		TestData	testData;
		
		CncPulsWidthCtrl(const CncPulsWidthCtrl&);

	public:		
		
		typedef Axis::AID AXIS;

		CncPulsWidthCtrl() 
		: initialized(false)
		, axis()
		, testData()
		{}
		
		~CncPulsWidthCtrl() {
		}
		
		// -----------------------------------------------------------------------------------
		bool isOk() {
			return     initialized == true 
					&& axis[AXIS::X].isOk()
					&& axis[AXIS::Y].isOk()
					&& axis[AXIS::Z].isOk()
			;
		}
		
		// -----------------------------------------------------------------------------------
		bool isStaOk() {
			return     initialized == true 
					&& axis[AXIS::X].isStaOk()
					&& axis[AXIS::Y].isStaOk()
					&& axis[AXIS::Z].isStaOk()
			;
		}
		
		// -----------------------------------------------------------------------------------
		bool isDynOk() {
			return     initialized == true 
					&& axis[AXIS::X].isDynOk()
					&& axis[AXIS::Y].isDynOk()
					&& axis[AXIS::Z].isDynOk()
			;
		}
		
		// -----------------------------------------------------------------------------------
		void reset() {
			axis[AXIS::X].reset();
			axis[AXIS::Y].reset();
			axis[AXIS::Z].reset();
		}
		
		// -----------------------------------------------------------------------------------
		void setup() {
			AXIS id = AXIS::X;
			axis[id].stepper.steps 				=  800;
			axis[id].stepper.pitch 				= 12.0;
			axis[id].stepper.minPW 				=  325;
			axis[id].acceleration.startF		=  300;
			axis[id].acceleration.stopF			=  400;
			
			id = AXIS::Y;
			axis[id].stepper.steps 				=  800;
			axis[id].stepper.pitch 				= 12.0;
			axis[id].stepper.minPW 				=  325;
			axis[id].acceleration.startF		=  300;
			axis[id].acceleration.stopF			=  400;
			
			id = AXIS::Z;
			axis[id].stepper.steps 				=  800;
			axis[id].stepper.pitch 				= 12.0;
			axis[id].stepper.minPW 				=  325;
			axis[id].acceleration.startF		=  300;
			axis[id].acceleration.stopF			=  400;
			
			if ( axis[AXIS::X].isStaOk() )	axis[AXIS::X].acceleration.determineDefaultValue(axis[AXIS::X].stepper.feedrate());
			if ( axis[AXIS::Y].isStaOk() )	axis[AXIS::Y].acceleration.determineDefaultValue(axis[AXIS::Y].stepper.feedrate());
			if ( axis[AXIS::Z].isStaOk() )	axis[AXIS::Z].acceleration.determineDefaultValue(axis[AXIS::Z].stepper.feedrate());

			initialized = true;
		}
		
		// -----------------------------------------------------------------------------------
		void traceStates(std::ostream& o) {
			if ( axis[AXIS::X].isOk() == false ) {
				o << "Axis: X" << std::endl;
				axis[AXIS::X].traceStates(o, 1);
			}
			if ( axis[AXIS::Y].isOk() == false ) {
				o << "Axis: Y" << std::endl;
				axis[AXIS::Y].traceStates(o, 1);
			}
			if ( axis[AXIS::Z].isOk() == false ) {
				o << "Axis: Z" << std::endl;
				axis[AXIS::Z].traceStates(o, 1);
			}
		}
		
		// -----------------------------------------------------------------------------------
		void trace(std::ostream& o) {
			o << "Axis: X" << std::endl;
			axis[AXIS::X].trace(o, 1);

			o << "Axis: Y" << std::endl;
			axis[AXIS::Y].trace(o, 1);

			o << "Axis: Z" << std::endl;
			axis[AXIS::Z].trace(o, 1);
		}
		
		// -----------------------------------------------------------------------------------
		bool test(double f_MMMin, double dx, double dy, double dz) {
			const double frx = axis[AXIS::X].stepper.feedrate();
			const double fry = axis[AXIS::Y].stepper.feedrate();
			const double frz = axis[AXIS::Z].stepper.feedrate();
				
			const int32_t x = round(dx / frx);
			const int32_t y = round(dy / fry);
			const int32_t z = round(dz / frz);
			
			//std::cout << "Test convert " << dx << " to " << x <<  std::endl;
			//std::cout << "Test convert " << dy << " to " << y <<  std::endl;
			//std::cout << "Test convert " << dz << " to " << z <<  std::endl;
				
			return test(f_MMMin, x, y, z);
		}
		
		// -----------------------------------------------------------------------------------
		bool test(double f_MMMin, int32_t dx, int32_t dy, int32_t dz) {
			std::cout << "\n// Test start:" << std::endl;
			
			if ( isStaOk() == false ) {
				std::cerr << "CncPulsWidthCtrl.isStaOK() == false" << std::endl;	
				traceStates(std::cerr);
				return false;
			}
			
			reset();
			
			// setup stepper
			axis[AXIS::X].stepper.curF_MMMin = f_MMMin;
			axis[AXIS::Y].stepper.curF_MMMin = f_MMMin;
			axis[AXIS::Z].stepper.curF_MMMin = f_MMMin;
			
			// setup accelaration
			axis[AXIS::X].determineAcceleration(ArdoObj::absolute(dx));
			axis[AXIS::Y].determineAcceleration(ArdoObj::absolute(dy));
			axis[AXIS::Z].determineAcceleration(ArdoObj::absolute(dz));
			
			if ( isDynOk() == false ) {
				std::cerr << "CncPulsWidthCtrl.isDynOK() == false" << std::endl;
				traceStates(std::cerr);
				return false;
			}
			
			// prepare test data
			testData.reset();
			testData.input.realtime	= false;
			testData.input.f 		= f_MMMin;
			testData.input.dxSteps 	= dx;
			testData.input.dySteps 	= dy;
			testData.input.dzSteps 	= dz;
	
			testData.output.X.minimalDelay	= axis[AXIS::X].getMinimalDelay();
			testData.output.X.curSpeedDelay	= axis[AXIS::X].getCurSpeedDelay();
			testData.output.Y.minimalDelay	= axis[AXIS::Y].getMinimalDelay();
			testData.output.Y.curSpeedDelay	= axis[AXIS::Y].getCurSpeedDelay();
			testData.output.Z.minimalDelay	= axis[AXIS::Z].getMinimalDelay();
			testData.output.Z.curSpeedDelay	= axis[AXIS::Z].getCurSpeedDelay();
			
			// simulate rendering and stepping
			byte ret = RET_ERROR;
			if ( initializeMove() )
				ret = renderMove(dx, dy, dz);
			
			if ( ret == RET_OK ) 
				finalizeMove();
			
			testData.report(std::cout, 1);
			return ret = RET_OK;
		}
		
		// -----------------------------------------------------------------------------------
		bool initializeMove() {
			// todo
			return true;
		}
		
		// -----------------------------------------------------------------------------------
		bool finalizeMove() {
			// todo
			return true;
		}
		
		// -----------------------------------------------------------------------------------
		void performNextStep() {
			// todo
			// draw, log, whatever, etc.
			// based on testData.
			
			switch ( testData.output.last ) {
				case 'X': testData.output.xyzIndex; /*...*/ break;
				case 'Y': testData.output.xyzIndex; /*...*/ break;
				case 'Z': testData.output.xyzIndex; /*...*/ break;
			}
		}
		
		// -----------------------------------------------------------------------------------
		// ArduinoPositionRenderer interface
		virtual void notifyPositionChange()          {}
		virtual byte checkRuntimeEnv()               { return RET_OK; }
		virtual byte setDirectionX(int32_t steps)    { return RET_OK; }
		virtual byte setDirectionY(int32_t steps)    { return RET_OK; }
		virtual byte setDirectionZ(int32_t steps)    { return RET_OK; }
		
		// -----------------------------------------------------------------------------------
		// ArduinoPositionRenderer interface
		virtual byte performNextStepX() { 
			testData.output.X.curAccelDelay = axis[AXIS::X].getCurAccelDelay(testData.output.X.cntSteps);
			testData.output.X.cntSteps++;
			
			testData.output.xyzIndex++;
			testData.output.xyzDelay += testData.output.X.curAccelDelay;
			
			testData.output.last = 'X';
			performNextStep();
			return RET_OK; 
		}
		
		// -----------------------------------------------------------------------------------
		// ArduinoPositionRenderer interface
		virtual byte performNextStepY() { 
			testData.output.X.curAccelDelay = axis[AXIS::Y].getCurAccelDelay(testData.output.Y.cntSteps);
			testData.output.Y.cntSteps++;
			
			testData.output.xyzIndex++;
			testData.output.xyzDelay += testData.output.Y.curAccelDelay;
			
			testData.output.last = 'Y';
			performNextStep();
			return RET_OK; 
		}
		
		// -----------------------------------------------------------------------------------
		// ArduinoPositionRenderer interface
		virtual byte performNextStepZ() { 
			testData.output.X.curAccelDelay = axis[AXIS::Z].getCurAccelDelay(testData.output.Z.cntSteps);
			testData.output.Z.cntSteps++;
			
			testData.output.xyzIndex++;
			testData.output.xyzDelay += testData.output.Z.curAccelDelay;
			
			testData.output.last = 'Z';
			performNextStep();
			return RET_OK; 
		}
		
};

// ---------------------------------------------------
int main() {
	
	std::cout << "Start" << std::endl;
		
		CncPulsWidthCtrl ctrl;
		ctrl.setup();
		
		if ( ctrl.isStaOk() == false ) {
			std::cerr << "CncPulsWidthCtrl isn't OK!" << std::endl;
			ctrl.traceStates(std::cerr);
			
			return 1;
		}
		
		ctrl.trace(std::cout);
		std::cout << "CncPulsWidthCtrl initial trace finalized . . ." << std::endl << std::endl;
			
		//ctrl.test(725.0, 10, 20, 1);
		ctrl.test(725.0, 10.0, 20.0, -1.0);
		
	std::cout << "Quit" << std::endl;
}