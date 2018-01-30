//
//  Combat.cpp
//  Dork 2
//
//  Created by Alessandro Vinciguerra on 24/11/2017.
//      <alesvinciguerra@gmail.com>
//Copyright (C) 2017 Arc676/Alessandro Vinciguerra

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

#include "Combat.h"

Move Combat::moves[2][2] = {
	{ATTACK, SPECIAL_MOVE},
	{USE_ITEM, RUN}
};

Combat::Combat(Player* player, Enemy* enemy) : Scene(), enemy(enemy) {
	loadPlayerData(player);
	selector = orxObject_CreateFromConfig("Selector");
	orxVECTOR pos = {-1400, 596, 0};
	orxObject_SetPosition(selector, &pos);

	playerStats = new StatViewer(player, {-1500, 270, 0});
	enemyStats = new StatViewer(enemy, {-1000, 150, 0});
	music = orxSound_CreateFromConfig("FightMusic");

	potionName = orxObject_CreateFromConfig("SV");
	pos = {-1000, 470, 0};
	orxObject_SetPosition(potionName, &pos);
	orxObject_Enable(potionName, orxFALSE);

	potionEffect = orxObject_CreateFromConfig("SV");
	pos.fY += 20;
	orxObject_SetPosition(potionEffect, &pos);
	orxObject_Enable(potionEffect, orxFALSE);

	pos = {-1060, 480, 0};
	allPotions = std::vector<orxOBJECT*>(POTIONCOUNT);
	for (int i = 0; i < POTIONCOUNT; i++) {
		orxOBJECT* potion = orxObject_CreateFromConfig(Potion::configCodeForType((PotionType)i));
		orxObject_SetPosition(potion, &pos);
		orxObject_Enable(potion, orxFALSE);
		allPotions[i] = potion;
	}

	orxObject_CreateFromConfig("CombatUI");
	setPauseMenuPosition({-1150.0, 400.0, 0});
}

void Combat::activate() {
	playerPos = player->getPosition();
	player->setPosition({-1200, 450, 0});
	enemy->setPosition({-1100, 200, 0});

	orxObject_SetTargetAnim(player->getEntity(), "IdleUAnim");
	orxCHAR anim[30];
	orxString_Print(anim, "IdleDAnim%s", enemy->getName());
	orxObject_SetTargetAnim(enemy->getEntity(), anim);

	hasPotions = playerHasPotions();

	Scene::activate();
}

bool Combat::playerHasPotions() {
	for (int i = 0; i < POTIONCOUNT; i++) {
		if (player->amountOfPotionOwned((PotionType)i) > 0) {
			return true;
		}
	}
	return false;
}

void Combat::deactivate() {
	orxObject_SetLifeTime(selector, 0);
	enemy->despawn();
	playerStats->destroy();
	enemyStats->destroy();
	player->setPosition(playerPos);
	Scene::deactivate();
}

SceneType Combat::makeMove(Move move) {
	switch (move) {
		case RUN:
			return EXPLORATION;
		case ATTACK:
		{
			player->alterSpeed(modifiers[0]);
			player->alterStrength(modifiers[1]);
			player->alterDefense(modifiers[2]);

			Entity* firstAttacker = player;
			Entity* secondAttacker = enemy;
			if (enemy->getSpeed() > player->getSpeed()) {
				firstAttacker = enemy;
				secondAttacker = player;
			}

			Entity::entityAttack(firstAttacker, secondAttacker);
			if (secondAttacker->getHP() > 0){
				Entity::entityAttack(secondAttacker, firstAttacker);
			}

			player->alterSpeed(-modifiers[0]);
			player->alterStrength(-modifiers[1]);
			player->alterDefense(-modifiers[2]);
			if (specialMoveCooldown > 0) {
				specialMoveCooldown--;
			}
		}
			break;
		case SPECIAL_MOVE:
			if (specialMoveCooldown > 0) {
				orxObject_AddSound(selector, "ErrorSound");
				return COMBAT;
			}
			switch (player->getType()) {
				case MAGIC:
				{
					int dHP = ceil((player->getLevel() + 5) / 10);
					int ddef = ceil((player->getLevel() + 5) / 50);
					player->alterDefense(-ddef);
					Entity::entityAttack(enemy, player);
					player->alterDefense(ddef);
					player->alterHP(dHP);
					specialMoveCooldown = orxMAX(10, player->getLevel() * 0.06);
				}
					break;
				case SPEED:
				{
					int dstr = ceil((player->getLevel() + 5) / 50);
					int ddef = ceil((player->getLevel() + 5) / 60);
					Entity::entityAttack(enemy, player);
					modifiers[1] += dstr;
					modifiers[2] -= ddef;
					specialMoveCooldown = orxMAX(8, player->getLevel() * 0.03);
				}
					break;
				case MELEE:
				{
					int ddef = ceil((player->getLevel() + 5) / 50);
					int dv = ceil((player->getLevel() + 5) / 180);
					Entity::entityAttack(enemy, player);
					modifiers[0] -= dv;
					modifiers[2] += ddef;
					specialMoveCooldown = orxMAX(7, player->getLevel() * 0.02);
				}
					break;
				case RANGE:
				{
					int dstr = ceil((player->getLevel() + 5) / 40);
					int dv = ceil((player->getLevel() + 5) / 150);
					Entity::entityAttack(enemy, player);
					modifiers[0] -= dv;
					modifiers[1] += dstr;
					specialMoveCooldown = orxMAX(8, player->getLevel() * 0.03);
				}
					break;
				default:
					break;
			}
			break;
		case USE_ITEM:
			if (hasPotions) {
				isSelectingPotion = true;

				orxObject_Enable(potionName, orxTRUE);
				orxObject_Enable(potionEffect, orxTRUE);
				orxObject_Enable(allPotions[selectedPotion], orxTRUE);
				if (player->amountOfPotionOwned(selectedPotion) == 0) {
					selectPotion(1);
				}
				updatePotionDescription();
			} else {
				orxObject_AddSound(selector, "ErrorSound");
			}
			return COMBAT;
			break;
	}
	if (enemy->getHP() <= 0) {
		player->defeat(enemy);
		return EXPLORATION;
	} else if (player->getHP() <= 0) {
		return MAIN_MENU;
	}
	playerStats->reloadData();
	enemyStats->reloadData();
	return COMBAT;
}

