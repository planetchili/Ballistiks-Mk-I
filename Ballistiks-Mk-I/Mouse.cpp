/****************************************************************************************** 
 *	Chili DirectX Framework Version 14.03.22											  *	
 *	Mouse.cpp																			  *
 *	Copyright 2014 PlanetChili <http://www.planetchili.net>								  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "Mouse.h"

MouseClient::MouseClient( const MouseServer& server )
: server( server )
{}
int MouseClient::GetMouseX() const
{
	return server.x;
}
int MouseClient::GetMouseY() const
{
	return server.y;
}
bool MouseClient::LeftIsPressed() const
{
	return server.leftIsPressed;
}
bool MouseClient::RightIsPressed() const
{
	return server.rightIsPressed;
}
bool MouseClient::IsInWindow() const
{
	return server.isInWindow;
}

MouseServer::MouseServer()
:	isInWindow( false ),
	leftIsPressed( false ),
	rightIsPressed( false ),
	x( -1 ),
	y( -1 )
{}
void MouseServer::OnMouseMove( int x,int y )
{
	this->x = x;
	this->y = y;
}
void MouseServer::OnMouseLeave()
{
	isInWindow = false;
}
void MouseServer::OnMouseEnter()
{
	isInWindow = true;
}
void MouseServer::OnLeftPressed()
{
	leftIsPressed = true;
}
void MouseServer::OnLeftReleased()
{
	leftIsPressed = false;
}
void MouseServer::OnRightPressed()
{
	rightIsPressed = true;
}
void MouseServer::OnRightReleased()
{
	rightIsPressed = false;
}
bool MouseServer::IsInWindow() const
{
	return isInWindow;
}