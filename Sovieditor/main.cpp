#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <iterator>
#include <windows.h>
#include <shobjidl.h>
#include <objbase.h>

//#define _WIN32_WINNT 0x0600 

#ifndef UNICODE
#define UNICODE
#endif 

#define NTDDI_VERSION 0x0A000006 //NTDDI_WIN10_RS5
#define _WIN32_WINNT 0x0A00

/*
TODO:
	Umlaute
	...
*/

//Set ld flags in mingw in order to compile this. See Stackoverflow
//CPP-Reference:
std::string print_wide(const wchar_t* wstr) {
    std::mbstate_t state = std::mbstate_t();
    std::size_t len = 1 + std::wcsrtombs(nullptr, &wstr, 0, &state);
    std::vector<char> mbstr(len);
    std::wcsrtombs(&mbstr[0], &wstr, mbstr.size(), &state);
    //std::cout << &mbstr[0] << std::endl;
    return std::string(mbstr.begin(), mbstr.end());
}

//Win32 API - Stackoverflow:
std::string fileDialogWinApi() {
	std::string s1;

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        IFileOpenDialog* pFileOpen;

        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
        // if object created ( that's com object )
        if (SUCCEEDED(hr)) {
            hr = pFileOpen->Show(NULL);

            if (SUCCEEDED(hr)) {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr)){
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    if (SUCCEEDED(hr)) {
                        //MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
                        s1 = print_wide(pszFilePath);
                        CoTaskMemFree(pszFilePath);

                        std::string stringtoconvert;

                        std::wstring temp = std::wstring(stringtoconvert.begin(), stringtoconvert.end());
                        LPCWSTR lpcwstr = temp.c_str();
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }    
    return s1;
}

class EditorMouse;
class MarkedText;
class Syntax;
class Slider;
class Cursor;
class Editor;
class Text;

struct string_to_display {
	std::string str;
	bool highlight;
	int stdLength;
};

struct TextWidths {
	int charWidth;
	int tabWidth;
	int textWidth;
};

struct cursorPosition {
	int lineNr;
	int posX;
	int posY;

	cursorPosition() {

	}

	cursorPosition(int lineNr, int posX, int posY) {
		this->lineNr = lineNr;
		this->posX = posX;
		this->posY = posY;
	}
};

class EditorMouse {
	bool inUse;
	sf::Mouse m;
	sf::Vector2i mcords;

public:
	EditorMouse() {

	}

	void loadMouseCoordinates(sf::RenderWindow &window) {
		mcords = m.getPosition(window);
	}

	int getPosX() {
		return mcords.x;
	}

	int getPosY() {
		return mcords.y;
	}

	sf::Vector2i& getMouseCords() {
		return mcords;
	}

	void setInUse(bool t) {
		inUse = t;
	}

	bool getInUse() {
		return inUse;
	}
};

class MarkedText {
	cursorPosition cp1;
	cursorPosition cp2;
	bool isActive;
	sf::RectangleShape rs;

public:
	MarkedText() {

	}

	MarkedText(cursorPosition cp1, cursorPosition cp2) {
		this->cp1 = cp1;
		this->cp2 = cp2;
		isActive = true;
	}

	std::string getString(Editor &editor, Cursor &cursor, Text &text);
	cursorPosition setString(Editor &editor, Cursor &cursor, Text &text, std::string stringToInsert);
	cursorPosition deleteString(Editor &editor, Cursor &cursor, Text &text);
	void drawMarkedText(Editor &editor, Text &text, sf::RenderWindow &window);
	void drawRectangle(sf::RenderWindow &window, int posX, int posY, int width, int height);
	void checkOrientation();

	void setCursorPositionOne(cursorPosition cp1) {
		this->cp1 = cp1;
	}

	void setCursorPositionTwo(cursorPosition cp2) {
		this->cp2 = cp2;
	}

	void setActive() {
		isActive = true;
	}

	void unsetActive() {
		isActive = false;
	}

	bool getActive() {
		return isActive;
	}
};

class Slider {
private:
	bool inUse;
	bool allowUpdate;
	int posX;
	int posY;
	int changeY;
	int defaultSize;
	int sizeY;
	int mPosBef;
	int changeYClick;
	int sliderWidth;
	sf::RectangleShape sliderRect;
	sf::CircleShape circle;

public:
	Slider() {

	}

	Slider(int posX, int posY) {
		this->posX = posX;
		this->posY = posY;
		this->defaultSize = 20;
		this->sizeY = defaultSize;
		this->inUse = false;
		this->mPosBef = 0;
		this->changeY = 0;
		this->allowUpdate = true;
		this->sliderWidth = 7;
	}

	void loadEvents(Editor &editor, Cursor &cursor, Text &text, sf::Event &event, sf::RenderWindow &window, sf::Vector2i &mcords);
	void loadDraw(Editor &editor, sf::RenderWindow &window);
	void updateSlider(Editor &editor, Cursor &cursor, Text &text);
	void setChangeYToLine(Cursor &cursor, Text &text, sf::RenderWindow &window);
	void sliderInUse(Editor &editor, Cursor &cursor, Text &text, sf::Event &event, sf::RenderWindow &window, sf::Vector2i &mcords);
	void sliderClicked(Editor &editor, Cursor &cursor, Text &text, sf::Event &event, sf::RenderWindow &window, sf::Vector2i &mcords);
	void setChangeYOutOfBounds(Editor &editor, Cursor &cursor, Text &text, sf::Event &event, sf::RenderWindow &window, sf::Vector2i &mcords);
	void setChangeYInBounds(Editor &editor, Cursor &cursor, Text &text, sf::Event &event, sf::RenderWindow &window, sf::Vector2i &mcords);

	void setInUse(bool t) {
		inUse = t;
	}

	bool getInUse() {
		return inUse;
	}

	void setSliderWidth(int x) {
		this->sliderWidth = x;
	}

	int getSliderWidth() {
		return this->sliderWidth + 10;
	}

	void setCPosY(int posY) {
		this->changeY = posY;
	}

	int getCPosY() {
		return changeY;
	}

	void setPosY(int posY) {
		this->posY = posY;
	}

	void setPosX(int posX) {
		this->posX = posX;
	} 

	int getPosX() {
		return posX;
	}

	void escapeEvent() {
		inUse = false;
		mPosBef = 0;
	}	
};

class Text {
private:
	std::vector<std::string> lines;
	sf::Text textDraw;
	float bottomLine;
	bool allowInput;
	TextWidths textWidths;
	int fontSize;
	int fontSizeSpacing;
	sf::Font font;
	std::vector<std::string> keywords;
	std::vector<std::string> keywordsSpecial;

public:
	Text() {
		keywords = {"/*", "*/", "//", "void", "int", "double", "float", "bool", "return", "break", "switch", "case", "class", "struct", "if", "else", "while", "for", "#include", "#if",
		"#define", "#ifndef", "#endif", "public:", "private:", "string", "std", "sf"};
		keywordsSpecial = {"{", "}", "=", "!", "(", ")", ";", ":", "+", "-", "*", "/", "\"", ">", ",", "<"};

		setFontSize(14, 20);
	}

	void loadFile(std::string file);
	int loadFont(std::string sfont);
	void loadTextWidthsBounds(int lineNr);
	void loadVars();
	void loadText();
	void insertText(int cursorLineNr, int cursorCharNr, char insert, bool deleteB);
	void loadEvents(Editor &editor, Cursor &cursor, sf::Event &event);
	void loadDraw(sf::RenderWindow &window, Editor &editor);
	void countChars(void* outVal, Editor &editor, std::string s1, Cursor &cursor, void func(int* outVal2, int* igive, int* sgive));
	int getSize(int lineNr, int charNr);
	//void countCharsTab(int* i, int* size, int* i2);

	int getFontSize() {
		return fontSize;
	}

	int getFontSizeSpacing() {
		return fontSizeSpacing;
	}

	void setFontSize(int real, int spacing) {
		fontSize = real;
		fontSizeSpacing = spacing;
	}

	float getBottomLine() {
		return bottomLine;
	}

	void setBottomLine(float x) {
		bottomLine = x;
	}

	std::string getLine(int x) {
		return lines.at(x);
	} 

	int getCharWidth() {
		return textWidths.charWidth;
	}

	int getTabWidth() {
		return textWidths.tabWidth;
	}

	int getTextWidth() {
		return textWidths.textWidth;
	}

	int getLineSize() {
		return lines.size();
	}

	void insertLines(int cursorLineNr, std::string text) {
		lines.insert(lines.begin() + cursorLineNr, text);
	}

	void deleteLine(int x) {
		lines.erase(lines.begin() + x);
	}

	void addText(int x, std::string textAdd) {
		lines.at(x) += textAdd;
	}

	void setText(int x, std::string textSet) {
		lines.at(x) = textSet;
	}

	void setAllowInput() {
		allowInput = true;
	}

	void denieInput() {
		allowInput = false;
	}

	bool allowInputSet() {
		return allowInput;
	}

	static void countCharsSize(int* outVal, int* i, int* size) {
		(*(int*)(outVal)) = (*i)+1;
	}
};

class Cursor {
private:	
	int posY;
	int posX;
	int cursorLineNr;
	int cursorBlink;
	int cursorShow;
	int posXAtClick;
	int lineNrAtClick;
	int posYAtClick;
	sf::RectangleShape cursorDraw;
	sf::RectangleShape cursorBackground;
	MarkedText MT;

public:

	Cursor() {
		this->posXAtClick = -1;
		this->lineNrAtClick = -1;
		this->posYAtClick = -1;
		MT.unsetActive();
	}

	void loadEvents(Editor &editor, sf::Event &event, Text &text);
	void loadDraw(Editor &editor, Text &text, sf::RenderWindow &window);
	void loadCursor(Editor &editor, Text &text);
	void loadVars(Editor &editor);
	void setCursorRects(Editor &editor, Text &text);
	bool cursorLeft(Editor &editor, Text &text);
	bool cursorRight(Editor &editor, Text &text);
	bool cursorUp(Editor &editor, Text &text);
	bool cursorDown(Editor &editor, Text &text);
	bool cursorSimpleBackspace(Editor &editor, Text &text);
	bool cursorMTBackspace(Editor &editor, Text &text);
	bool cursorBackspace(Editor &editor, Text &text);
	bool cursorEnter(Editor &editor, Text &text);
	bool cursorU(Editor &editor, Text &text);
	bool cursorD(Editor &editor, Text &text);
	void cursorMouseEvent(Editor &editor, Text &text, sf::Event &event, sf::Vector2i &mcords);
	void cursorMouseMarking(Editor &editor, Text &text, sf::Event &event, sf::Vector2i &mcords);
	void cursorMouseEventRecognition(Editor &editor, Text &text, sf::Event &event, sf::Vector2i &mcords);
	bool checkMouseInWindowBounds(sf::Vector2i &mcords, int screenPosX, int screenPosY, int screenWidth, int screenHeight);
	void setMousePosToCursorPos(Editor &editor, Text &text, sf::Event &event, sf::Vector2i &mcords);
	void setMousePosYToLine(Text &text, sf::Vector2i &mcords);
	int getMousePosYToLineNr(int mousePosY, int fontSize, int topSpace);
	int setMousePosXToCursorX(Editor &editor, Text &text, sf::Vector2i &mcords, int lineNr);
	int setMousePosXToCursorPosXValues(Editor &editor, Text &text, int charSize, int charSizeBef, sf::Vector2i &mcords);
	int getTabsForNewLine(Text &text, int tillPos);
	void setPosYBottomLine(Editor &editor, Text &text);
	void createNewLineSetNewLineParameters(Editor &editor, Text &text, int &lineNr, int tillPos, int tabNr);
	void setNormalBackspace(Editor &editor, Text &text, int cursorCharNr);
	void setDeleteLineBackspace(Editor &editor, Text &text);
	cursorPosition cursorInsertText(Editor &editor, Text &text, std::string textToInsert, int lineNr, int linePosX, int linePosY);
	int getPosXToCharNr(Editor &editor, Text &text, int linePosX, int lineNr);
	void cursorInsertTextMakeNewLine(Editor &editor, Text &text, std::string &textToInsert, int &lineNr, int &linePosX);
	void ctrlV(Editor &editor, Text &text);
	void simpleCtrlV(Editor &editor, Text &text);

	bool defaultMTIsActive() {
		return MT.getActive();
	}

	std::string getStringFromMarkedText(Editor &editor, Cursor &cursor, Text &text) {
		return MT.getString(editor, cursor, text);
	}

	cursorPosition deleteDefaultMTString(Editor &editor, Cursor &cursor, Text &text) {
		return MT.deleteString(editor, cursor, text);
	}

	cursorPosition defaultMTSetString(Editor &editor, Cursor &cursor, Text &text, std::string stringToInsert) {
		return MT.setString(editor, cursor, text, stringToInsert);
	}

	void drawDefaultMarkedText(Editor &editor, Text &text, sf::RenderWindow &window) {
		MT.drawMarkedText(editor, text, window);
	}

	//When posXAtClick is set at -1 all the conditions know that posXAtClick does not have a meaningful value.
	void enableSetPosXAtClick() {
		this->posXAtClick = -1;
		this->lineNrAtClick = -1;
		this->posYAtClick = -1;
	}

	void setPosXAtClick() {
		posXAtClick = posX;
		lineNrAtClick = cursorLineNr;
		posYAtClick = posY;
	}

	int getPosY() {
		return posY;
	}

	void setPosY(int x) {
		posY = x;
	}

	int getPosX() {
		return posX;
	}

	void setPosX(int x) {
		posX = x;
	}

	int getCursorLineNr() {
		return cursorLineNr;
	}

	void setCursorLineNr(int x) {
		cursorLineNr = x;
	}

	void showCursor() {
		cursorShow = 20;
	}

	void blankCursor() {
		cursorShow = 0;
	}
};

class Editor {
private:
	sf::RenderWindow window;
	sf::RectangleShape background;
	sf::RectangleShape leftNumBlock;
	Text text;
	Cursor cursor;
	Slider slider;
	int GreyBlockSize;
	bool setCTRL;
	bool setAlt;
	int leftIndent;
	std::string path;
	std::string wTitle;
	bool mouseInRange;
	EditorMouse mouse;

public:
	Editor() {
		window.create(sf::VideoMode(1200, 900), "Sovieditor");
		window.setFramerateLimit(60);

		GreyBlockSize = 30;
		background.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
		background.setPosition(0, 0);
		background.setFillColor(sf::Color::White);
		leftNumBlock.setSize(sf::Vector2f(30, window.getSize().y));
		leftNumBlock.setFillColor(sf::Color(210, 210, 210));
		leftNumBlock.setPosition(0, 0);
		setCTRL = false;
		wTitle = "Sovieditor";
	}

	void loadEvents(sf::Event &event);
	void loadDraw();
	void loadEditor();
	void loadAllEvents(sf::Event &event);
	void loadSwitchKeyPressedEvents(sf::Event &event);
	void loadSwitchKeyReleasedEvents(sf::Event &event);
	void loadAllDraws();
	void loadLoop();
	void loadVars();
	void openFile(Text &text, Cursor &cursor);
	void writeFile(Text &text);
	void dynamicGBS(Cursor &cursor, Text &text);
	void checkMouse();
	void mScrolled(sf::Event &event);
	void loadTotalEvents();

	void setMouseInUse() {
		mouse.setInUse(true);
	}

	void unsetMouseInUse() {
		mouse.setInUse(false);
	}

	void loadMouse(EditorMouse &m) {
		this->mouse = m;
	}

	bool getSliderInUse() {
		return slider.getInUse();
	}

	int getLeftIndent() {
		return slider.getSliderWidth();
	}

	bool getMouseInRange() {
		return mouseInRange;
	}

	void setMouseInRange(bool s) {
		mouseInRange = s;
	}

	void setFilePath(std::string p) {
		path = p;
	}

	void setTextObj(Text text) {
		this->text = text;
	}

	void setCursorObj(Cursor cursor) {
		this->cursor = cursor;
		
		cursor.loadCursor(*this, text);
	}

	void setSliderObj(Slider slider) {
		this->slider = slider;
	}

	int getGreyBlockSize() {
		return GreyBlockSize;
	}

	void setGreyBlockSize(int x) {
		GreyBlockSize = x;
	}

	sf::Vector2u wGetSize() {
		return window.getSize();
	}

	bool getCTRL() {
		return setCTRL;
	}

	void onCTRL() {
		setCTRL = true;
	}

	void offCTRL() {
		setCTRL = false;
	}

	void onAlt() {
		setAlt = true;
	}

	void offAlt() {
		setAlt = false;
	}

	bool getAlt() {
		return setAlt;
	}

	void setTitleNotSaved() {
		window.setTitle(wTitle + "*");
	}

	void setTitleSaved() {
		window.setTitle(wTitle);
	}
};

void MarkedText::drawRectangle(sf::RenderWindow &window, int posX, int posY, int width, int height) {
	rs.setFillColor(sf::Color(150, 150, 150));
	rs.setPosition(sf::Vector2f(posX, posY));
	rs.setSize(sf::Vector2f(abs(width), height));

	window.draw(rs);
}

void MarkedText::checkOrientation() {
	if(cp1.lineNr > cp2.lineNr || (cp1.posX > cp2.posX && cp1.lineNr == cp2.lineNr)) {
		cursorPosition ret = cp1;
		cp1 = cp2;
		cp2 = ret;	
	}
}

void MarkedText::drawMarkedText(Editor &editor, Text &text, sf::RenderWindow &window) {
	if(!isActive) {
		return;
	}

	if(cp2.lineNr < text.getBottomLine() - 1) {
		return;
	}
	
	if(cp1.lineNr == cp2.lineNr) {
		int MTPosY = ((cp1.lineNr - text.getBottomLine() + 1)) * text.getFontSizeSpacing() + 3;
		drawRectangle(window, cp1.posX, MTPosY, abs(cp1.posX - cp2.posX), text.getFontSizeSpacing());
		return;
	}

	int iLineNr = abs(cp2.lineNr - cp1.lineNr);

	int i = 0;
	int MTPosY = ((cp1.lineNr - text.getBottomLine() + 1)) * text.getFontSizeSpacing() + 3;
	drawRectangle(window, cp1.posX, MTPosY, abs((cp1.posX-editor.getGreyBlockSize()) - text.getSize(cp1.lineNr, -1)), text.getFontSizeSpacing());
	while(i<iLineNr-1) {
		i++;
		MTPosY += text.getFontSizeSpacing();
		drawRectangle(window, editor.getGreyBlockSize(), MTPosY, text.getSize(cp1.lineNr + i, -1), text.getFontSizeSpacing());
	}
	MTPosY += text.getFontSizeSpacing();
	drawRectangle(window, editor.getGreyBlockSize(), MTPosY, cp2.posX - editor.getGreyBlockSize(), text.getFontSizeSpacing());
}

std::string MarkedText::getString(Editor &editor, Cursor &cursor, Text &text) {
	if(!isActive) {
		return "";
	}

	std::string returnstr;

	if(cp1.lineNr == cp2.lineNr) {
		int cp1x = cursor.getPosXToCharNr(editor, text, cp1.posX, cp1.lineNr);
		int cp2x = cursor.getPosXToCharNr(editor, text, cp2.posX, cp1.lineNr);

		if(cp2x == -1) {
			cp2x = text.getLine(cp1.lineNr).size();
		}

		returnstr = text.getLine(cp1.lineNr).substr(cp1x, cp2x - cp1x);
		return returnstr;
	}

	int iLineNr = abs(cp2.lineNr - cp1.lineNr);
	int cp1x = cursor.getPosXToCharNr(editor, text, cp1.posX, cp1.lineNr);
	int cp2x = cursor.getPosXToCharNr(editor, text, cp2.posX, cp2.lineNr);
	cp1x = (cp1x == -1) ? (text.getLine(cp1.lineNr).size()) : (cp1x);
	cp2x = (cp2x == -1) ? (text.getLine(cp2.lineNr).size()) : (cp2x);
	returnstr += text.getLine(cp1.lineNr).substr(cp1x, text.getLine(cp1.lineNr).size() - cp1x);
	int i = 0;
	while(i<iLineNr-1) {
		i++;
		returnstr += "\n" + text.getLine(cp1.lineNr + i);
	}
	returnstr += "\n";
	returnstr += text.getLine(cp2.lineNr).substr(0, cp2x + 1);

	return returnstr;
}

cursorPosition MarkedText::deleteString(Editor &editor, Cursor &cursor, Text &text) {
	cursorPosition retcp;

	if(!isActive) {
		return retcp;
	}

	if(cp1.lineNr == cp2.lineNr) {
		int cp1x = cursor.getPosXToCharNr(editor, text, cp1.posX, cp1.lineNr);
		int cp2x = cursor.getPosXToCharNr(editor, text, cp2.posX, cp1.lineNr);
		std::string text1 = text.getLine(cp1.lineNr).substr(0, cp1x);
		std::string text2 = (cp2x != -2) ? text.getLine(cp1.lineNr).substr(cp2x, text.getLine(cp1.lineNr).size()-cp2x) : "";
		text.setText(cp1.lineNr, text1 + text2);

		retcp.lineNr = cp1.lineNr;
		retcp.posX = cp1.posX;
		retcp.posY = cp1.posY;
		return retcp;
	}

	int cp1x = cursor.getPosXToCharNr(editor, text, cp1.posX, cp1.lineNr);
	int cp2x = cursor.getPosXToCharNr(editor, text, cp2.posX, cp2.lineNr);
	std::string text1 = text.getLine(cp1.lineNr).substr(0, cp1x);
	std::string text2 = (cp2x != -2) ? text.getLine(cp2.lineNr).substr(cp2x, text.getLine(cp2.lineNr).size()-cp2x) : "";
	text.setText(cp1.lineNr, text1 + text2);

	retcp.lineNr = cp1.lineNr;
	retcp.posX = cp1.posX;
	retcp.posY = cp1.posY;

	for(int i = cp1.lineNr+1;i<=cp2.lineNr;++i) {
		text.deleteLine(cp1.lineNr+1);
	}

	return retcp;
}

void Slider::setChangeYToLine(Cursor &cursor, Text &text, sf::RenderWindow &window) {
	int maxLines = (int)((window.getSize().y - 3) / text.getFontSizeSpacing());
	int zeile = (int)((float)changeY * (float)((float)(text.getLineSize() - maxLines) / (float)(window.getSize().y - posY - sizeY)));
	cursor.setCursorLineNr(cursor.getCursorLineNr() + zeile + 1 - text.getBottomLine());
	text.setBottomLine(zeile + 1);
	allowUpdate = false;
}

void Slider::setChangeYOutOfBounds(Editor &editor, Cursor &cursor, Text &text, sf::Event &event, sf::RenderWindow &window, sf::Vector2i &mcords) {
	if(mcords.x >= editor.getGreyBlockSize() && mcords.x <= window.getSize().x)	{
		if(mcords.y < posY) {
			changeY = 0;
			setChangeYToLine(cursor, text, window);
			return;
		}else if(mcords.y > window.getSize().y) {
			changeY = window.getSize().y - sizeY; 
			setChangeYToLine(cursor, text, window);
			return;
		}
	}
}

void Slider::setChangeYInBounds(Editor &editor, Cursor &cursor, Text &text, sf::Event &event, sf::RenderWindow &window, sf::Vector2i &mcords) {
	if(mcords.x >= editor.getGreyBlockSize() && mcords.y >= 0 && mcords.x <= window.getSize().x && mcords.y <= window.getSize().y) {
		int difference = (float)mPosBef - mcords.y;
		int maxLines = (int)((window.getSize().y - 3) / text.getFontSizeSpacing());

		if((changeYClick - difference >= posY) && (changeYClick - difference + posY + sizeY <= window.getSize().y)) {
			changeY = changeYClick - difference;
			setChangeYToLine(cursor, text, window);
		}
	}
}

void Slider::sliderInUse(Editor &editor, Cursor &cursor, Text &text, sf::Event &event, sf::RenderWindow &window, sf::Vector2i &mcords) {
	setChangeYOutOfBounds(editor, cursor, text, event, window, mcords);

	setChangeYInBounds(editor, cursor, text, event, window, mcords);
}

void Slider::sliderClicked(Editor &editor, Cursor &cursor, Text &text, sf::Event &event, sf::RenderWindow &window, sf::Vector2i &mcords) {
	sf::Cursor c;
	if(c.loadFromSystem(sf::Cursor::Arrow))
		window.setMouseCursor(c);	
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !inUse) {
		inUse = true;
		editor.setMouseInUse();
		mPosBef = mcords.y;
		changeYClick = changeY;
	}
}

