//
//  MainMenu.cpp
//  Dork 2
//
//  Created by Alessandro Vinciguerra on 03/12/2017.
//      <alesvinciguerra@gmail.com>
//Copyright (C) 2017-8 Arc676/Alessandro Vinciguerra

//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation (version 3)

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
//See README and LICENSE for more details

#include "MainMenu.h"

void MainMenu::activate() {
	//reset character data
	memset(name, 0, NAMELENGTH);
	cursorPos = 0;
	orxKeyboard_ClearBuffer();

	orxVECTOR pos = {-1216, 1592, 0};
	orxObject_SetTextString(nameField, "Name:");
	orxObject_SetPosition(nameField, &pos);

	pos = {-1216, 1652, 0};
	orxObject_SetTextString(typeField, Entity::typeToString(chosenType));
	orxObject_SetPosition(typeField, &pos);

	//reset selector
	pos = {-1100, 1600, 0};
	orxObject_SetPosition(selector, &pos);
	currentSelection = 0;

	Scene::activate();
}

MainMenu::MainMenu() {
	nameField = orxObject_CreateFromConfig("SV");
	typeField = orxObject_CreateFromConfig("SV");
	selector = orxObject_CreateFromConfig("MMSelector");
	lrArrows = orxObject_CreateFromConfig("MMArrows");
	orxObject_CreateFromConfig("MMObjects");
}

SceneType MainMenu::getSceneType() {
	return MAIN_MENU;
}

SceneType MainMenu::update(const orxCLOCK_INFO* clockInfo) {
	if (currentSelection == 0) {
		orxKEYBOARD_KEY keyPressed = orxKeyboard_ReadKey();
		if (keyPressed != orxKEYBOARD_KEY_NONE) {
			if (keyPressed == orxKEYBOARD_KEY_BACKSPACE) {
				if (cursorPos > 0) {
					name[--cursorPos] = 0;
					orxObject_SetTextString(nameField, name);
				}
			} else {
				if (cursorPos < NAMELENGTH) {
					const orxSTRING character = orxKeyboard_ReadString();
					orxU32 c = orxString_GetFirstCharacterCodePoint(character, orxNULL);
					if (orxString_IsCharacterAlphaNumeric(c)) {
						name[cursorPos++] = c;
						orxObject_SetTextString(nameField, name);
					}
				}
			}
			orxKeyboard_ClearBuffer();
			if (keyPressed < orxKEYBOARD_KEY_UP || keyPressed > orxKEYBOARD_KEY_LEFT) {
				return MAIN_MENU;
			}
		}
	}
	orxVECTOR pos;
	orxObject_GetPosition(selector, &pos);
	int prevSelection = currentSelection;
	EntityType prevType = chosenType;
	if (getKeyDown((orxSTRING)"GoDown") && currentSelection < 3) {
		currentSelection++;
		pos.fY += 60;
	} else if (getKeyDown((orxSTRING)"GoUp") && currentSelection > 0) {
		currentSelection--;
		pos.fY -= 60;
	} else if (getKeyDown((orxSTRING)"GoLeft") && chosenType > 0 && currentSelection == 1) {
		chosenType = (EntityType)(chosenType - 1);
	} else if (getKeyDown((orxSTRING)"GoRight") && chosenType < NOTYPE - 1 && currentSelection == 1) {
		chosenType = (EntityType)(chosenType + 1);
	} else if (getKeyDown((orxSTRING)"Enter")) {
		if (cursorPos > 0 && currentSelection >= 2) {
			player = new Player(name, chosenType);
			if (currentSelection == 3) {
				if (player->read(name) != orxSTATUS_SUCCESS) {
					orxObject_AddSound(selector, "ErrorSound");
					return MAIN_MENU;
				}
			}
			return EXPLORATION;
		} else {
			orxObject_AddSound(selector, "ErrorSound");
		}
	}
	if (currentSelection != prevSelection) {
		orxObject_SetPosition(selector, &pos);
		orxObject_AddSound(selector, "SelectorSound");
	} else if (chosenType != prevType) {
		orxObject_AddSound(selector, "TickSound");
		orxObject_SetTextString(typeField, Entity::typeToString(chosenType));
	}
	orxObject_Enable(lrArrows, currentSelection == 1);
	return MAIN_MENU;
}
