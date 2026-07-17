//
// Created by arthu on 17/07/2026.
//

#ifndef HEXEDITOR_HEXEDITOR_H
#define HEXEDITOR_HEXEDITOR_H

#include <string>

class Buffer;
class HexEditor 
{
	public:
		HexEditor(){};
		virtual ~HexEditor();

		virtual int Init() = 0;
		virtual void Render( const Buffer& oBuffer, bool& bQuit ){};

	private:

		std::string Format( const char* sFormat,... );
		void DisplayDebugText( const Buffer& oBuffer );
};


#endif //HEXEDITOR_HEXEDITOR_H
