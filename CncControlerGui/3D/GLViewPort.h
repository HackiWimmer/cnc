#ifndef GL_VIEW_PORT_H
#define GL_VIEW_PORT_H

class GLViewPort {

	public:
		enum DistortType {VPT_Distored, VPT_Undistored};
		enum OrigPosType {VPOP_TopLeft, VPOP_TopRight, VPOP_BottomLeft, VPOP_BottomRight, VPOP_Center, VPOP_Custom};

		////////////////////////////////////////////////////
		GLViewPort(GLViewPort::DistortType type)
		: x(0), y(0), w(0), h(0), n(0)
		, distortType(type)
		, origPosType(OrigPosType::VPOP_Center)
		{}

		////////////////////////////////////////////////////
		~GLViewPort() {
		}

		////////////////////////////////////////////////////
		int getX() { return x; }
		int getY() { return y; }
		int getW() { return w; }
		int getH() { return h; }

		////////////////////////////////////////////////////
		int getNormalizedSizeW() {
			if ( distortType == GLViewPort::VPT_Undistored )
				return n;

			return w;
		}

		////////////////////////////////////////////////////
		int getNormalizedSizeH() {
			if ( distortType == GLViewPort::VPT_Undistored )
				return n;

			return h;
		}

		////////////////////////////////////////////////////
		GLViewPort::DistortType getDistortType() { return distortType; }

		////////////////////////////////////////////////////
		GLViewPort::OrigPosType getOrigPosType() { return origPosType; }
		void setOrigPosType(GLViewPort::OrigPosType opt) { origPosType = opt; }

		////////////////////////////////////////////////////
		void evaluate(int wndSizeW,
					  int wndSizeH,
					  int custPosX=0,
					  int custPosY=0) {

			switch ( origPosType ) {

				case VPOP_Custom: 		x = (custPosX == 0 ? getX() : custPosX );
										y = (custPosY == 0 ? getY() : custPosY );
										w = wndSizeW * 2;
										h = wndSizeH * 2;
										break;
				
				case VPOP_Center:
				default: 				x = 0;
										y = 0;
										w = wndSizeW;
										h = wndSizeH;
										break;
/*
				case VPOP_TopLeft: 		x = -1 * (wndSizeW / 2 - margin);
										y = -1 * (margin);
										w = wndSizeW - 2 * margin;
										h = wndSizeH - 2 * margin;;
										break;

				case VPOP_TopRight: 	x = -1 * (margin);
										y = -1 * (margin);
										w = wndSizeW - 2 * margin;
										h = wndSizeH - 2 * margin;;
										break;

				case VPOP_BottomLeft: 	x = -1 * (wndSizeW / 2 - margin);
										y = -1 * (wndSizeH / 2 - margin);
										w = wndSizeW - 2 * margin;
										h = wndSizeH - 2 * margin;;
										break;

				case VPOP_BottomRight: 	x = -1 * (margin);
										y = -1 * (wndSizeH / 2 - margin);
										w = wndSizeW - 2 * margin;
										h = wndSizeH - 2 * margin;;
										break;
*/
			}

			// determine the normalized size
			w > h ? n = h : n = w;
		}

	private:

		static const unsigned int margin = 10;

		int x, y, w, h, n;
		GLViewPort::DistortType distortType;
		GLViewPort::OrigPosType origPosType;

};

#endif