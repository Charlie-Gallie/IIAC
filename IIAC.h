#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <Windows.h>

struct vec2 {
	int
		x = 0,
		y = 0;

	bool operator ==(const vec2& _v) { return x == _v.x && y == _v.y; }
	friend std::ostream& operator <<(std::ostream& os, const vec2& _v) {
		os << _v.x << " : " << _v.y;
		return os;
	}
};

struct Keycode {
	int windowsKeycode = 0;
	char
		lowercase = '\0',
		uppercase = '\0';
	bool isTypable = true;
	bool affectedByCapsLock = true;
	char character = '\0';
};

std::vector<Keycode> keycodesUK = {
	{ '1', '1', '!' },
	{ '2', '2', '"' },
	{ '3', '3', '£' },
	{ '4', '4', '$' },
	{ '5', '5', '%' },
	{ '6', '6', '^' },
	{ '7', '7', '&' },
	{ '8', '8', '*' },
	{ '9', '9', '(' },
	{ '0', '0', ')' },

	{ 'A', 'a', 'A' },
	{ 'B', 'b', 'B' },
	{ 'C', 'c', 'C' },
	{ 'D', 'd', 'D' },
	{ 'E', 'e', 'E' },
	{ 'F', 'f', 'F' },
	{ 'G', 'g', 'G' },
	{ 'H', 'h', 'H' },
	{ 'I', 'i', 'I' },
	{ 'J', 'j', 'J' },
	{ 'K', 'k', 'K' },
	{ 'L', 'l', 'L' },
	{ 'M', 'm', 'M' },
	{ 'N', 'n', 'N' },
	{ 'O', 'o', 'O' },
	{ 'P', 'p', 'P' },
	{ 'Q', 'q', 'Q' },
	{ 'R', 'r', 'R' },
	{ 'S', 's', 'S' },
	{ 'T', 't', 'T' },
	{ 'U', 'u', 'U' },
	{ 'V', 'v', 'V' },
	{ 'W', 'w', 'W' },
	{ 'X', 'x', 'X' },
	{ 'Y', 'y', 'Y' },
	{ 'Z', 'z', 'Z' },

	{ VK_SPACE,			' ', ' ' },
	{ VK_OEM_COMMA,		',', '<', true, false },
	{ VK_OEM_PERIOD,	'.', '>', true, false },
	{ VK_OEM_MINUS,		'-', '_', true, false },
	{ VK_OEM_PLUS,		'=', '+', true, false },
	{ VK_OEM_1,			';', ':', true, false },
	{ VK_OEM_2,			'/', '?', true, false },
	{ VK_OEM_3,			'\'', '@', true, false },
	{ VK_OEM_4,			'[', '{', true, false },
	{ VK_OEM_5,			'\\', '|', true, false },
	{ VK_OEM_6,			']', '}', true, false },
	{ VK_OEM_7,			'#', '~', true, false },

	{ VK_TAB,			'\t', '\t', false, false },
	{ VK_BACK,			'\b', '\b', false, false },
	{ VK_RETURN, VK_RETURN, VK_RETURN, false, false },

	{ VK_LEFT, VK_LEFT, VK_LEFT, false, false },
	{ VK_RIGHT, VK_RIGHT, VK_RIGHT, false, false },
	{ VK_UP, VK_UP, VK_UP, false, false },
	{ VK_DOWN, VK_DOWN, VK_DOWN, false, false },

	{ VK_LEFT, VK_LEFT, VK_LEFT, false, false }
};

std::vector<Keycode> localKeycodes = keycodesUK;

Keycode getKeyPress() {
	// Returns a keycode object where keycode.character is intended character
	for (Keycode keycode : localKeycodes) {
		if (GetAsyncKeyState(keycode.windowsKeycode) & 1) { // & 1 Gives normal character repeats when holding the key
			keycode.character = GetKeyState(VK_CAPITAL) || GetAsyncKeyState(VK_SHIFT) ? keycode.uppercase : keycode.lowercase;
			return keycode;
		}
	}

	return Keycode();
}

class IIAC {
public:
	IIAC() {
		scrollingInitiallyEnabled = isScrollingEnabled();
		enableScrolling(false);
	}