void Combat::consumePotions() {
	Potion* p = Potion::getCopyOf(selectedPotion);
	switch (selectedPotion) {
		case SPEEDBOOST:
			modifiers[0] += player->getSpeed() * orxMath_Pow(p->getAmount(), desiredQuantity);
			break;
		case STRBOOST:
			modifiers[1] += player->getStrength() * orxMath_Pow(p->getAmount(), desiredQuantity);
			break;
		case DEFBOOST:
			modifiers[2] += player->getDefense() * orxMath_Pow(p->getAmount(), desiredQuantity);
		case QUICKHEAL_2:
		case QUICKHEAL_5:
		case QUICKHEAL_10:
		case QUICKHEAL_20:
		case QUICKHEAL_50:
			player->alterHP(p->getAmount() * desiredQuantity);
			break;
		default:
			break;
	}
	player->changePotionAmount(selectedPotion, -desiredQuantity);
	playerStats->reloadData();
}

SceneType Combat::update(const orxCLOCK_INFO* clockInfo) {
	if (isSelectingPotion) {
		bool switchPotion = false;
		int direction = 1;
		if (getKeyDown((orxSTRING)"Pause")) {
			isSelectingPotion = false;
		} else if (getKeyDown((orxSTRING)"Enter")) {
			consumePotions();
			hasPotions = playerHasPotions();
			isSelectingPotion = false;
			desiredQuantity = 1;
		} else if (getKeyDown((orxSTRING)"GoDown") &&
				   desiredQuantity > 1) {
			desiredQuantity--;
			updatePotionDescription();
		} else if (getKeyDown((orxSTRING)"GoUp") &&
				   desiredQuantity < player->amountOfPotionOwned(selectedPotion)) {
			desiredQuantity++;
			updatePotionDescription();
		} else if (getKeyDown((orxSTRING)"GoLeft")) {
			switchPotion = true;
			direction = -1;
		} else if (getKeyDown((orxSTRING)"GoRight")) {
			switchPotion = true;
		}
		if (switchPotion) {
			desiredQuantity = 1;
			selectPotion(direction);
			updatePotionDescription();
		}
		if (!isSelectingPotion) {
			orxObject_Enable(potionName, orxFALSE);
			orxObject_Enable(potionEffect, orxFALSE);
			orxObject_Enable(allPotions[selectedPotion], orxFALSE);
		}
	} else {
		orxBOOL hadText = hasText;
		SceneType type = Scene::update(clockInfo);
		if (type != COMBAT) {
			return type;
		}
		if (paused || hadText) {
			return COMBAT;
		}
		orxVECTOR pos;
		orxBOOL selChanged = orxTRUE;
		orxObject_GetPosition(selector, &pos);
		if (getKeyDown((orxSTRING)"GoDown") && y < 1) {
			pos.fY += 60;
			y++;
		} else if (getKeyDown((orxSTRING)"GoUp") && y > 0) {
			pos.fY -= 60;
			y--;
		} else if (getKeyDown((orxSTRING)"GoLeft") && x > 0) {
			pos.fX -= 300;
			x--;
		} else if (getKeyDown((orxSTRING)"GoRight") && x < 1) {
			pos.fX += 300;
			x++;
		} else {
			if (getKeyDown((orxSTRING)"Enter")) {
				return makeMove(moves[y][x]);
			}
			selChanged = orxFALSE;
		}
		if (selChanged) {
			orxObject_SetPosition(selector, &pos);
		}
	}
	return COMBAT;
}

void Combat::selectPotion(int direction) {
	orxObject_Enable(allPotions[selectedPotion], orxFALSE);
	do {
		int newval = ((int)selectedPotion + (POTIONCOUNT + direction)) % POTIONCOUNT;
		selectedPotion = (PotionType)newval;
	} while (player->amountOfPotionOwned(selectedPotion) == 0);
	orxObject_Enable(allPotions[selectedPotion], orxTRUE);
}

void Combat::updatePotionDescription() {
	Potion* p = Potion::allPotions[selectedPotion];

	orxCHAR text[30];
	orxString_Print(text, "Potion: %s", p->getName());
	orxObject_SetTextString(potionName, text);

	switch (p->getType()) {
		case QUICKHEAL_2:
		case QUICKHEAL_5:
		case QUICKHEAL_10:
		case QUICKHEAL_20:
		case QUICKHEAL_50:
			orxString_Print(text, "Heals: %d (%dx) -> %d",
							(int)p->getAmount(), desiredQuantity, (int)(p->getAmount() * desiredQuantity));
			break;
		case STRBOOST:
		case SPEEDBOOST:
		case DEFBOOST:
			orxString_Print(text, "Effect: +%d%% (%dx) -> +%d%%",
							(int)(p->getAmount() * 100), desiredQuantity,
							(int)(pow(1 + p->getAmount(), desiredQuantity) * 100 - 100));
			break;
		default:
			break;
	}
	orxObject_SetTextString(potionEffect, text);
}

SceneType Combat::getSceneType() {
	return COMBAT;
}
