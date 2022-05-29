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
			
			explicit GLCncPath(const wxString& instanceName);
			virtual ~GLCncPath();
			
			void setVirtualEndToFirst() 	{ setVirtualEnd(1); }
			void setVirtualEndToLast() 		{ setVirtualEnd(vectiesBuffer.getVertexCount()); }
			
			bool hasNextVertex() const;
			bool hasPreviousVertex() const;
			
			long previewNextVertexId();
			long previewPreviousVertexId();
			
			long previewNextVertexNormalizedId();
			long previewPreviousVertexNormalizedId();

			void incVirtualEnd()			{ setVirtualEnd(getVirtualEnd() + 1 ); }
			void decVirtualEnd() 			{ setVirtualEnd(getVirtualEnd() - 1 ); }
			void incVirtualEndById();
			void decVirtualEndById();
			
			void setVirtualEnd(long val);
			
			const long getVirtualEnd() const { return virtualEnd; }
			const long getVirtualEndAsId();
			const long getVirtualEndAsNormalizedId();
			
			void activateNotifications(bool state = true) { publishNotifications = state; }
			void deactivateNotifications() { activateNotifications(false); }
			
			const GLCncPathVertices& getMin() const { return minVecties; }
			const GLCncPathVertices& getMax() const { return maxVecties; }
			
			const BoundBox& getBoundBox();
			const BoundBox& getObjectBoundBox();
			
			void createVertexArray()											{ vectiesBuffer.createVertexArray(); }
			void destroyVertexArray()											{ vectiesBuffer.destroyVertexArray(); }
			
			unsigned long size()										const	{ return vectiesBuffer.getVertexCount(); }
			GLOpenGLPathBufferStore* getOpenGLBufferStore() 					{ return &vectiesBuffer; }
			void reconstruct(const GLOpenGLPathBuffer::ReconstructOptions& opt)	{ vectiesBuffer.reconstruct(opt); }
			void setColours(const GLOpenGLPathBuffer::VertexColours& colours) 	{ vectiesBuffer.setColours(colours); }
			void display(GLOpenGLPathBuffer::DisplayType dt);
			
			void clear();
			void appendPathData(const GLOpenGLPathBuffer::CncVertex& v);
		
		private:
			GLOpenGLPathBufferStore					vectiesBuffer;
			GLCncPathVertices						minVecties;
			GLCncPathVertices						maxVecties;
			
			GLCncPathVertices						minObjVecties;
			GLCncPathVertices						maxObjVecties;
			
			long 									virtualEnd;
			
			bool									publishNotifications;
			std::vector<GLI::GLCncPath::Callback*> 	callbacks;

			inline const BoundBox& evaluateBoundBox(const GLCncPathVertices& min, const GLCncPathVertices& max);
			
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