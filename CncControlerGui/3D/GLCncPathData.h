#ifndef OPENGL_CNC_PATH_DATA_H
#define OPENGL_CNC_PATH_DATA_H

#include <cfloat>
#include <vector>
#include <wx/colour.h>
#include "CncCommon.h"
#include "3D/GLOpenGLPathBufferStore.h"

namespace GLI {
	
	////////////////////////////////////////////////////////////////
	class GLCncPathVertices {
		
		public:
			
			enum CncMode {
				CM_RAPID, 
				CM_WORK,
				CM_MAX,
				CM_USER_DEFINED
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
			static const char getCncModeAsChar(CncMode m) {
				static char ret;
				switch( m ) {
					case CM_WORK: 			ret = 'W'; break;
					case CM_RAPID:			ret = 'R'; break;
					case CM_MAX:			ret = 'M'; break;
					case CM_USER_DEFINED:	ret = 'U'; break;
				}
				return ret;
			}
			
			////////////////////////////////////////////
			static const wxString& getCncModeAsString(CncMode m) {
				static wxString ret;
				switch( m ) {
					case CM_WORK: 			ret.assign(cnc::WORK_SPEED_CHAR); 		break;
					case CM_RAPID:			ret.assign(cnc::RAPID_SPEED_CHAR);		break;
					case CM_MAX:			ret.assign(cnc::MAX_SPEED_CHAR); 		break;
					case CM_USER_DEFINED:	ret.assign(cnc::USER_DEFIND_SPEED_CHAR); break;
				}
				return ret;
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
	class GLCncPath {
		
		public:
			
			/////////////////////////////////////////////////////////
			struct Callback {
				
				Callback() {}
				virtual ~Callback() {}
				
				virtual void notifyCncPathChanged() = 0;
			};
			
			void registerCallback(GLI::GLCncPath::Callback* cb) {
				if ( cb == NULL )
					return;
					
				callbacks.push_back(cb); 
				
				// release the first callback
				cb->notifyCncPathChanged();
			}
			
			////////////////////////////////////////////
			explicit GLCncPath(const wxString& instanceName);
			virtual ~GLCncPath();
			
			////////////////////////////////////////////
			void setVirtualEndToFirst() 	{ setVirtualEnd(1); }
			void setVirtualEndToLast() 		{ setVirtualEnd(vectiesBuffer.getVertexCount()); }
			
			////////////////////////////////////////////
			bool hasNextVertex() const;
			bool hasPreviousVertex() const;
			
			////////////////////////////////////////////
			long previewNextVertexId();
			long previewPreviousVertexId();
			
			////////////////////////////////////////////
			void incVirtualEnd()			{ setVirtualEnd(getVirtualEnd() + 1 ); }
			void decVirtualEnd() 			{ setVirtualEnd(getVirtualEnd() - 1 ); }
			void incVirtualEndById();
			void decVirtualEndById();
			
			void spoolVertiesForCurrentId();
			
			////////////////////////////////////////////
			void setVirtualEnd(long val);
			
			////////////////////////////////////////////
			const long getVirtualEnd() const { return virtualEnd; }
			const long getVirtualEndAsId();
			
			////////////////////////////////////////////
			void activateNotifications(bool state = true) { publishNotifications = state; }
			void deactivateNotifications() { activateNotifications(false); }
			
			////////////////////////////////////////////
			const GLCncPathVertices& getMin() const { return minVecties; }
			const GLCncPathVertices& getMax() const { return maxVecties; }
			
			////////////////////////////////////////////
			const BoundBox& getBoundBox() {
				static BoundBox bBox;
				bBox.clear();
				
				// a bound box of 2 or less points didn't make sense
				if ( vectiesBuffer.getVertexCount() < 3 )
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
				if ( vectiesBuffer.getVertexCount() < 1 )
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
			unsigned long size()										const	{ return vectiesBuffer.getVertexCount(); }
			GLOpenGLPathBufferStore* getOpenGLBufferStore() 					{ return &vectiesBuffer; }
			void reconstruct(const GLOpenGLPathBuffer::ReconstructOptions& opt)	{ vectiesBuffer.reconstruct(opt); }
			void setColours(const GLOpenGLPathBuffer::VertexColours& colours) 	{ vectiesBuffer.setColours(colours); }
			void display(GLOpenGLPathBuffer::DisplayType dt);
			
			void clear();
			void appendPathData(const GLOpenGLPathBuffer::CncVertex& v);
		
		private:
			GLOpenGLPathBufferStore					vectiesBuffer;
			GLCncPathVertices 						minVecties;
			GLCncPathVertices 						maxVecties;
			
			long 									virtualEnd;
			
			bool									publishNotifications;
			std::vector<GLI::GLCncPath::Callback*> 	callbacks;
			
			////////////////////////////////////////////
			inline void updateVirtualEnd() { setVirtualEnd(vectiesBuffer.getVertexCount()); }
			
			////////////////////////////////////////////
			void notifyCncPathChanged() {
				
				if ( publishNotifications == false )
					return;
				
				for ( auto it = callbacks.begin(); it != callbacks.end(); it++ ) {
					if ( (*it) == NULL )
						continue;
						
					(*it)->notifyCncPathChanged();
				}
			}
	};
	
}; // namespace GLI

#endif