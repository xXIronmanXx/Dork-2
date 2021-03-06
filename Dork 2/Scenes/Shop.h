//
//  Shop.h
//  Dork 2
//
//  Created by Alessandro Vinciguerra on 24/11/2017.
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

#ifndef Shop_h
#define Shop_h

#include "Purchasing.h"
#include "Potion.h"

class Shop : public Purchasing {
	int quantity = 1;
	std::vector<orxOBJECT*> potionCounts;

	orxOBJECT* potionName;
	orxOBJECT* potionPrice;
	orxOBJECT* potionEffect;
protected:
	virtual int makePurchase();
	virtual void loadItemData();
public:
	Shop(Player*);

	virtual void loadPlayerData(Player*);
	virtual SceneType getSceneType();
	virtual SceneType update(const orxCLOCK_INFO*);
};

#endif
