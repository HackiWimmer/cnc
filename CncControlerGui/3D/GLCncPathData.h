#ifndef OPENGL_CNC_PATH_DATA_H
#define OPENGL_CNC_PATH_DATA_H

#include <cfloat>
#include <vector>
#include <wx/colour.h>

namespace GLI {
	
	////////////////////////////////////////////////////////////////
	class GLCncPathVertices {
		
		public:
			
			enum CncMode {
				CM_FLY, 
				CM_WORK
			};
			
			enum FormatType { 
				FT_TRANSPARENT,
				FT_SOLID,
				FT_DOT,
				FT_LONG_DASH,
				FT_SHORT_DASH,
				FT_DOT_DASH
			};
			
			////////////////////////////////////////////
			GLCncPathVertices() 
			: _x(0)
			, _y(0)
			, _z(0)
			, _id(-1L)
			, colour(defaultColour)
			, formatType(defaultFormatType)
			, cncMode(defaultCncMode)
			{}

			////////////////////////////////////////////
			GLCncPathVertices(float x, float y, float z) 
			: _x(x)
			, _y(y)
			, _z(z)
			, _id(-1L)
			, colour(defaultColour)
			, formatType(defaultFormatType)
			, cncMode(defaultCncMode)
			{}
			
			////////////////////////////////////////////
			GLCncPathVertices(long id, float x, float y, float z, 
			                  const wxColour& col, 
							  GLCncPathVertices::FormatType ft,
							  GLCncPathVertices::CncMode cm) 
			: _x(x)
			, _y(y)
			, _z(z)
			, _id(id)
			, colour(col)
			, formatType(ft)
			, cncMode(cm)
			{}
			
			////////////////////////////////////////////
			GLCncPathVertices(const GLCncPathVertices& cpv) 
			: _x(cpv.getX())
			, _y(cpv.getY())
			, _z(cpv.getZ())
			, _id(cpv.getId())
			, colour(cpv.getColour())
			, formatType(cpv.getFormatType())
			, cncMode(cpv.getCncMode())
			{}
			
			////////////////////////////////////////////
			~GLCncPathVertices() {
			}
			
			////////////////////////////////////////////
			const float getX() const { return _x; }
			const float getY() const { return _y; }
			const float getZ() const { return _z; }
			
			////////////////////////////////////////////
			const long getId() const { return _id; }
			
			////////////////////////////////////////////
			const GLCncPathVertices::FormatType getFormatType() const { return formatType; }
			
			////////////////////////////////////////////
			const GLCncPathVertices::CncMode getCncMode() const { return cncMode; }
			
			////////////////////////////////////////////
			const wxColour& getColour(long refID=-1L) const { 
				if ( _id >= 0 && refID >= 0 ) {
					if ( _id == refID )
						return idColour;
				}
				
				return colour; 
			}
			
			////////////////////////////////////////////
			const GLCncPathVertices& set(long id, float x, float y, float z) {
				_x = x;
				_y = y;
				_z = z;
				
				id         = -1L;
				
				colour     = defaultColour;
				formatType = defaultFormatType;
				cncMode    = defaultCncMode;
				
				return *this;
			}
			
			////////////////////////////////////////////
			const GLCncPathVertices& set(long id, float x, float y, float z, 
			                             const wxColour& col,
			                             GLCncPathVertices::FormatType ft,
										 GLCncPathVertices::CncMode cm) {
				_x = x;
				_y = y;
				_z = z;
				
				_id = id;
				
				colour		= col;
				formatType	= ft;
				cncMode		= cm;
				
				return *this;
			}
			
			////////////////////////////////////////////
			const GLCncPathVertices& setX(float x) { _x = x; return *this; }
			const GLCncPathVertices& setY(float y) { _y = y; return *this; }
			const GLCncPathVertices& setZ(float z) { _z = z; return *this; }
			
		private:
			const wxColour 		defaultColour 		= wxColour(255,255,255);
			const FormatType 	defaultFormatType 	= FT_SOLID;
			const CncMode		defaultCncMode 		= CM_WORK;
			
			float _x;
			float _y;
			float _z;
			
			long _id;

			const wxColour idColour = wxColour(255, 0, 0);
			
			wxColour   	colour;
			FormatType 	formatType;
			CncMode		cncMode;
	};
	
	////////////////////////////////////////////////////////////////
	typedef std::pair<GLCncPathVertices, GLCncPathVertices> BoundBoxLine;
	typedef std::vector<BoundBoxLine> BoundBox; 
	
	////////////////////////////////////////////////////////////////
	class GLCncPath : public std::vector<GLCncPathVertices> {
		