	void edit() {
		while (1) {
			handleConsoleUpdates();

			Keycode keycode = getKeyPress();
			if (keycode.character != '\0') {
				if (keycode.isTypable) {
					lines[cursorPos.y].insert(cursorPos.x, std::string(1, keycode.character));
					cursorPos.x++;

					updateText();
					continue;
				}

				/*		Backspace		*/
				if (keycode.character == VK_BACK) {
					if (cursorPos.x > 0) {
						lines[cursorPos.y].erase(cursorPos.x - 1, 1);
						cursorPos.x--;
					}
					/*		Backspacing onto previous line		*/
					else if (cursorPos.y > 0) {
						cursorPos.x = lines[cursorPos.y - 1].size();
						lines[cursorPos.y - 1].append(lines[cursorPos.y].substr(0));
						lines.erase(lines.begin() + cursorPos.y);
						cursorPos.y--;
					}
				}

				/*		Return		*/
				if (keycode.character == VK_RETURN) {
					lines.insert(lines.begin() + cursorPos.y + 1, lines[cursorPos.y].substr(cursorPos.x));
					lines[cursorPos.y].erase(lines[cursorPos.y].begin() + cursorPos.x, lines[cursorPos.y].end());

					cursorPos.y++;
					cursorPos.x = 0;
				}

				/*		Cursor controls		*/
				if (keycode.character == VK_LEFT) {
					if(cursorPos.x > 0)
						cursorPos.x--;
					else if (cursorPos.y > 0) {
						cursorPos.x = (int)lines[cursorPos.y - 1].size();
						cursorPos.y--;
					}
				}
				if (keycode.character == VK_RIGHT) {
					if(cursorPos.x < lines[cursorPos.y].size())
						cursorPos.x++;
					else if (cursorPos.y < (int)lines.size() - 1) {
						cursorPos.x = 0;
						cursorPos.y++;
					}
				}

				if (keycode.character == VK_UP && cursorPos.y > 0) {
					cursorPos.y--;
					if (lines[cursorPos.y].size() < cursorPos.x) cursorPos.x = (int)lines[cursorPos.y].size();
				}

				if (keycode.character == VK_DOWN && cursorPos.y < lines.size() - 1) {
					cursorPos.y++;
					if (lines[cursorPos.y].size() < cursorPos.x) cursorPos.x = (int)lines[cursorPos.y].size();
				}

				/*		Tab		*/
				if (keycode.character == '\t') { 
					/*		Tabs need to be spaces for now because adding 4 to cursorPos		*/
					/*		indexes out of bounds therefore a crash - and without the			*/
					/*		+4 it is misaligned.												*/
					for (int n = 0; n < 4; n++) {
						lines[cursorPos.y].insert(cursorPos.x, std::string(1, ' '));
						cursorPos.x++;
					}
				}

				updateText();
			}
		}
	}

private:
	std::deque<std::string> lines = { "" };
	vec2 consoleSize, cursorPos;
	bool scrollingInitiallyEnabled; // To revert it back to default when done editing text