void Slider::loadEvents(Editor &editor, Cursor &cursor, Text &text, sf::Event &event, sf::RenderWindow &window, sf::Vector2i &mcords) {
	if(!inUse && mcords.x >= window.getSize().x-sliderWidth && mcords.y >= posY+changeY && mcords.x <= window.getSize().x-2 && mcords.y <= posY+changeY+sizeY) {
		sliderClicked(editor, cursor, text, event, window, mcords);
	}
	
	if(inUse) {
		sliderInUse(editor, cursor, text, event, window, mcords);	
	}
}

void Slider::updateSlider(Editor &editor, Cursor &cursor, Text &text) {
	if(!allowUpdate) {
		allowUpdate = true;
		return;
	}

	int maxLines = (int)((editor.wGetSize().y - 3) / text.getFontSizeSpacing());

	if(text.getLineSize() - maxLines < (editor.wGetSize().y - posY - defaultSize)) {
		sizeY = (editor.wGetSize().y - posY - defaultSize) - text.getLineSize() - maxLines;
	}else {
		sizeY = defaultSize;
	}

	changeY = (int)((float)text.getBottomLine() * (float)((float)(editor.wGetSize().y - posY - sizeY) / (float)(text.getLineSize() - maxLines)));
}

void Slider::loadDraw(Editor &editor, sf::RenderWindow &window) {
	sf::Color c;

	c = sf::Color(200, 200, 200);

	if(inUse) {
		c = sf::Color(100, 100, 100);
	}

	circle.setRadius(2.5);
	circle.setPosition(sf::Vector2f(window.getSize().x - sliderWidth, posY+changeY));
	circle.setFillColor(c);

	window.draw(circle);

	circle.setRadius(2.5);
	circle.setPosition(sf::Vector2f(window.getSize().x - sliderWidth, posY+changeY+sizeY-5));
	circle.setFillColor(c);

	window.draw(circle);

	sliderRect.setFillColor(c);
	sliderRect.setPosition(sf::Vector2f(window.getSize().x - sliderWidth, posY+changeY+2.5));
	sliderRect.setSize(sf::Vector2f(5, sizeY-5));

	window.draw(sliderRect);
}

