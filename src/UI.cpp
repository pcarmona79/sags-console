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
// $Source: /home/pablo/Desarrollo/sags-cvs/clienttext/src/UI.cpp,v $
// $Revision: 1.1 $
// $Date: 2005/03/21 15:33:27 $
//

#include "UI.hpp"

UserInterface::UserInterface ()
{
	// Iniciamos Ncurses
	Window::ScreenInit ();

	// Chequeamos las dimensiones de la pantalla
	Window::ScreenCheck ();

	// Creamos las ventanas a usar
	Menu = new Window ("Menu", WINDOW_EXPAND, 1, COLOR_GREEN, COLOR_BLUE,
			   WINDOW_PACK_TOP | WINDOW_RESIZEABLE_X);
	List = new Window ("List", WINDOW_EXPAND, 1, COLOR_YELLOW, COLOR_BLACK,
			   WINDOW_PACK_TOP | WINDOW_RESIZEABLE_X | WINDOW_COLOR_BOLD);
	Input = new Window ("Input", WINDOW_EXPAND, 1, COLOR_WHITE, COLOR_BLACK,
			   WINDOW_PACK_BOTTOM | WINDOW_RESIZEABLE_X);
	Info = new Window ("Info", WINDOW_EXPAND, 1, COLOR_YELLOW, COLOR_BLUE,
			   WINDOW_PACK_BOTTOM | WINDOW_RESIZEABLE_X);
	Output = new Window ("Output", WINDOW_EXPAND, WINDOW_EXPAND, COLOR_WHITE, COLOR_BLACK,
			     WINDOW_PACK_ALL | WINDOW_RESIZEABLE_X | WINDOW_RESIZEABLE_Y);
}

UserInterface::~UserInterface ()
{
	if (Menu != NULL)
		delete Menu;
	if (List != NULL)
		delete List;
	if (Output != NULL)
		delete Output;
	if (Info != NULL)
		delete Info;
	if (Input != NULL)
		delete Input;

	// terminamos Ncurses
	Window::ScreenEnd ();
}

// definimos el objeto
UserInterface UI;
