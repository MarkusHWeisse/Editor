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
            std::cout << "came till here";
        }
        CoUninitialize();
    }    
    return s1;
}

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

class Slider;
class Cursor;
class Editor;
class Text;

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

public:
	Text() {
		keywords = {"/*", "*/", "//", "void", "int", "float", "bool", "return", "break", "switch", "case", "class", "struct", "if", "else", "while", "for"};
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
	sf::RectangleShape cursorDraw;
	sf::RectangleShape cursorBackground;

public:

	Cursor() {

	}

	void loadEvents(Editor &editor, sf::Event &event, Text &text);
	void loadDraw(Editor &editor, Text &text, sf::RenderWindow &window);
	void loadCursor(Editor &editor, sf::RenderWindow &window, Text &text);
	void loadVars(Editor &editor);
	void setCursorRects(Editor &editor, sf::RenderWindow &window, Text &text);
	bool cursorLeft(Editor &editor, Text &text);
	bool cursorRight(Editor &editor, Text &text);
	bool cursorUp(Editor &editor, Text &text);
	bool cursorDown(Editor &editor, Text &text);
	bool cursorBackspace(Editor &editor, Text &text);
	bool cursorEnter(Editor &editor, Text &text);
	bool cursorU(Editor &editor, Text &text);
	bool cursorD(Editor &editor, Text &text);

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
	int GreyBlockSize;
	bool setCTRL;
	bool setAlt;
	std::string path;
	std::string wTitle;

public:
	Editor() {

	}

	void loadEvents();
	void loadDraw();
	void loadEditor();
	void loadAllEvents(sf::Event &event);
	void loadAllDraws();
	void loadLoop();
	void loadVars();
	void openFile(Text &text, Cursor &cursor);
	void writeFile(Text &text);
	void dynamicGBS(Cursor &cursor, Text &text);

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

void Cursor::setCursorRects(Editor &editor, sf::RenderWindow &window, Text &text) {
	cursorBackground.setSize(sf::Vector2f(window.getSize().x - editor.getGreyBlockSize(), text.getFontSizeSpacing()));
	cursorBackground.setPosition(editor.getGreyBlockSize(), (posY * text.getFontSizeSpacing()) + 3);
	cursorBackground.setFillColor(sf::Color(240, 240, 240));

	cursorDraw.setSize(sf::Vector2f(1, text.getFontSizeSpacing()));
	cursorDraw.setPosition(posX, (posY * text.getFontSizeSpacing()) + 3);
	cursorDraw.setFillColor(sf::Color::Black);
}

void Cursor::loadCursor(Editor &editor, sf::RenderWindow &window, Text &text) {
	loadVars(editor);
	
	setCursorRects(editor, window, text);
}

void Cursor::loadVars(Editor &editor) {
	posY = 0;
	posX = editor.getGreyBlockSize();
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
		std::cout << maxLines << std::endl;
		return true;
	}else {
		maxLines = text.getBottomLine() - 1;
		text.setBottomLine(1);
		cursorLineNr -=  maxLines;
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

	if(editor.getCTRL() && text.getBottomLine() < text.getLineSize() - maxLines - 1) {
		text.setBottomLine(text.getBottomLine() + maxLines);
		cursorLineNr += maxLines;
		std::cout << maxLines << std::endl;
		std::cout << "dgsfdgsdfg";
		return true;
	}else {
		text.setBottomLine(text.getLineSize() - maxLines);
		cursorLineNr = text.getLineSize() - maxLines;
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

bool Cursor::cursorBackspace(Editor &editor, Text &text) {
	if(editor.getCTRL()) {
		return false;
	}

	int cursorCharNr = 0;
	text.countChars(&cursorCharNr, editor, text.getLine(cursorLineNr), *this, text.countCharsSize);
	if((posX-editor.getGreyBlockSize()) != 0) {
		int nextChar = 0;
		text.countChars(&nextChar, editor, text.getLine(cursorLineNr), *this, text.countCharsSize);
		if(text.getLine(cursorLineNr).at(nextChar - 1) == '	') {
			posX-=text.getTabWidth();
		}else {
			posX-=text.getCharWidth();
		}
		text.insertText(cursorLineNr, cursorCharNr, ' ', true);
		text.loadTextWidthsBounds(cursorLineNr);		
	}else if((posX - editor.getGreyBlockSize()) == 0 && cursorLineNr > 0) {
		std::string deletedLine = text.getLine(cursorLineNr);
		text.deleteLine(cursorLineNr);
		cursorLineNr--;
		text.loadTextWidthsBounds(cursorLineNr);
		posX = editor.getGreyBlockSize() + text.getLine(cursorLineNr).length() * text.getCharWidth();
		text.addText(cursorLineNr, deletedLine);
		text.loadTextWidthsBounds(cursorLineNr);
		posY--;

	}
	return true;
}

bool Cursor::cursorEnter(Editor &editor, Text &text) {
	if(editor.getCTRL()) {
		return false;
	}

	std::string tabs = "";
	int tabNr = 0;
	for (int i = 0;i<text.getLine(cursorLineNr).length();i++) {
		if(text.getLine(cursorLineNr).at(i) == '	') {
			tabs += "	";
			tabNr++;
		}else {
			break;
		}	
	}	
	
	if((posY+1) > (int)((editor.wGetSize().y - 3) / text.getFontSizeSpacing()) - 1) {
		text.setBottomLine(text.getBottomLine() + 1);
	}else {
		posY++;
	}	
	
	int strPos = 0;
	text.countChars(&strPos, editor, text.getLine(cursorLineNr), *this, text.countCharsSize);
	std::string insertStr = text.getLine(cursorLineNr).substr(strPos, text.getLine(cursorLineNr).length());
	std::string remainingStr = text.getLine(cursorLineNr).substr(0, strPos);
	cursorLineNr++;
	text.insertLines(cursorLineNr, tabs + insertStr);
	text.setText(cursorLineNr-1, remainingStr);
	posX = editor.getGreyBlockSize() + tabNr * text.getTabWidth();
	text.loadTextWidthsBounds(cursorLineNr);

	return true;
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
	//cursor.loadCursor(*this, window, text);
}

//This function initializes cursor vars two times.
void Editor::loadEditor() {
	text.setFontSize(14, 20);
	openFile(text, cursor);

	cursor.loadCursor(*this, window, text);

	window.create(sf::VideoMode(1200, 900), "Sovieditor");
	window.setFramerateLimit(60);

	loadVars();

	loadLoop();
}

void Editor::loadVars() { //TODO: One String for Title
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

void Editor::loadLoop() {
	while (window.isOpen()) {
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

void Editor::loadEvents() {

}

void Editor::loadAllDraws() {
	loadDraw();
	cursor.loadDraw(*this, text, window);
	text.loadDraw(window, *this);
}

void Editor::dynamicGBS(Cursor &cursor, Text &text) {
	double i = 1.0;
	bool setDo = true;
	while(setDo) {
		if((int)((double)(text.getLineSize()) / (double)(i)) >= 1) {
			i *= 10.0;
			//std::cout << "i: " << ((int)((float)(text.getLineSize()) / (float)(i)) >= 1) << std::endl; 
		}else {
			i *= 0.1;
			setDo = false;
		}
	}

	int changeBefore = getGreyBlockSize();
	if(i > 100) {
		int change = (int)((i/100.0) * 4);
		setGreyBlockSize(change);
		text.loadTextWidthsBounds(cursor.getCursorLineNr());
		cursor.setPosX(cursor.getPosX() - changeBefore + change);

		int *nothing;
		text.countChars(&nothing, *this, text.getLine(cursor.getCursorLineNr()), cursor, text.countCharsSize);
		//std::cout << "das wird getan" << std::endl;
	}
}

void Editor::loadAllEvents(sf::Event &event) {
	dynamicGBS(cursor, text);

	loadEvents();
	//cursor.loadEvents(*this, event, text);

	if(!getCTRL()) {
		text.loadEvents(*this, cursor, event);
	}

	if(event.type == sf::Event::KeyPressed) {
		switch(event.key.code) {
			case sf::Keyboard::Backspace:
				if(cursor.cursorBackspace(*this, text)) {
					text.denieInput();
				}	
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
					loadEditor();
					std::cout << "doing this";
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
			case sf::Keyboard::LControl:
				onCTRL();
				std::cout << "LControl pressed";
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

	if(event.type == sf::Event::KeyReleased) {
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

	int *nothing;
	text.countChars(&nothing, *this, text.getLine(cursor.getCursorLineNr()), cursor, text.countCharsSize);
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
	
	text3.setString(lines.at(lineNr));
	textWidths.textWidth = text3.getLocalBounds().width;
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
	for(int i = 0;ilinec<lines.size() && ((i+1)*fontSizeSpacing + 3) <= window.getSize().y;i++) {

		std::vector<string_to_display> strDisplay;

		int before;
		int last;
		for(int j = 0;j<1;j++) {
			int k = 0;
			before = 0;
			last = 0;
			while(k<lines.at(i+ilinec).length()) {
				std::size_t pos = lines.at(i+ilinec).find(keywords.at(j), k);
				int newpos = 0;
				for(int x = 0;x<keywords.size();x++) {
					std::size_t pos2 = lines.at(i+ilinec).find(keywords.at(x), k);
					if(pos2 < pos) {
						pos = pos2;
						newpos = x;
					}
				}
				//std::cout << "fgjghj" << std::endl;
				if(pos != std::string::npos) {
					if(pos > 0 && pos + keywords.at(newpos).length() < lines.at(i+ilinec).length()-1) {
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
				textDraw.setColor(sf::Color::Blue);
			}else {
				textDraw.setColor(sf::Color::Black);
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
		textDraw.setPosition(5, i*fontSizeSpacing+3);
		window.draw(textDraw);
	}	
}

int main() {
	//std::cout << fileDialogWinApi();

	Editor editor;
	editor.loadEditor();
}