int Cursor::setMousePosXToCursorPosXValues(Editor &editor, Text &text, int charSize, int charSizeBef, sf::Vector2i &mcords) {
	int r;
	
	if(mcords.x <= editor.getGreyBlockSize()) {
		return editor.getGreyBlockSize();
	}

	if(charSize - mcords.x - editor.getGreyBlockSize() > mcords.x - editor.getGreyBlockSize() - charSizeBef) {
		r = charSizeBef + editor.getGreyBlockSize();
	}else {
		r = charSize + editor.getGreyBlockSize();
	} 

	return r;
}

int Cursor::setMousePosXToCursorX(Editor &editor, Text &text, sf::Vector2i &mcords, int lineNr) {
	int charSize = 0;
	int charSizeBef = 0;
	for(int i = 0;i<text.getLine(lineNr).size();++i) {
		charSizeBef = charSize;

		if(text.getLine(lineNr).at(i) == '	') {
			charSize += text.getTabWidth();
		}else {
			charSize += text.getCharWidth();
		}

		if(charSize >= mcords.x - editor.getGreyBlockSize()) {
			return setMousePosXToCursorPosXValues(editor, text, charSize, charSizeBef, mcords);
		}
	}

	return charSize + editor.getGreyBlockSize();
}

int Cursor::getMousePosYToLineNr(int mousePosY, int fontSize, int topSpace) {
	return (int)((mousePosY - topSpace) / fontSize);
}

