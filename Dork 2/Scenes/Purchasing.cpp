//
//  Purchasing.cpp
//  Dork 2
//
//  Created by Alessandro Vinciguerra on 01/12/2017.
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

#include "Purchasing.h"

Purchasing::Purchasing() : Scene() {}

void Purchasing::activate() {
	orxObject_SetPosition(selectorArrow, &defaultPos);
	currentSelection = 0;
	statViewer->reloadData();
	loadItemData();
	Scene::activate();
}

SceneType Purchasing::update(const orxCLOCK_INFO* clockInfo) {
	orxBOOL hadText = Scene::currentlyHasText();
	SceneType type = Scene::update(clockInfo);
	if (type != getSceneType()) {
		return type;
	}
	if (paused || hadText) {
		return getSceneType();
	}
	orxVECTOR pos;
	orxObject_GetPosition(selectorArrow, &pos);
	int prevSelection = currentSelection;
	if (getKeyDown((orxSTRING)"GoDown") && currentSelection < selectionLimit) {
		pos.fY += 60;
		currentSelection++;
	} else if (getKeyDown((orxSTRING)"GoUp") && currentSelection > 0) {
		pos.fY -= 60;
		currentSelection--;
	} else if (getKeyDown((orxSTRING)"Enter")) {
		if (currentSelection == selectionLimit) {
			return EXPLORATION;
		}
		int result = makePurchase();
		if (result == PURCHASE_SUCCESSFUL) {
			orxObject_AddSound(selectorArrow, "Kaching");
		} else if (result == PURCHASE_FAILED) {
			orxObject_AddSound(selectorArrow, "ErrorSound");
		}
	}
	if (currentSelection != prevSelection) {
		orxObject_SetPosition(selectorArrow, &pos);
		orxObject_AddSound(selectorArrow, "SelectorSound");
	}
	if (currentSelection < selectionLimit) {
		loadItemData();
	}
	return getSceneType();
}
