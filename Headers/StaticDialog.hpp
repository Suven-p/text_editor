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

#ifndef STATIC_DIALOG_H
#define STATIC_DIALOG_H

//#include "resource.h"
#include "BaseWindow.hpp"

class StaticDialog : public Os::BaseWindow
{
public:
	StaticDialog() : BaseWindow() {};
	~StaticDialog() {};
	virtual void create(int dialogID);

	virtual bool isCreated() const {
		return static_cast<bool>(m_hwnd);
	};

	//virtual do

	void destroy() {
		::DestroyWindow(m_hwnd);
	};

protected:
	static BOOL CALLBACK dlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) = 0;
};

#endif //STATIC_DIALOG_H