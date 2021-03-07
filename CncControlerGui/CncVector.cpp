#include "CncVector.h"

namespace TEST {
	
	struct xxx {
		
		xxx() {
			CncFloatVector ux(1.0, 0.0, 0.0);
			CncFloatVector uy(0.0, 1.0, 0.0);
			CncFloatVector uz(0.0, 0.0, 1.0);
			
			CncFloatVector v1(-1.0, -1.0, 0.0);
			
			std::cout << "CncVector Test" << std::endl;
			
			std::cout << CncFloatVector::radias2Degree(v1.getAlphaTo(ux)) << std::endl;
			std::cout << CncFloatVector::radias2Degree(v1.getAlphaTo(uy)) << std::endl;
			std::cout << CncFloatVector::radias2Degree(v1.getAlphaTo(uz)) << std::endl;
			
			
			CncFloatMatrix4x4 mat;
			//mat.set(CncFloatMatrix4x4::V11, 0.0);
			mat.set(CncFloatMatrix4x4::V1, v1);
			
			std::cout << mat << std::endl;
			
			
			//abort();
		}
		
	} XXX;
};