void Cursor::setMousePosYToLine(Text &text, sf::Vector2i &mcords) {
	cursorLineNr = getMousePosYToLineNr(mcords.y, text.getFontSizeSpacing(), 3);
	posY = cursorLineNr;
	cursorLineNr += text.getBottomLine() - 1;
}

void Cursor::setMousePosToCursorPos(Editor &editor, Text &text, sf::Event &event, sf::Vector2i &mcords) {
	setMousePosYToLine(text, mcords);

	posX = setMousePosXToCursorX(editor, text, mcords, cursorLineNr);

	if(posXAtClick == -1) {
		MT.unsetActive();
		setPosXAtClick();
	}
}

bool Cursor::checkMouseInWindowBounds(sf::Vector2i &mcords, int screenPosX, int screenPosY, int screenWidth, int screenHeight) {
	return mcords.x >= screenPosX && mcords.x <= screenWidth && mcords.y >= screenPosY && mcords.y <= screenHeight;
}

void Cursor::cursorMouseMarking(Editor &editor, Text &text, sf::Event &event, sf::Vector2i &mcords) {
	if(lineNrAtClick - cursorLineNr != 0 || getPosXToCharNr(editor, text, posX, cursorLineNr) - getPosXToCharNr(editor, text, posXAtClick, lineNrAtClick) != 0) {
		std::cout << "You are marking Text" << std::endl;
		MarkedText newMT(cursorPosition(lineNrAtClick, posXAtClick, posYAtClick), cursorPosition(cursorLineNr, posX, posY));
		MT = newMT;
		MT.checkOrientation();
	}
}

void Cursor::cursorMouseEventRecognition(Editor &editor, Text &text, sf::Event &event, sf::Vector2i &mcords) {
	setMousePosToCursorPos(editor, text, event, mcords);
	showCursor();
	cursorMouseMarking(editor, text, event, mcords);
}

void Cursor::cursorMouseEvent(Editor &editor, Text &text, sf::Event &event, sf::Vector2i &mcords) {
	if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && checkMouseInWindowBounds(mcords, 0, 0, editor.wGetSize().x - editor.getLeftIndent(), editor.wGetSize().y)) {
		cursorMouseEventRecognition(editor, text, event, mcords);
		std::cout << "recognizing" << std::endl;
	}
}

void Cursor::setCursorRects(Editor &editor, Text &text) {
	cursorBackground.setSize(sf::Vector2f(editor.wGetSize().x - editor.getGreyBlockSize(), text.getFontSizeSpacing()));
	cursorBackground.setPosition(editor.getGreyBlockSize(), (posY * text.getFontSizeSpacing()) + 3);
	cursorBackground.setFillColor(sf::Color(240, 240, 240));

	cursorDraw.setSize(sf::Vector2f(1, text.getFontSizeSpacing()));
	cursorDraw.setPosition(posX, (posY * text.getFontSizeSpacing()) + 3);
	cursorDraw.setFillColor(sf::Color::Black);
}

