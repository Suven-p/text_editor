//this file is part of notepad++
//Copyright (C)2003 Don HO ( donho@altern.org )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef USER_DEFINE_H
#define USER_DEFINE_H

#include <string>
#include <shlwapi.h>
#include "StaticDialog.hpp"
#include "UserDefineResource.h"
//#include "EditControl.hpp"

class EditControl;

#define WL_LEN_MAX 1024

#define BOLD_MASK     1
#define ITALIC_MASK   2

const int nbWordList = 4;
const int nbBlockColor = 5;
const int nbBoolean = 5;

const bool DOCK = true;
const bool UNDOCK = false;

struct WordList
{
	WordList() : _color(0x0000), _fontStyle(0) { _words[0] = '\0'; };

	char _words[WL_LEN_MAX];
	int _color;
	unsigned char _fontStyle;

	bool isBold() const { return static_cast<bool>(_fontStyle & BOLD_MASK); };
	bool isItalic() const { return static_cast<bool>(_fontStyle & ITALIC_MASK); };
};

class UserDefineDialog : public StaticDialog
{
public:
	UserDefineDialog();

	void init(HINSTANCE hInst, HWND hPere, EditControl* pSev) {
		if (!_pScintilla)
		{
			BaseWindow::init(hInst, hPere);
			_pScintilla = pSev;
		}
	};

	void setScintilla(EditControl* pScinView) {
		_pScintilla = pScinView;
	};

	virtual void create(int dialogID);

	bool isCreated() const {
		return m_hwnd;
	};
	int getWidth() const {
		return _dlgPos.right;
	};

	int getHeight() const {
		return _dlgPos.bottom;
	};
	void doDialog(bool willBeShown = true) {
		if (!isCreated())
			create(IDD_USER_DEFINE_BOX);
		display(willBeShown);
	};

	const WordList& getWordList(int index) const {
		return _wordLists[index];
	};

	const char* getBoolList() const {
		return _boolList;
	};

	int getBlockColor(int index) const {
		return _blockColors[index];
	};

	inline void changeStyle();

	int getNbWordList() const { return nbWordList; };
	int getNbBlock() const { return nbBlockColor; };

	bool isDocked() const { return _status == DOCK; };

protected:
	virtual BOOL CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	HWND _itemHandlesArray[18];
	EditControl* _pScintilla;
	WordList _wordLists[nbWordList];
	char _boolList[nbBoolean + 1];
	int _blockColors[nbBlockColor];
	bool _status;
	RECT _dlgPos;

	void updateUserDefineData();
	void getActualPosSize() {
		::GetWindowRect(m_hwnd, &_dlgPos);
		_dlgPos.right -= _dlgPos.left;
		_dlgPos.bottom -= _dlgPos.top;
	};
	void restorePosSize() { resize_to(_dlgPos); };
};

#endif USER_DEFINE_H