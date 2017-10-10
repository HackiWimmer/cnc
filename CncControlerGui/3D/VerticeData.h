#ifndef VERTICE_DATA_H
#define VERTICE_DATA_H

#include "CncPosition.h"
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
			, _mode(GLI::GLCncPathVertices::CncMode::CM_FLY)
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
			void setVertice(long id, CncSpeed speedType, const CncPosition<T>& cp) {
				_id = id;
				
				switch ( speedType ) {
					case CncSpeedWork: 	setWorkVertice(cp);
										break;
										
					case CncSpeedRapid: setRapidVertice(cp);
										break;
				}
			}
		
		protected:
			long _id;
			
			T _x;
			T _y;
			T _z;
			GLI::GLCncPathVertices::CncMode _mode;
			
			//////////////////////////////////////////////
			void setRapidVertice(const CncPosition<T>& cp) {
				_x = cp.getX();
				_y = cp.getY();
				_z = cp.getZ();
				
				_mode = GLI::GLCncPathVertices::CncMode::CM_FLY;
			}

			//////////////////////////////////////////////
			void setWorkVertice(const CncPosition<T>& cp) {
				_x = cp.getX();
				_y = cp.getY();
				_z = cp.getZ();
				
				_mode = GLI::GLCncPathVertices::CncMode::CM_WORK;
			}
	};

	
	typedef VerticeData<int32_t> VerticeLongData;
	typedef VerticeData<double>  VerticeDoubleData;
}
	
#endif