void Cursor::loadCursor(Editor &editor, Text &text) {
	loadVars(editor);
	
	setCursorRects(editor, text);
}

void Cursor::loadVars(Editor &editor) {
	posY = 0;
	posX = 0;
	cursorLineNr = 0;
	cursorBlink = 0;
	cursorShow = 0;
}

bool Cursor::cursorLeft(Editor &editor, Text &text) {
	if(editor.getCTRL()) {
		return false;
	}

	if((posX-editor.getGreyBlockSize()) != 0) {
		int nextChar = 0;
		text.countChars(&nextChar, editor, text.getLine(cursorLineNr), *this, text.countCharsSize);
		if(text.getLine(cursorLineNr).at(nextChar - 1) == '	') {
			posX-=text.getTabWidth();
		}else {
			posX-=text.getCharWidth();
		}
	}

	return true;
}

bool Cursor::cursorRight(Editor &editor, Text &text) {
	if(editor.getCTRL()) {
		return false;
	}

	int charPosSize = 0;
	text.countChars(&charPosSize, editor, text.getLine(cursorLineNr), *this, text.countCharsSize);
	if(charPosSize < text.getLine(cursorLineNr).length()) {
		int nextChar = 0;
		text.countChars(&nextChar, editor, text.getLine(cursorLineNr), *this, text.countCharsSize);
		if(text.getLine(cursorLineNr).at(nextChar) == '	') {
			posX+=text.getTabWidth();
		}else {
			posX += text.getCharWidth();
		}
	}
	return true;
}

bool Cursor::cursorU(Editor &editor, Text &text) {
	int maxLines = (int)((editor.wGetSize().y - 3) / text.getFontSizeSpacing());

	if(editor.getCTRL() && text.getBottomLine() - maxLines > 0) {
		text.setBottomLine(text.getBottomLine() - maxLines);
		cursorLineNr -= maxLines;
		return true;
	}else {
		cursorLineNr -= text.getBottomLine() - 1;
		text.setBottomLine(1);
	}
	return false;
}

bool Cursor::cursorUp(Editor &editor, Text &text) { 
	if(editor.getCTRL()) {
		return false;
	}

	cursorLineNr--;
	if((cursorLineNr) < 0) {
		cursorLineNr++;
	}else if((posY-1) < 0) {
		text.setBottomLine(text.getBottomLine() - 1);
	}else {
		posY--;
	}
	return true;
}

bool Cursor::cursorD(Editor &editor, Text &text) {
	int maxLines = (int)((editor.wGetSize().y - 3) / text.getFontSizeSpacing());

	if(editor.getCTRL() && text.getBottomLine() + 2*maxLines - 1 <= text.getLineSize()) {
		text.setBottomLine(text.getBottomLine() + maxLines);
		cursorLineNr += maxLines;
		return true;
	}else {
		cursorLineNr += (text.getLineSize() - maxLines + 1) - text.getBottomLine();
		text.setBottomLine(text.getLineSize() - maxLines + 1);
	}
	return false;
}

bool Cursor::cursorDown(Editor &editor, Text &text) {
	if(editor.getCTRL()) {
		return false;
	}

	if(cursorLineNr + 1 < text.getLineSize()) {
		if((posY+1) > (int)((editor.wGetSize().y - 3) / text.getFontSizeSpacing()) - 1) {
			text.setBottomLine(text.getBottomLine() + 1);
		}else {
			posY++;
		}
		cursorLineNr++;
	}
	return true;
}

void Cursor::setNormalBackspace(Editor &editor, Text &text, int cursorCharNr) {
	int nextChar = 0;
	text.countChars(&nextChar, editor, text.getLine(cursorLineNr), *this, text.countCharsSize);
	if(text.getLine(cursorLineNr).at(nextChar - 1) == '	') {
		posX-=text.getTabWidth();
	}else {
		posX-=text.getCharWidth();
	}
	text.insertText(cursorLineNr, cursorCharNr, ' ', true);
	text.loadTextWidthsBounds(cursorLineNr);
}

void Cursor::setDeleteLineBackspace(Editor &editor, Text &text) {
	std::string deletedLine = text.getLine(cursorLineNr);
	text.deleteLine(cursorLineNr);
	cursorLineNr--;
	text.loadTextWidthsBounds(cursorLineNr);
	posX = editor.getGreyBlockSize() + text.getSize(cursorLineNr, -1);
	text.addText(cursorLineNr, deletedLine);
	text.loadTextWidthsBounds(cursorLineNr);
	posY--;
}

bool Cursor::cursorSimpleBackspace(Editor &editor, Text &text) {
	if(editor.getCTRL()) {
		return false;
	}

	int cursorCharNr = 0;
	text.countChars(&cursorCharNr, editor, text.getLine(cursorLineNr), *this, text.countCharsSize);
	if((posX-editor.getGreyBlockSize()) != 0) {
		setNormalBackspace(editor, text, cursorCharNr);		
	}else if((posX - editor.getGreyBlockSize()) == 0 && cursorLineNr > 0) {
		setDeleteLineBackspace(editor, text);
	}
	return true;
}

bool Cursor::cursorMTBackspace(Editor &editor, Text &text) {
	cursorPosition cp = deleteDefaultMTString(editor, *this, text);
	setPosX(cp.posX);
	setPosY(cp.posY);
	if(text.getBottomLine() > cp.lineNr+1) {
		text.setBottomLine(cp.lineNr+1);
	}	
	setCursorLineNr(cp.lineNr);
	MT.unsetActive();
	return true;
}

bool Cursor::cursorBackspace(Editor &editor, Text &text) {
	if(defaultMTIsActive()) {
		cursorMTBackspace(editor, text);
		text.denieInput();
		return true;
	}else if(cursorSimpleBackspace(editor, text)) {
		text.denieInput();
		return true;
	}	
	return false;
} 

int Cursor::getTabsForNewLine(Text &text, int tillPos) {
	int tabNr = 0;
	for (int i = 0;i<text.getLine(cursorLineNr).length() && i<tillPos;i++) {
		if(text.getLine(cursorLineNr).at(i) == '	') {
			tabNr++;
		}else {
			break;
		}	
	}
	return tabNr;
}

void Cursor::setPosYBottomLine(Editor &editor, Text &text) {
	if((posY+1) > (int)((editor.wGetSize().y - 3) / text.getFontSizeSpacing()) - 1) {
		text.setBottomLine(text.getBottomLine() + 1);
	}else {
		posY++;
	}
}

void Cursor::createNewLineSetNewLineParameters(Editor &editor, Text &text, int &lineNr, int tillPos, int tabNr) {
	std::string insertStr = text.getLine(lineNr).substr(tillPos, text.getLine(lineNr).length());
	std::string remainingStr = text.getLine(lineNr).substr(0, tillPos);
	lineNr++;
	text.insertLines(lineNr, std::string(tabNr, '	') + insertStr);
	text.setText(lineNr-1, remainingStr);
	//text.loadTextWidthsBounds(lineNr);
}

bool Cursor::cursorEnter(Editor &editor, Text &text) {
	if(editor.getCTRL()) {
		return false;
	}

	int strPos = 0;
	text.countChars(&strPos, editor, text.getLine(cursorLineNr), *this, text.countCharsSize);	

	setPosYBottomLine(editor, text);

	int tabNr = getTabsForNewLine(text, strPos);

	createNewLineSetNewLineParameters(editor, text, cursorLineNr, strPos, tabNr);

	posX = editor.getGreyBlockSize() + tabNr * text.getTabWidth();

	return true;
}

