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
// $Source: /home/pablo/Desarrollo/sags-cvs/clienttext/src/Window.cpp,v $
// $Revision: 1.1 $
// $Date: 2005/03/21 15:33:27 $
//

#include <cstdarg>
#include "Log.hpp"
#include "Window.hpp"

// Inicialización de variables estáticas
int Window::scr_lines = 0;             // cambiará en Window::ScreenCheck
int Window::scr_columns = 0;           // cambiará en Window::ScreenCheck
int Window::scr_top_line = 0;
int Window::scr_top_col = 0;
int Window::scr_bottom_line = 0;       // cambiará en Window::ScreenCheck
int Window::scr_bottom_col = 0;
int Window::scr_available_lines = 0;   // cambiará en Window::ScreenCheck
int Window::scr_available_columns = 0; // cambiará en Window::ScreenCheck
bool Window::scr_has_color = false;    // cambiará en Window::ScreenInit
int Window::scr_color_count = 0;

//*********************************
//  Constructores y destructores
//*********************************

Window::Window (const char *label, int width, int height,
		NCURSES_COLOR_T fgcolor, NCURSES_COLOR_T bgcolor,
		int options)
{
	int xpos, ypos, color_options = 0;

	// guardamos el nombre
	strncpy (w_label, label, 15);

	// guardamos los colores
	w_fgcolor = fgcolor;
	w_bgcolor = bgcolor;

	// guardamos las dimensiones deseadas
	if (width == WINDOW_EXPAND)
		w_columns = scr_available_columns;
	else
		w_columns = width;

	if (height == WINDOW_EXPAND)
		w_lines = scr_available_lines;
	else
		w_lines = height;

	// calculamos la posición a ocupar
	if (options & WINDOW_PACK_ALL)
	{
		ypos = scr_top_line;
		w_lines = scr_available_lines;
		w_columns = scr_available_columns;
	}
	else if (options & WINDOW_PACK_BOTTOM)
	{
		ypos = scr_bottom_line;
		scr_bottom_line -= w_lines;
	}
	else
	{
		// WINDOW_PACK_TOP
		ypos = scr_top_line;
		scr_top_line += w_lines;
	}

	// calculamos el espacio que quedará disponible
	scr_available_lines -= w_lines;

	// FIXME: por ahora sólo pondremos las ventanas
	//        en la columna 0.
	xpos = 0;

	// guardamos la posición de la ventana
	w_top_line = ypos;
	w_top_col = xpos;

	// creamos la ventana Ncurses
	w_ncwin = subwin (stdscr, w_lines, w_columns, w_top_line, w_top_col);

	Print ("/%s: %dx%d (%d,%d) [%dx%d] +%d TL%d BL%d/\n", w_label, w_lines, w_columns,
	       w_top_line, w_top_col, scr_lines, scr_columns, scr_available_lines,
	       scr_top_line, scr_bottom_line);

	// fijamos los colores
	if (scr_has_color)
	{
		w_color_idx = ++scr_color_count;
		init_pair (w_color_idx, w_fgcolor, w_bgcolor);

		if (options & WINDOW_COLOR_BOLD)
			color_options |= A_BOLD;
		if (options & WINDOW_COLOR_REVERSE)
			color_options |= A_REVERSE;

		wbkgd (w_ncwin, ' ' | COLOR_PAIR (w_color_idx) | color_options);
		wattron (w_ncwin, COLOR_PAIR (w_color_idx) | color_options);
	}

	// refrescamos para que se vean los cambios
	wrefresh (w_ncwin);
}

Window::~Window ()
{
	delwin (w_ncwin);
}

//*********************************
//  Miembros públicos estáticos
//*********************************

void Window::ScreenInit (void)
{
	initscr ();
	crmode ();
	keypad (stdscr, TRUE);

	if (start_color () == ERR)
		scr_has_color = false;
	else
		scr_has_color = true;
}

void Window::ScreenCheck (void)
{
	getmaxyx (stdscr, scr_lines, scr_columns);
	scr_bottom_line = scr_lines - 1;
	scr_available_lines = scr_lines;
	scr_available_columns = scr_columns;
}

void Window::ScreenEnd (void)
{
	endwin ();
}

//*********************************
//  Miembros públicos
//*********************************

void Window::Print (const char* fmt, ...)
{
	char msg[LOG_MAX_STRING + 1];
	va_list ap;

	va_start (ap, fmt);
	vsnprintf (msg, LOG_MAX_STRING + 1, fmt, ap);
	va_end (ap);

	wprintw (w_ncwin, "%s", msg);
	wrefresh (w_ncwin);
}
