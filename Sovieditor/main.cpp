#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <iterator>

//#define _WIN32_WINNT 0x0600 

#ifndef UNICODE
#define UNICODE
#endif 

#define NTDDI_VERSION 0x0A000006 //NTDDI_WIN10_RS5
#define _WIN32_WINNT 0x0A00

#define GreyBlockSize 30

/*#include <windows.h>
#include <shobjidl.h> */

/*int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | 
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog *pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
                IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = pFileOpen->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem *pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {
                        MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
    return 0;
}*/

void countChars(void* outVal, sf::Text t1, int* posx, void func(int* outVal2, int* igive, int* sgive)) {
		std::string s1 = t1.getString();
		
		t1.setString(" ");
		sf::FloatRect countCharsRect = t1.getLocalBounds();
		
		t1.setString("	");
		sf::FloatRect countCharsRectTab = t1.getLocalBounds();
		
		t1.setString("");
		
		if(s1.size() == 0) {
				*(int *)outVal = 0;
				*posx = 0 + GreyBlockSize;
		}	
		
		int sizeChars = 0;
		int sizeCharsBefore = 0;
		for(int i = 0;i<s1.size();i++) {
				if(s1.at(i) == '	') {
					sizeCharsBefore = sizeChars;
					sizeChars += countCharsRectTab.width;
				}else {
					sizeChars += countCharsRect.width;
				}
				if(sizeChars == *posx - GreyBlockSize) {
					int return1 = 0;
					(func(&return1, &i, &sizeChars));
					(*(int*)(outVal)) = return1;
					return;
				}else if(sizeChars > *posx - GreyBlockSize) {
					int return1 = 0;
					(func(&return1, posx, &sizeCharsBefore));
					return;
				}
		}
		
		*posx = sizeChars + GreyBlockSize;
		
}

void countCharsSize(int* outVal, int* i, int* size) {
		(*(int*)(outVal)) = (*i)+1;
}

void countCharsTab(int* i, int* size, int* i2) {
		(*(size)) = *i2 + GreyBlockSize;
}	

class Slider {
	private:
	sf::RenderWindow* window;
	sf::Mouse* mouse;
	sf::Vector2i* mcords;
	bool inUse;
	int linesNr;
	int fontSize;
	float uebersetzung;
	int pos;
	int mPosBef;
	
	void setUebersetzung() {
		uebersetzung = ((float)window->getSize().y /((float)linesNr * (float) fontSize / (float) window->getSize().y ));
	}

	float getUebersetzung() {
		return uebersetzung;
	}	
	
	public:
	Slider() {
		inUse = false;
		linesNr = 0;
		fontSize = 0;
		pos = 0;
		mPosBef = 0;
	}

	Slider(sf::RenderWindow* win,sf::Mouse* mouse, sf::Vector2i* mcords, int linesNr, int fontSize) {
		this->window = win;
		this->mouse = mouse;
		this->mcords = mcords;
		inUse = false;
		this->linesNr = linesNr;
		this->fontSize = fontSize;
		pos = 0;
		mPosBef = 0;
	}
	
	void setLines(int linesNr) {
		this->linesNr = linesNr;
	}

	void setFontSize(int fontSize) {
		this->fontSize = fontSize;
	}	

	void draw() {
		setUebersetzung();
		
		sf::CircleShape cs;
		cs.setRadius(2.5);
		cs.setPosition(sf::Vector2f(window->getSize().x - 7, pos+2.5));
		
		sf::CircleShape cs2;
		cs2.setRadius(2.5);
		
		sf::RectangleShape rs;
		rs.setSize(sf::Vector2f(5, (float)getUebersetzung()-10));
		
		cs2.setPosition(sf::Vector2f(window->getSize().x - 7, pos+(int)getUebersetzung()-7));
		rs.setPosition(sf::Vector2f(window->getSize().x - 7, pos+5));
		
		if(inUse) {
			cs.setFillColor(sf::Color(100, 100, 100));
			cs2.setFillColor(sf::Color(100, 100, 100));
			rs.setFillColor(sf::Color(100, 100, 100));
		}else {
			cs.setFillColor(sf::Color(200, 200, 200));
			cs2.setFillColor(sf::Color(200, 200, 200));
			rs.setFillColor(sf::Color(200, 200, 200));
		}

		window->draw(cs);
		window->draw(cs2);
		window->draw(rs);
	}