int Cursor::getPosXToCharNr(Editor &editor, Text &text, int linePosX, int lineNr) {
	int width = 0;
	for(int i = 0;i<text.getLine(lineNr).size();++i) {
		if(width == linePosX - editor.getGreyBlockSize()) {
			return i;
		}
		
		if(text.getLine(lineNr).at(i) == '	') {
			width += text.getTabWidth();
		}else {
			width += text.getCharWidth();
		}
	}

	if(linePosX - editor.getGreyBlockSize() == text.getSize(lineNr, text.getLine(lineNr).size())) {
		//return text.getLine(lineNr).size()-1;
		return -2;
	}
	return -1;
}

void Cursor::cursorInsertTextMakeNewLine(Editor &editor, Text &text, std::string &textToInsert, int &lineNr, int &linePosX) {
	int j = getPosXToCharNr(editor, text, linePosX, lineNr);

	if(j == -1 || j == -2) {
		textToInsert = text.getLine(lineNr) + textToInsert; 
	}else {
		std::string first = text.getLine(lineNr).substr(0, j);
		std::string second = text.getLine(lineNr).substr(j, text.getLine(lineNr).size() - j);
		textToInsert = first + textToInsert + second;
	}

	text.setText(lineNr, textToInsert);

	std::cout << textToInsert << std::endl;

	for(int i = 0;i<textToInsert.size();++i) {
		char c = textToInsert.at(i);
		if(c == '\r') {
			std::string first = textToInsert.substr(0, i);
			std::string second = textToInsert.substr(i+2, textToInsert.size() - i - 2);
			textToInsert = first + second;
			text.setText(lineNr, first);
			lineNr++;
			text.insertLines(lineNr+1, second);
			//createNewLineSetNewLineParameters(editor, text, lineNr, i, 0);
			textToInsert = text.getLine(lineNr);
		}
	}

	editor.setTitleNotSaved();
}

cursorPosition Cursor::cursorInsertText(Editor &editor, Text &text, std::string textToInsert, int lineNr, int linePosX, int linePosY) {
	cursorPosition returnCP;
	returnCP.lineNr = lineNr;
	returnCP.posX = linePosX;
	returnCP.posY = linePosY;

	if(textToInsert.find('\n', 0) == std::string::npos) {
		text.setText(lineNr, text.getLine(lineNr) + textToInsert);
		return returnCP;
	}

	cursorInsertTextMakeNewLine(editor, text, textToInsert, lineNr, linePosX);

	return returnCP;
}

void Cursor::loadEvents(Editor &editor, sf::Event &event, Text &text) {
	
}

void Cursor::loadDraw(Editor &editor, Text &text, sf::RenderWindow &window) { //Constant Framerate important, no usage of time.
	cursorBackground.setPosition(editor.getGreyBlockSize(), (posY * text.getFontSizeSpacing()) + 3);

	cursorDraw.setPosition(posX, (posY * text.getFontSizeSpacing()) + 3);

	window.draw(cursorBackground);
	
	if(cursorBlink <= 40 && cursorShow == 0) {
		if(cursorBlink > 0) {
			cursorBlink--;
		}else if(cursorBlink == -40) {
			cursorBlink *= -1;
		}else {
			cursorBlink--;
			window.draw(cursorDraw);
		}	
	}else {
		window.draw(cursorDraw);
		cursorShow--;
		cursorBlink = 40;
	}
}

void Cursor::simpleCtrlV(Editor &editor, Text &text) {
	cursorPosition cp;
	cp = cursorInsertText(editor, text, sf::Clipboard::getString().toAnsiString(), getCursorLineNr(), getPosX(), getPosY());
	setCursorLineNr(cp.lineNr);
	setPosX(cp.posX);
	setPosY(cp.posY);
	editor.offCTRL();
	text.denieInput();
}

void Cursor::ctrlV(Editor &editor, Text &text) {
	if(MT.getActive()) {
		cursorPosition cp = MT.deleteString(editor, *this, text);
		setPosX(cp.posX);
		setPosY(cp.posY);
		if(text.getBottomLine() > cp.lineNr+1) {
			text.setBottomLine(cp.lineNr+1);
		}	
		setCursorLineNr(cp.lineNr);
		MT.unsetActive();
		cursorInsertText(editor, text, sf::Clipboard::getString(), cursorLineNr, posX, posY);
		text.denieInput();
	}else if(editor.getCTRL()) {
		simpleCtrlV(editor, text);
	}
}

void Editor::writeFile(Text &text) {
	std::ofstream myfile;

	myfile.open(path, std::ofstream::out | std::ofstream::trunc);

	for(int i = 0;i<text.getLineSize() && myfile.is_open();i++) {
		myfile << text.getLine(i) + "\n";
	}

	myfile.close();

	setTitleSaved();
}

void Editor::openFile(Text &text, Cursor &cursor) {
	std::string file_path = fileDialogWinApi();
	text.loadFile(file_path);
	text.loadText();
	cursor.loadVars(*this);
	path = file_path;
	std::ofstream wfile;
	wfile.open("se_data_current.sedatac", std::ofstream::out | std::ofstream::trunc);
	wfile << file_path;
	wfile.close();
	//cursor.loadCursor(*this, window, text);
}

//This function initializes cursor vars two times.
void Editor::loadEditor() {
	std::ifstream file("se_data_current.sedatac");

	if(!file.is_open()) {
		std::ofstream nfile("se_data_current.sedatac");
		nfile.close();
	}

	std::string file_path;
	if(!getline(file, file_path)) {
		std::ofstream wfile("se_data_current.sedatac");
		file_path = fileDialogWinApi();
		wfile << file_path;
		wfile.close();
	}
	text.loadFile(file_path);
	setFilePath(file_path);

	file.close();

	loadVars();

	loadLoop();
}

void Editor::loadVars() { //TODO: One String for Title

}

void Editor::checkMouse() {
	mouseInRange = false;
	mouse.loadMouseCoordinates(window);

	//std::cout << "Mouse X: " << mcords.x << "; Cursor X: " << cursor.getPosX() << "; Mouse Y: " << mcords.y << "; Cursor Y: " << cursor.getPosY() << std::endl;
	
	if(mouse.getPosX() >= GreyBlockSize && mouse.getPosY() >= 0 && mouse.getPosX() <= window.getSize().x - GreyBlockSize && mouse.getPosY() <= window.getSize().y) {
		sf::Cursor c;
		if(c.loadFromSystem(sf::Cursor::Text))
			window.setMouseCursor(c);
		mouseInRange = true;
	}else if(mouse.getPosX() >= 0 && mouse.getPosX() <= GreyBlockSize || mouse.getPosX() >= window.getSize().x - GreyBlockSize && mouse.getPosX() <= window.getSize().x) {
		/*sf::Cursor c;
		if(c.loadFromSystem(sf::Cursor::SizeAll))
			window.setMouseCursor(c);*/
	}else {
		sf::Cursor c;
		if(c.loadFromSystem(sf::Cursor::Arrow))
			window.setMouseCursor(c);
	}
}

void Editor::loadLoop() {
	while (window.isOpen()) {
		checkMouse();

		loadTotalEvents();

		window.clear();

		loadAllDraws();

		window.display();
    }
}

void Editor::loadDraw() {
	window.draw(background);

	leftNumBlock.setSize(sf::Vector2f(getGreyBlockSize(), window.getSize().y));
	window.draw(leftNumBlock);
}

void Editor::mScrolled(sf::Event &event) {
	if(event.type == sf::Event::MouseWheelScrolled) {
		int maxLines = (int)((wGetSize().y - 3) / text.getFontSizeSpacing());
		int change = (event.mouseWheelScroll.delta > 0) ? (text.getBottomLine() - 1 > 0 ? -1 : 0) : (text.getBottomLine() - 1 >= (text.getLineSize() - maxLines) ? 0 : 1);
		text.setBottomLine(text.getBottomLine() + change);
		cursor.setCursorLineNr(cursor.getCursorLineNr() + change);
	}
}

