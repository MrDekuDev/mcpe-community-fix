#include "TouchJoinGameScreen.h"
#include "../StartMenuScreen.h"
#include "../ProgressScreen.h"
#include "../../Font.h"
#include "../../../Minecraft.h"
#include "../../../renderer/Textures.h"
#include <cstdlib>

namespace Touch {

void AvailableGamesList::selectStart( int item) {
	startSelected = item;
}

void AvailableGamesList::selectCancel() {
	startSelected = -1;
}

void AvailableGamesList::selectItem( int item, bool doubleClick ) {
	LOGI("selected an item! %d\n", item);
	selectedItem = item;
}

void AvailableGamesList::renderItem( int i, int x, int y, int h, Tesselator& t )
{
	if (startSelected == i && Multitouch::getFirstActivePointerIdEx() >= 0) {
		fill((int)x0, y, (int)x1, y+h, 0x809E684F);
	}

	const PingedCompatibleServer& s = copiedServerList[i];
	unsigned int color  = s.isSpecial? 0x6090a0 : 0xffffb0;
	unsigned int color2 = 0xffffa0;

	int xx1 = (int)x0 + 24;
	int xx2 = xx1;

	if (s.isSpecial) {
		xx1 += 50;

		glEnable2(GL_TEXTURE_2D);
        glColor4f2(1,1,1,1);
        glEnable2(GL_BLEND);
		minecraft->textures->loadAndBindTexture("gui/badge/minecon140.png");
		blit(xx2, y + 6, 0, 0, 37, 8, 140, 240);
	}

	drawString(minecraft->font, s.name.C_String(), xx1, y + 4 + 2, color);
	drawString(minecraft->font, s.address.ToString(false), xx2, y + 18, color2);
}


JoinGameScreen::JoinGameScreen()
:	bJoin(  2, "Join Game"),
	bBack(  3, "Back"),
	bHeader(0, ""),
	bConnect(4, "Connect"),
	gamesList(NULL),
	ipTextBox(NULL)
{
	bJoin.active = false;
}

JoinGameScreen::~JoinGameScreen()
{
	delete gamesList;
	delete ipTextBox;
}

void JoinGameScreen::init()
{
	buttons.push_back(&bBack);
	buttons.push_back(&bHeader);
	buttons.push_back(&bConnect);

	minecraft->raknetInstance->clearServerList();
	gamesList = new AvailableGamesList(minecraft, width, height);
	
	ipTextBox = new TextBox(100, 0, 0, 200, 20, "");
	textBoxes.push_back(ipTextBox);

#ifdef ANDROID
	tabButtons.push_back(&bBack);
	tabButtons.push_back(&bConnect);
#endif
}

void JoinGameScreen::setupPositions() {
	bJoin.y = 0;
	bBack.y = 0;
	bHeader.y = 0;
	bConnect.y = height - 30;

	bBack.x = 0;
	bHeader.x = bBack.width;
	bHeader.width = width - bHeader.x;
	bConnect.x = width - 100;
	bConnect.width = 90;
	
	if (ipTextBox)
	{
		ipTextBox->x = 10;
		ipTextBox->y = height - 55;
		ipTextBox->w = width - 120;
		ipTextBox->h = 20;
	}
}

void JoinGameScreen::buttonClicked(Button* button)
{
	if (button->id == bJoin.id)
	{
		if (isIndexValid(gamesList->selectedItem))
		{
			PingedCompatibleServer selectedServer = gamesList->copiedServerList[gamesList->selectedItem];
			minecraft->joinMultiplayer(selectedServer);
			{
				bJoin.active = false;
				bBack.active = false;
				minecraft->setScreen(new ProgressScreen());
			}
		}
	}
	if (button->id == bConnect.id)
	{
		if (ipTextBox && ipTextBox->text.length() > 0)
		{
			std::string ipText = ipTextBox->text;
			std::string ip = ipText;
			int port = 19132;
			
			size_t colonPos = ipText.find(':');
			if (colonPos != std::string::npos)
			{
				ip = ipText.substr(0, colonPos);
				port = atoi(ipText.substr(colonPos + 1).c_str());
				if (port <= 0) port = 19132;
			}
			
			PingedCompatibleServer manualServer;
			manualServer.address.FromStringExplicitPort(ip.c_str(), port);
			manualServer.name = ipText.c_str();
			manualServer.isSpecial = false;
			
			minecraft->joinMultiplayer(manualServer);
			bJoin.active = false;
			bBack.active = false;
			bConnect.active = false;
			minecraft->setScreen(new ProgressScreen());
		}
	}
	if (button->id == bBack.id)
	{
		minecraft->cancelLocateMultiplayer();
		minecraft->screenChooser.setScreen(SCREEN_STARTMENU);
	}
}

bool JoinGameScreen::handleBackEvent(bool isDown)
{
	if (!isDown)
	{
		minecraft->cancelLocateMultiplayer();
		minecraft->screenChooser.setScreen(SCREEN_STARTMENU);
	}
	return true;
}


bool JoinGameScreen::isIndexValid( int index )
{
	return gamesList && index >= 0 && index < gamesList->getNumberOfItems();
}

void JoinGameScreen::tick()
{
	if (isIndexValid(gamesList->selectedItem)) {
		buttonClicked(&bJoin);
		return;
	}

	const ServerList& orgServerList = minecraft->raknetInstance->getServerList();
	ServerList serverList;
	for (unsigned int i = 0; i < orgServerList.size(); ++i)
		if (orgServerList[i].name.GetLength() > 0)
			serverList.push_back(orgServerList[i]);

	if (serverList.size() != gamesList->copiedServerList.size())
	{
		PingedCompatibleServer selectedServer;
		bool hasSelection = false;
		if (isIndexValid(gamesList->selectedItem))
		{
			selectedServer = gamesList->copiedServerList[gamesList->selectedItem];
			hasSelection = true;
		}

		gamesList->copiedServerList = serverList;
		gamesList->selectItem(-1, false);

		if (hasSelection)
		{
			for (unsigned int i = 0; i < gamesList->copiedServerList.size(); i++)
			{
				if (gamesList->copiedServerList[i].address == selectedServer.address)
				{
					gamesList->selectItem(i, false);
					break;
				}
			}
		}
	} else {
		for (int i = (int)gamesList->copiedServerList.size()-1; i >= 0 ; --i) {
			for (int j = 0; j < (int) serverList.size(); ++j)
				if (serverList[j].address == gamesList->copiedServerList[i].address)
					gamesList->copiedServerList[i].name = serverList[j].name;
		}
	}

	bJoin.active = isIndexValid(gamesList->selectedItem);
}

void JoinGameScreen::keyPressed(int key)
{
	if (ipTextBox && ipTextBox->focused)
	{
		if (key == 8)
		{
			if (ipTextBox->text.length() > 0)
				ipTextBox->text.erase(ipTextBox->text.length() - 1);
			return;
		}
		else if (key == 13)
		{
			buttonClicked(&bConnect);
			return;
		}
	}
	Screen::keyPressed(key);
}

void JoinGameScreen::keyboardNewChar(char inputChar)
{
	if (ipTextBox && ipTextBox->focused)
	{
		if ((inputChar >= '0' && inputChar <= '9') || 
		    inputChar == '.' || inputChar == ':' || 
		    (inputChar >= 'a' && inputChar <= 'z') || 
		    (inputChar >= 'A' && inputChar <= 'Z'))
		{
			ipTextBox->text += inputChar;
		}
		return;
	}
	Screen::keyboardNewChar(inputChar);
}

void JoinGameScreen::mouseClicked(int x, int y, int buttonNum)
{
	if (ipTextBox)
	{
		bool inBox = x >= ipTextBox->x && x < ipTextBox->x + ipTextBox->w &&
		             y >= ipTextBox->y && y < ipTextBox->y + ipTextBox->h;
		if (inBox)
		{
			ipTextBox->setFocus(minecraft);
		}
		else
		{
			ipTextBox->loseFocus(minecraft);
		}
	}
	Screen::mouseClicked(x, y, buttonNum);
}

void JoinGameScreen::render( int xm, int ym, float a )
{
	bool hasNetwork = minecraft->platform()->isNetworkEnabled(true);
#ifdef WIN32
	hasNetwork = hasNetwork && !GetAsyncKeyState(VK_TAB);
#endif

	renderBackground();
	if (hasNetwork) gamesList->render(xm, ym, a);
	else gamesList->renderDirtBackground();
	Screen::render(xm, ym, a);

	const int baseX = bHeader.x + bHeader.width / 2;

	if (hasNetwork) {
		std::string s = "Scanning for WiFi Games...";
		drawCenteredString(minecraft->font, s, baseX, 8, 0xffffffff);

		const int textWidth = minecraft->font->width(s);
		const int spinnerX = baseX + textWidth / 2 + 6;

		static const char* spinnerTexts[] = {"-", "\\", "|", "/"};
		int n = ((int)(5.5f * getTimeS()) % 4);
		drawCenteredString(minecraft->font, spinnerTexts[n], spinnerX, 8, 0xffffffff);
		
		drawString(minecraft->font, "IP:port:", ipTextBox->x, ipTextBox->y - 12, 0xffaaaaaa);
		if (ipTextBox)
			ipTextBox->render(minecraft, xm, ym);
	} else {
		drawCenteredString(minecraft->font, "WiFi is disabled", baseX, 8, 0xffffffff);
	}
}

bool JoinGameScreen::isInGameScreen() { return false; }

};
