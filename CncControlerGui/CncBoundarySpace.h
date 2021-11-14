#ifndef CNC_BOUNDARY_SPACE_H
#define CNC_BOUNDARY_SPACE_H

#include <iostream>
#include "CncCommon.h"
#include "CncPosition.h"

class CncBoundarySpace {
	
	public:
		
		class HardwareOriginOffset {
			
			private:
				bool valid = false;
				
				// stores the signed offset origin to Xmin, Ymin and Zmax
				CncLongDistance offset;
				
			public:
				
				HardwareOriginOffset();
				explicit HardwareOriginOffset(const HardwareOriginOffset& hwo);
				
				void					reset();
				void					set(int32_t px, int32_t py, int32_t pz);
				void 					set(double px, double py, double pz);
				void					set(const CncLongPosition&);
				void 					set(const CncDoublePosition&);
				
				bool					isValid()			const	{ return valid; }
				void					setValid(bool s)			{ valid = s; }
				
				const CncLongDistance&	getAsSteps()		const	{ return offset; }
				int32_t					getAsStepsX()		const	{ return offset.getX(); }
				int32_t					getAsStepsY()		const	{ return offset.getY(); }
				int32_t					getAsStepsZ()		const	{ return offset.getZ(); }
				
				CncDoubleDistance		getAsMetric()		const;
				double					getAsMetricX()		const;
				double					getAsMetricY()		const;
				double					getAsMetricZ()		const;
				
				CncLongPosition			transLog2Phy(const CncLongPosition& logicPos) const;
				CncDoublePosition		transLog2Phy(const CncDoublePosition& logicPos) const;
				
				CncLongPosition			transPhy2Log(const CncLongPosition& physicalPos) const;
				CncDoublePosition		transPhy2Log(const CncDoublePosition& physicalPos) const;
		};

	
	private:
		
		HardwareOriginOffset	hardwareOffset;
		CncDoubleDistance		calculateRefPosition;
		
		// without harware reference
		CncDoubleBounderies		boundLogically;
		
		// harware normalized
		CncDoubleBounderies		boundPhysically;
		
		CncRefPositionMode		refPostionMode;
		CncDoubleOffset 		measurementOffset;
		double					workpieceThickness;
		
		void					calculate();
		
	public:
		// -----------------------------------------------------------
		CncBoundarySpace();
		
		// -----------------------------------------------------------
		// copy semantic
		CncBoundarySpace(const CncBoundarySpace&) = delete;
		CncBoundarySpace& operator= (const CncBoundarySpace&) = delete;
		
		// -----------------------------------------------------------
		// move semantic
		CncBoundarySpace(CncBoundarySpace&&) = delete;
		CncBoundarySpace& operator= (CncBoundarySpace&&) = delete;
		
		bool					isValid()										const;
		bool 					includesWpt()									const	{ return refPostionMode == CncRM_Mode6 || refPostionMode == CncRM_Mode6; }
		bool 					includesMpt()									const	{ return refPostionMode == CncRM_Mode6; }
		bool					hasHardwareOffset()								const;
		bool					isPositionInside(const CncLongPosition& p)		const;
		bool					isPositionInside(const CncDoublePosition& p)	const;
		
		int32_t					getMinStepsX() const;
		int32_t					getMinStepsY() const;
		int32_t					getMinStepsZ() const;
		int32_t					getMaxStepsX() const;
		int32_t					getMaxStepsY() const;
		int32_t					getMaxStepsZ() const;
		
		double					getMinMetricX() const;
		double					getMinMetricY() const;
		double					getMinMetricZ() const;
		double					getMaxMetricX() const;
		double					getMaxMetricY() const;
		double					getMaxMetricZ() const;
		
		CncRefPositionMode		getReferencePositionMode()						const	{ return refPostionMode; }
		double					getWorkpieceThickness()							const	{ return workpieceThickness; }
		double					getSurfaceOffset()								const	{ return workpieceThickness; }
		const CncDoubleOffset	getMeasurementOffset()							const	{ return measurementOffset; }

		const CncLongPosition	getCalculatedRefPositionSteps()					const;
		const CncDoublePosition	getCalculatedRefPositionMetric()				const;

		void					setRefPositionMode(CncRefPositionMode rpm)				{ refPostionMode		= rpm;		calculate(); }
		void					setWorkpieceThickness(double wpt)						{ workpieceThickness	= wpt;		calculate(); }
		void					setMeasurementOffset(CncDoubleOffset ofs)				{ measurementOffset		= ofs;		calculate(); }
		
		void					resetHardwareOffset()									{ hardwareOffset.reset(); 			calculate(); }
		void					setHardwareOffsetValid(bool s)							{ hardwareOffset.setValid(s); 		calculate(); }
		void					setHardwareOffset(int32_t px, int32_t py, int32_t pz)	{ hardwareOffset.set(px, py, pz);	calculate(); }
		void 					setHardwareOffset(double px, double py, double pz)		{ hardwareOffset.set(px, py, pz);	calculate(); }
		void					setHardwareOffset(const CncLongPosition& p)				{ hardwareOffset.set(p);			calculate(); }
		void 					setHardwareOffset(const CncDoublePosition& p)			{ hardwareOffset.set(p);			calculate(); }
		void 					setHardwareOffset(const HardwareOriginOffset& h)		{ hardwareOffset = h;				calculate(); }
		const HardwareOriginOffset&	getHardwareOffset()							const	{ return hardwareOffset; }
		
		void traceTo(std::ostream& o, unsigned int indent)						const;
		
};

#endif