void Editor::loadEvents(sf::Event &event) {
	mScrolled(event);
}

void Editor::loadTotalEvents() {
	sf::Event event;
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			window.close();
		if (event.type == sf::Event::Resized) {
			// update the view to the new size of the window
			sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
			window.setView(sf::View(visibleArea));
		}

		loadAllEvents(event);
	}
}

void Editor::loadAllDraws() {
	loadDraw();
	cursor.loadDraw(*this, text, window);
	cursor.drawDefaultMarkedText(*this, text, window);
	text.loadDraw(window, *this);
	slider.loadDraw(*this, window);
}

void Editor::dynamicGBS(Cursor &cursor, Text &text) {
	double i = 1.0;
	bool setDo = true;
	while(setDo) {
		if((int)((double)(text.getLineSize()) / (double)(i)) >= 1) {
			i *= 10.0;
			//std::cout << "i: " << ((int)((float)(text.getLineSize()) / (float)(i)) >= 1) << std::endl; 
		}else {
			setDo = false;
		}
	}

	int changeBefore = getGreyBlockSize();
	int change = (int)(std::log(i) / std::log(10.0)) * 8;
	change += 14;
	setGreyBlockSize(change);
	text.loadTextWidthsBounds(cursor.getCursorLineNr());
	cursor.setPosX(cursor.getPosX() - changeBefore + change);

	int *nothing;
	text.countChars(&nothing, *this, text.getLine(cursor.getCursorLineNr()), cursor, text.countCharsSize);
	//std::cout << "das wird getan" << std::endl;
}

void Editor::loadAllEvents(sf::Event &event) {
	if(!window.hasFocus()) {
		return;
	}

	dynamicGBS(cursor, text);

	loadEvents(event);
	//cursor.loadEvents(*this, event, text);

	slider.loadEvents(*this, cursor, text, event, window, mouse.getMouseCords());	

	if(!getCTRL()) {
		text.loadEvents(*this, cursor, event);
	}

	if(!mouse.getInUse()) {
		cursor.cursorMouseEvent(*this, text, event, mouse.getMouseCords());
	}

	if(event.type == sf::Event::MouseButtonReleased) {
		slider.escapeEvent();
		cursor.enableSetPosXAtClick();
		mouse.setInUse(false);
	}

	if(event.type == sf::Event::KeyPressed) {
		loadSwitchKeyPressedEvents(event);
	}	

	if(event.type == sf::Event::KeyReleased) {
		loadSwitchKeyReleasedEvents(event);
	}

	int *nothing;
	text.countChars(&nothing, *this, text.getLine(cursor.getCursorLineNr()), cursor, text.countCharsSize);

	slider.updateSlider(*this, cursor, text);
}

void Editor::loadSwitchKeyPressedEvents(sf::Event &event) {
	switch(event.key.code) {
		case sf::Keyboard::Backspace:
			cursor.cursorBackspace(*this, text);
			break;
		case sf::Keyboard::Left:
			if(cursor.cursorLeft(*this, text)) {
			}	
			break;
		case sf::Keyboard::Right:
			if(cursor.cursorRight(*this, text)) {
			}	
			break;
		case sf::Keyboard::Up:
			if(cursor.cursorUp(*this, text)) {
			}
			break;
		case sf::Keyboard::Down:
			if(cursor.cursorDown(*this, text)) {
			}	
			break;
		case sf::Keyboard::Enter:
			if(cursor.cursorEnter(*this, text)) {
				text.denieInput();
			}	
			break;
		case sf::Keyboard::U:
			if(getCTRL()) {
				cursor.cursorU(*this, text);
				text.denieInput();
			}	
			break;
		case sf::Keyboard::D:
			if(getCTRL()) {
				cursor.cursorD(*this, text);
				text.denieInput();
			}	
			break;
		case sf::Keyboard::O:
			if(getCTRL()) {
				openFile(text, cursor);
				loadVars();
				loadLoop();
				offCTRL();
				text.denieInput();
			}	
			break;
		case sf::Keyboard::S:
			if(getCTRL()) {
				writeFile(text);
				offCTRL();
				text.denieInput();
			}
			break;	
		case sf::Keyboard::V:
			cursor.ctrlV(*this, text);
			break;
		case  sf::Keyboard::C:
			if(getCTRL()) {
				sf::Clipboard::setString(cursor.getStringFromMarkedText(*this, cursor, text));
			}	
			break;
		case sf::Keyboard::LControl:
			onCTRL();
			break;	
		case sf::Keyboard::RAlt: //In order to scheiß auf control
			offCTRL();
			onAlt();
			break;	
		default:
			break;
	}
	cursor.showCursor();
}

void Editor::loadSwitchKeyReleasedEvents(sf::Event &event) {
	switch(event.key.code) {
		case sf::Keyboard::Backspace:
			text.setAllowInput();
			break;
		case sf::Keyboard::Left:
			break;
		case sf::Keyboard::Right:
			break;
		case sf::Keyboard::Up:
			break;
		case sf::Keyboard::Down:
			break;	
		case sf::Keyboard::Enter:
			text.setAllowInput();
			break;
		case sf::Keyboard::LControl:
			offCTRL();
			text.setAllowInput();
			break;
		case sf::Keyboard::RAlt:
			offAlt();
			break;
		default:
			break;
	}
}

/*
*	Either count the number of characters with respect to the tab or change the X-Position of the Cursor if it 
*	would otherwise be stuck inside a tab(when moving up and down it check if we would move inside a tab and then 
*	changes our psoition before the tab). 
*/
void Text::countChars(void* outVal, Editor &editor, std::string s1, Cursor &cursor, void func(int* outVal2, int* igive, int* sgive)) {
	if(s1.length() == 0) {
		*(int *)outVal = 0;
		cursor.setPosX(editor.getGreyBlockSize());	
	}	

	int sizeChars = 0;
	int sizeCharsBefore = 0;
	for(int i = 0;i<s1.size();i++) {
		if(s1.at(i) == '	') {
			sizeCharsBefore = sizeChars;
			sizeChars += textWidths.tabWidth;
		}else {
			sizeChars += textWidths.charWidth;
		}
		if(sizeChars == (cursor.getPosX()) - editor.getGreyBlockSize()) {
			int return1 = 0;
			(func(&return1, &i, &sizeChars));
			(*(int*)(outVal)) = return1;
			return;
		}else if(sizeChars > (cursor.getPosX()) - editor.getGreyBlockSize()) {
			cursor.setPosX(sizeCharsBefore + editor.getGreyBlockSize());
			return;
		}
	}
		
	cursor.setPosX(sizeChars + editor.getGreyBlockSize());
		
}

int Text::getSize(int lineNr, int charNr) {
	int j = 0;
	for(int i = 0;i < lines.at(lineNr).size() && (charNr != -1 ? i < charNr : true);++i) {
		if(lines.at(lineNr).at(i) == '	') {
			j += getTabWidth();
		}else {
			j += getCharWidth();
		}
	}
	return j;
}

void Text::loadEvents(Editor &editor, Cursor &cursor, sf::Event &event) {
	if(event.type == sf::Event::TextEntered && allowInputSet()) {
		int cursorCharNr = 0;
		countChars(&cursorCharNr, editor, getLine(cursor.getCursorLineNr()), cursor, countCharsSize);
		char c = (char)event.text.unicode;
		insertText(cursor.getCursorLineNr(), cursorCharNr, c, false);

		if(c == '	') {
			cursor.setPosX(cursor.getPosX() + getTabWidth());
		}else {
			cursor.setPosX(cursor.getPosX() + getCharWidth());
		}

		loadTextWidthsBounds(cursor.getCursorLineNr());

		editor.setTitleNotSaved();
	}
}