	void getEvent(sf::Event *ev, float* touchY, int* cursorPosY, int* cursorLineNr) {
		if(mcords->x >= window->getSize().x-7 && mcords->y >= pos+2.5 && mcords->x <= window->getSize().x-2 && mcords->y <= pos+(int)getUebersetzung()+5) {
			sf::Cursor c;
			if(c.loadFromSystem(sf::Cursor::Arrow))
					window->setMouseCursor(c);	
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !inUse) {
				inUse = true;
				mPosBef = mcords->y;
			}	
		}
		
		if(inUse) {
			if(mcords->x >= GreyBlockSize && mcords->y >= 0 && mcords->x <= window->getSize().x && mcords->y + (int)getUebersetzung() + 10 <= window->getSize().y) {
				int difference = (float)mPosBef - mcords->y;
				
				float ue = (float)(linesNr) / ((float)window->getSize().y - getUebersetzung());
				
				*touchY = (float)pos * ue;
				*cursorPosY-=*touchY;
				
				pos += mcords->y - mPosBef;
				mPosBef = mcords->y;
			}	
		}	
	}
	
	void escapeEvent() {
		inUse = false;
		mPosBef = 0;
	}	
};	

int main()
{
	
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1200, 900), "Sovieditor");
	window.setFramerateLimit(60);
 
    // Load a sprite to display
  /*  sf::Texture texture;
    if (!texture.loadFromFile("cute_image.jpg"))
        return EXIT_FAILURE;
	sf::Sprite sprite(texture);*/
 
	int fontSize = 20;
 
	std::vector<std::string> lines;
 
	std::ifstream myfile ("testfile.txt");
	int cline;
	std::string line;
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
		  
		  lines.push_back(line);
		}
		myfile.close();
	}
 
	std::cout << "ffffffff";
 
    // Create a graphical text to display
    sf::Font font;
    if (!font.loadFromFile("TerminusTTF-4.49.3.ttf"))
        return EXIT_FAILURE;
    sf::Text text;
	text.setFont(font);
	text.setCharacterSize(14);
	text.setColor(sf::Color::Black);
	sf::RectangleShape leftNumBlock;
	leftNumBlock.setSize(sf::Vector2f(30, window.getSize().y));
	leftNumBlock.setFillColor(sf::Color(210, 210, 210));
	leftNumBlock.setPosition(0, 0);
	
	sf::RectangleShape def;
	
	sf::RectangleShape cursor;
	cursor.setFillColor(sf::Color::Black);
	cursor.setSize(sf::Vector2f(1, fontSize));
 
    // Load a music to play
	float touchY = 0;
	
	bool inText = 0;
	int lineNr = 0;
	int charNr = 0;
	int cursorPosX = 30;
	int cursorLineNr = 0;
	int cursorPosY = 0;
	int cursorBlink = 30;
	int cursorNotShow = 0;
	
	sf::Text text3;
	text3.setFont(font);
	text3.setCharacterSize(14);
	text3.setColor(sf::Color::Black);
	text3.setString(" ");
	sf::FloatRect FontPos = text3.getLocalBounds();
	
	text3.setString("	");
	sf::FloatRect FontPosTab = text3.getLocalBounds();
	
	text3.setString(lines.at(cursorLineNr));
	sf::FloatRect TextLocalBounds = text3.getLocalBounds();
	
	cursor.setPosition(30, 0);	
	
	bool allowChar = true;
	bool checkCursorLineNr = false;
	int rightkeyp = 0;
	sf::Mouse m;
	sf::Vector2i mcords;
	bool mouseInRange = false;
	
	//sf::RenderWindow* win,sf::Mouse* mouse, sf::Vector2i* mcords, int linesNr, int fontSize
	
	Slider s(&window, &m, &mcords, lines.size(), fontSize);
	
    // Start the game loop
    while (window.isOpen())
    {
		mouseInRange = false;
		mcords = m.getPosition(window);
		
		if(mcords.x >= GreyBlockSize && mcords.y >= 0 && mcords.x <= window.getSize().x - GreyBlockSize && mcords.y <= window.getSize().y) {
				sf::Cursor c;
				if(c.loadFromSystem(sf::Cursor::Text))
					window.setMouseCursor(c);
				mouseInRange = true;
		}else if(mcords.x >= 0 && mcords.x <= GreyBlockSize || mcords.x >= window.getSize().x - GreyBlockSize && mcords.x <= window.getSize().x) {
				/*sf::Cursor c;
				if(c.loadFromSystem(sf::Cursor::SizeAll))
					window.setMouseCursor(c);*/
		}else {
				sf::Cursor c;
				if(c.loadFromSystem(sf::Cursor::Arrow))
					window.setMouseCursor(c);
		}	
		
        // Process events
        sf::Event event;
		while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
			 if (event.type == sf::Event::Resized) {
				// update the view to the new size of the window
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea)); 
				
				leftNumBlock.setSize(sf::Vector2f(GreyBlockSize, window.getSize().y));
			}
			 if(event.type == sf::Event::MouseWheelScrolled) {
				int change = (event.mouseWheelScroll.delta > 0) ? ((event.mouseWheelScroll.delta + touchY < 0) ? 0 : -1) : (((int)(1 + touchY)+(int)((window.getSize().y - 3) / 20) > lines.size()) ? 0 : 1);
				cursorPosY -= change;
				touchY += change;
			}	 	
			
			s.getEvent(&event, &touchY, &cursorPosY, &cursorLineNr);
			
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && mouseInRange) {
				
				inText = 1;
				mcords = m.getPosition(window);
				lineNr = round(((float)mcords.y - 3) / fontSize);
				
				cursorLineNr = lineNr;
				
				text3.setString(lines.at(cursorLineNr));
				TextLocalBounds = text3.getLocalBounds();
				
				sf::Text text2;
				text2.setFont(font);
				text2.setCharacterSize(14);
				text2.setColor(sf::Color::Black);
				text2.setString(lines.at(lineNr));
				float charPosBefore = 0;
				std::string charStr = "";
				for(int i = 0;i<lines.at(lineNr).size();i++) {
			   	   	sf::Vector2f charPos = text2.findCharacterPos(i);
					charStr += std::string(1, (char)lines.at(lineNr).at(i));
					if(charPosBefore < abs(charPos.x-mcords.x)) {
						text2.setString(charStr);
						sf::FloatRect lb = text2.getLocalBounds();
						cursorPosX = lb.width + GreyBlockSize;
						break;
					}else {
						charPosBefore = abs(charPos.x-mcords.x);
					}
				}
				std::cout << charNr;
			}
			
			if (event.type == sf::Event::MouseButtonReleased) {
				s.escapeEvent();
			}			
			
			if(event.type == sf::Event::KeyPressed) { //Always update text3
				if (event.key.code == sf::Keyboard::Backspace) {
					int cursorCharNr = 0;
					countChars(&cursorCharNr, text3, &cursorPosX, countCharsSize);
					if((cursorPosX-30) != 0) {
						int nextChar = 0;
						countChars(&nextChar, text3, &cursorPosX, countCharsSize);
						if(lines.at(cursorLineNr).at(nextChar - 1) == '	') {
							cursorPosX-=FontPosTab.width;
						}else {
							cursorPosX-=FontPos.width;
						}		
					}
					lines.at(cursorLineNr) = lines.at(cursorLineNr).substr(0, cursorCharNr-1) + lines.at(cursorLineNr).substr(cursorCharNr, lines.at(cursorLineNr).size()-(cursorCharNr));
					std::cout << "wurde geklickt" << std::endl;
					allowChar = false;
					checkCursorLineNr = true;
				}else if (event.key.code == sf::Keyboard::Left) {
					if((cursorPosX-30) != 0) {
						int nextChar = 0;
						countChars(&nextChar, text3, &cursorPosX, countCharsSize);
						if(lines.at(cursorLineNr).at(nextChar - 1) == '	') {
							cursorPosX-=FontPosTab.width;
						}else {
							cursorPosX-=FontPos.width;
						}
					}
				}else if (event.key.code == sf::Keyboard::Right) {
					rightkeyp++;
					int charPosSize = 0;
					countChars(&charPosSize, text3, &cursorPosX, countCharsSize);
					std::cout << charPosSize << "charPosSize" << std::endl;
					if(charPosSize < lines.at(cursorLineNr).size()) {
						int nextChar = 0;
						countChars(&nextChar, text3, &cursorPosX, countCharsSize);
						if(lines.at(cursorLineNr).at(nextChar) == '	') {
							cursorPosX+=FontPosTab.width;
						}else if((cursorPosX) != TextLocalBounds.width+30) {
							cursorPosX+=FontPos.width;
						}
					}
				}else if (event.key.code == sf::Keyboard::Up) {
					cursorLineNr--;
					if((cursorLineNr) < 0) {
						cursorLineNr++;
					}else if((cursorPosY-1) < 0) {
						touchY--;
					}else {
						cursorPosY--;
					}
					checkCursorLineNr = true;
				}else if (event.key.code == sf::Keyboard::Down) {
					if(cursorLineNr + 1 < lines.size()) {
						if((cursorPosY+1) > (int)((window.getSize().y - 3) / fontSize) - 1) {
							touchY++;
						}else {
							cursorPosY++;
						}
						checkCursorLineNr = true;
						cursorLineNr++;
					}
				}else if (event.key.code == sf::Keyboard::Tab) {
					int cursorCharNr = 0;
					countChars(&cursorCharNr, text3, &cursorPosX, countCharsSize);
					lines.at(cursorLineNr) = lines.at(cursorLineNr).substr(0, cursorCharNr) + '	' + lines.at(cursorLineNr).substr(cursorCharNr, lines.at(cursorLineNr).size()-(cursorCharNr));
					cursorPosX+=FontPosTab.width;
					allowChar = false;
					checkCursorLineNr = true;
				}else if (event.key.code == sf::Keyboard::Enter) {
					std::string tabs = "";
					int tabNr = 0;
					for (int i = 0;i<lines.at(cursorLineNr).size();i++) {
						if(lines.at(cursorLineNr).at(i) == '	') {
							tabs += "	";
							tabNr++;
						}else {
							break;
						}	
					}	
					cursorLineNr++;
					cursorPosY++;
					lines.insert(lines.begin() + cursorLineNr, tabs);
					cursorPosX = GreyBlockSize + tabNr * (int)FontPosTab.width;
					checkCursorLineNr = true;
					allowChar = false;
				}	
				
				cursorNotShow = 20;
				
				if(checkCursorLineNr) {
					text3.setString(lines.at(cursorLineNr));
					TextLocalBounds = text3.getLocalBounds();
					checkCursorLineNr = false;
					int outVal = 0;
					countChars(&outVal, text3, &cursorPosX, countCharsTab);
				}
			}
			
			if(event.type == sf::Event::KeyReleased) {
				if (event.key.code == sf::Keyboard::Backspace) {
					allowChar = true;
				}else if (event.key.code == sf::Keyboard::Tab) {
					allowChar = true;
				}else if (event.key.code == sf::Keyboard::Enter) {
					allowChar = true;
				}	
			}
			
			if(event.type == sf::Event::TextEntered && allowChar) {
				int cursorCharNr = 0;
				countChars(&cursorCharNr, text3, &cursorPosX, countCharsSize);
				lines.at(cursorLineNr) = lines.at(cursorLineNr).substr(0, cursorCharNr) + (char)event.text.unicode + lines.at(cursorLineNr).substr(cursorCharNr, lines.at(cursorLineNr).size()-(cursorCharNr));
				cursorPosX+=FontPos.width;
				std::cout << "char entered";
				text3.setString(lines.at(cursorLineNr));
				TextLocalBounds = text3.getLocalBounds();
			}
			
			cursor.setPosition(cursorPosX, cursorPosY*fontSize+3);	
        }
 
        // Clear screen
        window.clear();
		
		def.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
		def.setPosition(0, 0);
		def.setFillColor(sf::Color::White);
		
		window.draw(def);
		
		def.setSize(sf::Vector2f(window.getSize().x, fontSize));
		def.setPosition(0, (cursorPosY * fontSize) + 3);
		def.setFillColor(sf::Color(240, 240, 240));
		
		window.draw(def);
		
		window.draw(leftNumBlock);
		
		int ilinec = ((int)touchY < 0) ? 0 : (int)touchY;
		for(int i = 0;ilinec<lines.size() && ((i+1)*fontSize + 3) <= window.getSize().y;i++) {
				text.setString(std::to_string(ilinec+1));
				text.setPosition(5, i*fontSize+3);
				window.draw(text);
				
			
				text.setString(lines.at(ilinec));
				text.setPosition(GreyBlockSize, i*fontSize + 3);
				window.draw(text);
				ilinec++;
		}	
		
		window.draw(text);
		
		if(cursorBlink <= 40 && cursorNotShow == 0) {
			if(cursorBlink > 0) {
				window.draw(cursor);
				cursorBlink--;
			}else if(cursorBlink == -40) {
				cursorBlink *= -1;
			}else {
				cursorBlink--;
			}	
		}else {
			window.draw(cursor);
			cursorNotShow--;
		}	
 
		s.draw();
 
        // Update the window
        window.display();
    }
 
    return 0;
}