		public:
			////////////////////////////////////////////
			GLCncPath() 
			: std::vector<GLCncPathVertices>()
			, minVecties(FLT_MAX, FLT_MAX, FLT_MAX)
			, maxVecties(FLT_MIN, FLT_MIN, FLT_MIN)
			{}
			
			////////////////////////////////////////////
			virtual ~GLCncPath() {
			}
			
			////////////////////////////////////////////
			const GLCncPathVertices& getMin() const { return minVecties; }
			const GLCncPathVertices& getMax() const { return maxVecties; }
			
			////////////////////////////////////////////
			const BoundBox& getBoundBox() {
				static BoundBox bBox;
				bBox.clear();
				
				// a bound box of 2 or less points didn't make sense
				if ( size() < 3 )
					return bBox;
				
				const float x = minVecties.getX(); float X = maxVecties.getX();
				const float y = minVecties.getY(); float Y = maxVecties.getY();
				const float z = minVecties.getZ(); float Z = maxVecties.getZ();
				
				// bottom - push_back(BoundBoxLine)
				bBox.push_back(std::make_pair(GLCncPathVertices(x,y,z), GLCncPathVertices(X,y,z)));
				bBox.push_back(std::make_pair(GLCncPathVertices(X,y,z), GLCncPathVertices(X,y,Z)));
				bBox.push_back(std::make_pair(GLCncPathVertices(X,y,Z), GLCncPathVertices(x,y,Z)));
				bBox.push_back(std::make_pair(GLCncPathVertices(x,y,Z), GLCncPathVertices(x,y,z)));
				
				// top - push_back(BoundBoxLine)
				bBox.push_back(std::make_pair(GLCncPathVertices(x,Y,z), GLCncPathVertices(X,Y,z)));
				bBox.push_back(std::make_pair(GLCncPathVertices(X,Y,z), GLCncPathVertices(X,Y,Z)));
				bBox.push_back(std::make_pair(GLCncPathVertices(X,Y,Z), GLCncPathVertices(x,Y,Z)));
				bBox.push_back(std::make_pair(GLCncPathVertices(x,Y,Z), GLCncPathVertices(x,Y,z)));
				
				// perpendicular - push_back(BoundBoxLine)
				bBox.push_back(std::make_pair(GLCncPathVertices(x,y,z), GLCncPathVertices(x,Y,z)));
				bBox.push_back(std::make_pair(GLCncPathVertices(X,y,z), GLCncPathVertices(X,Y,z)));
				bBox.push_back(std::make_pair(GLCncPathVertices(X,y,Z), GLCncPathVertices(X,Y,Z)));
				bBox.push_back(std::make_pair(GLCncPathVertices(x,y,Z), GLCncPathVertices(x,Y,Z)));
				
				return bBox;
			}
			
			////////////////////////////////////////////
			const float getAutoScaleFact() {
				if ( size() < 1 )
					return 0.1;
					
				const float x = minVecties.getX(); float X = maxVecties.getX();
				const float y = minVecties.getY(); float Y = maxVecties.getY();
				const float z = minVecties.getZ(); float Z = maxVecties.getZ();
				
				float totalDistX = X - x;
				float totalDistY = Y - y;
				float totalDistZ = Z - z;
				
				// range: -2 >= ret <= 2
				float ret = std::max(std::max(totalDistZ, totalDistY), totalDistX);
				if ( ret < 0.1 )
					return 0.1;
				
				return ret;
			}
			
			////////////////////////////////////////////
			void clear() {
				// reset boundings
				minVecties.set(-1L, FLT_MAX, FLT_MAX, FLT_MAX);
				maxVecties.set(-1L, FLT_MIN, FLT_MIN, FLT_MIN);
				
				std::vector<GLCncPathVertices>::clear();
			}
			
			////////////////////////////////////////////
			void push_back(const GLCncPathVertices& v) {
				
				minVecties.setX(std::min(v.getX(), minVecties.getX()));
				minVecties.setY(std::min(v.getY(), minVecties.getY()));
				minVecties.setZ(std::min(v.getZ(), minVecties.getZ()));
				
				maxVecties.setX(std::max(v.getX(), maxVecties.getX()));
				maxVecties.setY(std::max(v.getY(), maxVecties.getY()));
				maxVecties.setZ(std::max(v.getZ(), maxVecties.getZ()));
				
				std::vector<GLCncPathVertices>::push_back(v);
			}
		
		private:
			GLCncPathVertices 	minVecties;
			GLCncPathVertices 	maxVecties;
	};
	
}; // namespace GLI

#endif