void Text::loadText() {
	loadVars();
	loadTextWidthsBounds(bottomLine-1);
}

void Text::loadVars() {
	bottomLine = 1;
	allowInput = true;
}

void Text::loadFile(std::string file) {
	loadFont("CONSOLA.TTF");

	/*if(lines != NULL) {
		delete(lines);
	}*/

	lines.clear();

    std::ifstream myfile(file);
	int cline;
	std::string line;
	if (myfile.is_open())
	{
		while ( getline(myfile,line) )
		{
		  
		  lines.push_back(line);
		}
		myfile.close();
	}

	textDraw.setFont(font);
	textDraw.setCharacterSize(fontSize);
	textDraw.setColor(sf::Color::Black);
}

int Text::loadFont(std::string sfont) {
	if(!font.loadFromFile(/*"TerminusTTF-4.49.3.ttf"*/ sfont)) {
		std::cout << "no font";
		return 0;
	}
	return 1;
}

void Text::loadTextWidthsBounds(int lineNr) {
	sf::Text text3;
	text3.setFont(font);
	text3.setCharacterSize(fontSize);
	text3.setString(" ");
	textWidths.charWidth = text3.getLocalBounds().width;
	
	text3.setString("	");
	textWidths.tabWidth = text3.getLocalBounds().width;
	
	if(lines.size() > 0) {
		text3.setString(lines.at(lineNr));
		textWidths.textWidth = text3.getLocalBounds().width;
	}	
}

void Text::insertText(int cursorLineNr, int cursorCharNr, char insert, bool deleteB) {
	std::string sinsert;
	if(deleteB) {
		sinsert = "";
	}else {
		sinsert = insert;
	}

	lines.at(cursorLineNr) = lines.at(cursorLineNr).substr(0, cursorCharNr-(deleteB ? 1 : 0)) + sinsert + lines.at(cursorLineNr).substr(cursorCharNr, lines.at(cursorLineNr).size()-(cursorCharNr));
}

void Text::loadDraw(sf::RenderWindow &window, Editor &editor) {
	int ilinec = ((int)bottomLine < 0) ? 0 : (int)bottomLine-1;
	for(int i = 0;ilinec+i<lines.size() && ((i+1)*fontSizeSpacing + 3) <= window.getSize().y;i++) {

		std::vector<string_to_display> strDisplay;

		int before;
		int last;
		for(int j = 0;j<1;j++) {
			int k = 0;
			before = 0;
			last = 0;
			while(k<lines.at(i+ilinec).length()) {
				std::vector<std::string> keywordsNow(keywords);

				for(int z = 0;z<keywordsSpecial.size();z++) {
					keywordsNow.push_back(keywordsSpecial.at(z));
				}

				std::size_t pos = lines.at(i+ilinec).find(keywordsNow.at(j), k);
				int newpos = 0;
				bool specialOnes = false;
				for(int x = 0;x<keywordsNow.size();x++) {
					std::size_t pos2 = lines.at(i+ilinec).find(keywordsNow.at(x), k);
					if(x > keywords.size()-1) {
						specialOnes = true;
					}else {
						specialOnes = false;
					}
					
					if(pos2 < pos) {
						pos = pos2;
						newpos = x;
					}
				}
				//std::cout << "fgjghj" << std::endl;
				if(pos != std::string::npos) {
					if(specialOnes) {
						string_to_display strtop;

						strtop.str = lines.at(i+ilinec).substr(last, pos - last);
						strtop.highlight = false;
						strtop.stdLength = strtop.str.length();
						strDisplay.push_back(strtop);

						strtop.str = lines.at(i+ilinec).substr(pos, keywordsNow.at(newpos).length());
						strtop.highlight = true;
						strtop.stdLength = strtop.str.length();
						strDisplay.push_back(strtop);
						before = pos + keywordsNow.at(newpos).length() ;
						k = pos + keywordsNow.at(newpos).length();
						last = pos + keywordsNow.at(newpos).length();
					}else if(pos > 0 && pos + keywords.at(newpos).length() < lines.at(i+ilinec).length()-1) {
						if(!iswalnum(lines.at(i+ilinec).at(pos-1)) && !iswalnum(lines.at(i+ilinec).at(pos + keywords.at(newpos).length()))) {
							string_to_display strtop;

							strtop.str = lines.at(i+ilinec).substr(last, pos - last);
							strtop.highlight = false;
							strtop.stdLength = strtop.str.length();
							strDisplay.push_back(strtop);

							strtop.str = lines.at(i+ilinec).substr(pos, keywords.at(newpos).length());
							strtop.highlight = true;
							strtop.stdLength = strtop.str.length();
							strDisplay.push_back(strtop);
							before = pos + keywords.at(newpos).length() ;
							k = pos + keywords.at(newpos).length();
							last = pos + keywords.at(newpos).length();
							//std::cout << "adfasdf" << std::endl;

							//string_to_display strtop;
							//k += lines.at(i+ilinec).length();
						}else {
							/*string_to_display strtop;
							strtop.str = lines.at(i+ilinec).substr(last, lines.at(i+ilinec).length());
							strtop.highlight = false;
							strtop.stdLength = strtop.str.length();
							strDisplay.push_back(strtop);*/
							k++;
						}
					}else {
							string_to_display strtop;
							strtop.str = lines.at(i+ilinec).substr(before, pos + keywords.at(newpos).length());
							strtop.highlight = true;
							strtop.stdLength = strtop.str.length();
							strDisplay.push_back(strtop);
							before = pos + keywords.at(newpos).length() ;
							k = pos + keywords.at(newpos).length();
							last = pos + keywords.at(newpos).length();
							//std::cout << "adfasdf" << std::endl;
					}	
				}else {
						string_to_display strtop;
						strtop.str = lines.at(i+ilinec).substr(last, lines.at(i+ilinec).length());
						strtop.highlight = false;
						strtop.stdLength = strtop.str.length();
						strDisplay.push_back(strtop);
						k += lines.at(i+ilinec).length();
				}
			}
		}

		int abstand = 0;
		for(int j = 0;j<strDisplay.size();j++) {
			sf::Font f2 = font;
			if(strDisplay.at(j).highlight) {
				//textDraw.setStyle(sf::Text::Bold);
				textDraw.setColor(sf::Color(0, 119, 0));
			}else {
				textDraw.setColor(sf::Color::Blue);
			}
			/*textDraw.setString(std::to_string(ilinec+1));
			textDraw.setPosition(5, i*fontSizeSpacing+3);
			window.draw(textDraw);*/
		
			textDraw.setString(strDisplay.at(j).str);
			textDraw.setPosition(editor.getGreyBlockSize() + abstand * getCharWidth(), i*fontSizeSpacing + 3);
			window.draw(textDraw);

			for(int k = 0;k<strDisplay.at(j).stdLength;k++) {
				if(strDisplay.at(j).str.at(k) == '	') {
					abstand += 4;
				}else {
					abstand++;
				}
			}

			textDraw.setColor(sf::Color::Black);
			//textDraw.setStyle(sf::Text::Regular);

			//std::cout << strDisplay.at(j).str << std::endl;
		}	
		
		
	
		textDraw.setString(std::to_string(i+ilinec+1) /*strDisplay.at(j).str*/);
		textDraw.setPosition(editor.getGreyBlockSize() - textDraw.getString().getSize() * getCharWidth() - 5, i*fontSizeSpacing+3);
		window.draw(textDraw);
	}	
}

int main() {
	Editor editor;
	EditorMouse m_t;
	Text s_t;
	Cursor s_c;
	Slider slider_t(editor.wGetSize().x - 10, 0);

	s_t.loadText();
	s_c.loadCursor(editor, s_t);

	editor.setTextObj(s_t);
	editor.setCursorObj(s_c);
	editor.setSliderObj(slider_t);
	editor.loadMouse(m_t);
	editor.loadEditor();
}
