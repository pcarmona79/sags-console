//
// SAGS - Secure Administrator of Game Servers
// Copyright (C) 2005 Pablo Carmona Amigo
// 
// This file is part of SAGS Text Client.
//
// SAGS Text Client is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// SAGS Text Client is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// $Source: /home/pablo/Desarrollo/sags-cvs/clienttext/src/Window.hpp,v $
// $Revision: 1.1 $
// $Date: 2005/03/21 15:33:27 $
//

#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include <curses.h>

enum
{
	WINDOW_EXPAND = -1,
	WINDOW_PACK_TOP = 0,
	WINDOW_PACK_BOTTOM = 1,
	WINDOW_PACK_ALL = 2,
	WINDOW_RESIZEABLE_X = 4,
	WINDOW_RESIZEABLE_Y = 8,
	WINDOW_COLOR_BOLD = 16,
	WINDOW_COLOR_REVERSE = 32
};

class Window
{
private:
	int w_lines, w_columns;
	int w_top_line, w_top_col;
	WINDOW *w_ncwin;
	NCURSES_COLOR_T w_fgcolor, w_bgcolor;
	char w_label[16];
	int w_color_idx;

	static int scr_lines;
	static int scr_columns;
	static int scr_top_line;
	static int scr_top_col;
	static int scr_bottom_line;
	static int scr_bottom_col;
	static int scr_available_lines;
	static int scr_available_columns;
	static bool scr_has_color;
	static int scr_color_count;

public:
	Window (const char *label, int width, int height,
		NCURSES_COLOR_T fgcolor, NCURSES_COLOR_T bgcolor,
		int options);
	~Window ();

	static void ScreenInit (void);
	static void ScreenCheck (void);
	static void ScreenEnd (void);

	void Print (const char* fmt, ...);
};

#endif // __WINDOW_HPP__
