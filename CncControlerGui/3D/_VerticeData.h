#ifndef VERTICE_DATA_H
#define VERTICE_DATA_H

#include "CncPosition.h"
#include "3D/GLCncPathData.h"

namespace GLI {
	
	template <typename T, const int32_t T_MIN, const int32_t T_MAX> 
	class VerticeData {

		public:
			
			//////////////////////////////////////////////
			VerticeData<T, T_MIN, T_MAX>()
			: _id(-1L)
			, _x(0)
			, _y(0)
			, _z(0)
			, _mode(GLI::GLCncPathVertices::CncMode::CM_RAPID)
			{}

			//////////////////////////////////////////////
			virtual ~VerticeData<T, T_MIN, T_MAX>() {
			}
			
			//////////////////////////////////////////////
			bool isIdValid()   const { return _id >= 0; }
			const long getId() const { return _id; }

			//////////////////////////////////////////////
			const T getX() const { return _x; }
			const T getY() const { return _y; }
			const T getZ() const { return _z; }
			
			//////////////////////////////////////////////
			const GLI::GLCncPathVertices::CncMode getMode() const { return _mode; }
			
			//////////////////////////////////////////////
			bool somethingDifferent(const VerticeData& toCompare) {
				if ( _x != toCompare.getX() ) 			return true;
				if ( _y != toCompare.getY() ) 			return true;
				if ( _z != toCompare.getZ() ) 			return true;
				if ( _mode != toCompare.getMode() ) 	return true;
				return false;
			}
			
			//////////////////////////////////////////////
			void setVertice(long id, CncSpeedMode speedType, const CncPosition<T, T_MIN, T_MAX>& cp) {
				_id = id;
				
				_x = cp.getX();
				_y = cp.getY();
				_z = cp.getZ();

				typedef GLI::GLCncPathVertices::CncMode Mode;
				switch ( speedType ) {
					case CncSpeedWork: 			_mode = Mode::CM_WORK; 	break;
					case CncSpeedRapid: 		_mode = Mode::CM_RAPID;	break;
					case CncSpeedMax: 			_mode = Mode::CM_WORK;	break;
					case CncSpeedUserDefined: 	_mode = Mode::CM_WORK;	break;
				}
			}
		
		protected:
			long _id;
			
			T _x;
			T _y;
			T _z;
			GLI::GLCncPathVertices::CncMode _mode;
	};
	
	typedef VerticeData<int32_t, INT32_MIN, INT32_MAX> VerticeLongData;
	typedef VerticeData<double,  INT32_MIN, INT32_MAX>  VerticeDoubleData;
}
	
#endif
