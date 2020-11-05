#ifndef VERTICE_DATA_H
#define VERTICE_DATA_H

#include "CncPosition.h"
#include "CncCommon.h"
#include "3D/GLCncPathData.h"

namespace GLI {
	
	template <typename T>
	class VerticeData {

		public:
			
			//////////////////////////////////////////////
			VerticeData<T>()
			: _id(-1L)
			, _x(0)
			, _y(0)
			, _z(0)
			, _mode(CncSpeedRapid)
			{}

			//////////////////////////////////////////////
			virtual ~VerticeData<T>() {
			}
			
			//////////////////////////////////////////////
			bool isIdValid()   const { return _id >= 0; }
			const long getId() const { return _id; }

			//////////////////////////////////////////////
			const T getX() const { return _x; }
			const T getY() const { return _y; }
			const T getZ() const { return _z; }
			
			//////////////////////////////////////////////
			const CncSpeedMode getSpeedMode() const { return _mode; }
			
			//////////////////////////////////////////////
			const GLI::GLCncPathVertices::CncMode getMode() const { 
				typedef GLI::GLCncPathVertices::CncMode Mode;
				switch ( _mode ) {
					case CncSpeedWork: 			return Mode::CM_WORK;
					case CncSpeedRapid: 		return Mode::CM_RAPID;
					case CncSpeedMax: 			return Mode::CM_WORK;
					case CncSpeedUserDefined: 	return Mode::CM_USER_DEFINED;
				}
				
				return Mode::CM_RAPID;
			}
			
			//////////////////////////////////////////////
			bool somethingDifferent(const VerticeData& toCompare) {
				if ( _x    != toCompare.getX() ) 			return true;
				if ( _y    != toCompare.getY() ) 			return true;
				if ( _z    != toCompare.getZ() ) 			return true;
				if ( _mode != toCompare.getSpeedMode() ) 	return true;
				return false;
			}
			
			//////////////////////////////////////////////
			void setVertice(long id, CncSpeedMode speedType, const CncPosition<T>& cp) {
				_id = id;
				
				_x = cp.getX();
				_y = cp.getY();
				_z = cp.getZ();
				
				_mode = speedType;
			}
		
		protected:
			long _id;
			
			T _x;
			T _y;
			T _z;
			CncSpeedMode _mode;
	};
	
	typedef VerticeData<int32_t> VerticeLongData;
	typedef VerticeData<double>  VerticeDoubleData;
}
	
#endif
