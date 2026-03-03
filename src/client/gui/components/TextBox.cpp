#include "TextBox.h"
#include "../../Minecraft.h"
#include "../../renderer/Tesselator.h"
#include "../../../AppPlatform.h"

TextBox::TextBox( int id, const std::string& msg )
 : id(0), w(0), h(0), x(0), y(0), text(msg), focused(false) {

}

TextBox::TextBox( int id, int x, int y, const std::string& msg ) 
 : id(id), w(0), h(0), x(x), y(y), text(msg), focused(false) {

}

TextBox::TextBox( int id, int x, int y, int w, int h, const std::string& msg )
 : id(id), w(w), h(h), x(x), y(y), text(msg), focused(false) {

}

void TextBox::setFocus(Minecraft* minecraft) {
	if(!focused) {
		minecraft->platform()->showKeyboard();
		focused = true;
	}
}

bool TextBox::loseFocus(Minecraft* minecraft) {
	if(focused) {
		minecraft->platform()->showKeyboard();
		focused = false;
		return true;
	}
	return false;
}

void TextBox::render( Minecraft* minecraft, int xm, int ym ) {
	int bgCol = focused ? 0x80808080 : 0x80000000;
	fill(x, y, x + w, y + h, bgCol);
	
	int borderCol = focused ? 0xFFFFFFFF : 0xFF707070;
	fill(x, y, x + w, y + 1, borderCol);
	fill(x, y + h - 1, x + w, y + h, borderCol);
	fill(x, y, x + 1, y + h, borderCol);
	fill(x + w - 1, y, x + w, y + h, borderCol);
	
	std::string displayText = text;
	if (focused) {
		int time = (int)(getTimeS() * 2);
		if (time % 2 == 0) {
			displayText += "|";
		}
	}
	
	if (displayText.length() > 0) {
		drawString(minecraft->font, displayText, x + 4, y + (h - 8) / 2, 0xFFFFFFFF);
	}
}