	void updateText() {
		showCursor(false);
		//drawHeader();

		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });

		for (size_t lineIndex = 0; lineIndex < lines.size(); lineIndex++) {
			for (size_t characterIndex = 0; characterIndex < lines[lineIndex].size(); characterIndex++) {
				std::cout << lines[lineIndex][characterIndex];
			}

			for (size_t whitespaceIndex = lines[lineIndex].size() + 1; whitespaceIndex < getConsoleSize().x; whitespaceIndex++) {
				std::cout << " ";
			}

			std::cout << std::endl;
		}

		std::string whitespaceLine = "";
		for (int n = 0; n < getConsoleSize().x; n++) whitespaceLine += " ";

		for (size_t whitespaceLines = lines.size() + 2; whitespaceLines < getConsoleSize().y; whitespaceLines++) {
			fwrite(whitespaceLine.c_str(), sizeof(char), sizeof(whitespaceLine.c_str()), stdout); // Using fwrite() because it's faster than cout
		}

		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)cursorPos.x, (short)cursorPos.y });
		showCursor(true);
	}

	uint8_t drawHeader() {
		std::cout << "someFile.txt" << std::endl;
		std::string line;
		line.insert(line.begin(), getConsoleSize().x, '_');
		std::cout << line << std::endl;

		return 1;
	}

	void showCursor(bool shown) {
		HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO cursorInfo;

		GetConsoleCursorInfo(out, &cursorInfo);
		cursorInfo.bVisible = shown;
		SetConsoleCursorInfo(out, &cursorInfo);
	}

	void mousePress(MOUSE_EVENT_RECORD mouseEvent) {
		if (mouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
			vec2 mousePos = {
				mouseEvent.dwMousePosition.X,
				mouseEvent.dwMousePosition.Y
			};

			/*		Set the text cursor position to where was clicked		*/
			cursorPos.y = std::clamp(mousePos.y, 0, (int)lines.size() - 1);
			cursorPos.x = std::clamp(mousePos.x, 0, (int)lines[cursorPos.y].size());

			updateText();
		}		
	}

	void handleConsoleUpdates() {
		INPUT_RECORD eventsBuffer[128];
		HANDLE  stdHandle = GetStdHandle(STD_INPUT_HANDLE);

		/*		Enables mouse input events		*/
		SetConsoleMode(stdHandle, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);


		/*		Retrieves the number of console events			*/
		/*						-------							*/
		/*		This prevents ReadConsoleInput() from			*/
		/*		holding up the program while waiting for a		*/
		/*		console event.									*/
		DWORD numberOfConsoleEvents;
		GetNumberOfConsoleInputEvents(stdHandle, &numberOfConsoleEvents);

		/*		Reads any existing events		*/
		if (numberOfConsoleEvents > 0) {
			ReadConsoleInput(
				stdHandle,				// Stdin Handle
				eventsBuffer,			// Some buffer to write to
				128,					// Buffer size
				&numberOfConsoleEvents	// Number of events
			);
		}
		else return;

		/*		Check for console resize event		*/
		for (DWORD i = 0; i < numberOfConsoleEvents; i++) {
			switch (eventsBuffer[i].EventType) {
			case MOUSE_EVENT:
				mousePress(eventsBuffer[i].Event.MouseEvent);
				break;

			case WINDOW_BUFFER_SIZE_EVENT:
				consoleSize = getConsoleSize();

				// Prevents the buffer being left too large therefore enabling scrolling
				enableScrolling(false);
				break;
			}
		}
	}

	vec2 getConsoleSize() {
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

		/*		Return the console size in characters		*/
		return vec2{
			csbi.srWindow.Right - csbi.srWindow.Left + 1,
			csbi.srWindow.Bottom - csbi.srWindow.Top + 1
		};
	}

	void enableScrolling(bool scrollingEnabled) {
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

		CONSOLE_SCREEN_BUFFER_INFO scrBufferInfo;
		GetConsoleScreenBufferInfo(hOut, &scrBufferInfo);

		// Console buffer size
		short
			scrBufferWidth = scrBufferInfo.dwSize.X,
			scrBufferHeight = scrBufferInfo.dwSize.Y;

		// Make the buffer size match the window size in characters
		// std::max() is used so the buffer is never less than 1 - That causes a crash
		COORD windowSizedBuffer = {
			windowSizedBuffer.X = std::max<int>(getConsoleSize().x, 1),
			windowSizedBuffer.Y = std::max<int>(getConsoleSize().y, 1)
		};

		COORD scrollSizedBuffer = {
			30,
			9001
		};

		// Set the new dimensions
		SetConsoleScreenBufferSize(hOut, scrollingEnabled ? scrollSizedBuffer : windowSizedBuffer);
	}

	bool isScrollingEnabled() {
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

		GetConsoleScreenBufferInfo(hOut, &consoleInfo);

		/*		Compares the console size to the buffer size		*/
		/*		They will be equal is scrolling is disabled			*/
		vec2 consoleSize = getConsoleSize();
		vec2 bufferSize = {
			consoleInfo.dwSize.X,
			consoleInfo.dwSize.Y
		};

		return !(bufferSize == consoleSize);
	}
};