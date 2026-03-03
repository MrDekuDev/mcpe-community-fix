#include "KeyboardInput.h"
#include "../../Options.h"
#include "../../../world/entity/player/Player.h"
#include "../../../world/entity/player/Inventory.h"
#include "../../player/LocalPlayer.h"

KeyboardInput::KeyboardInput( Options* options )
{
	for (int i = 0; i < NumKeys; ++i)
		keys[i] = false;
	this->options = options;
}

void KeyboardInput::setKey( int key, bool state )
{
	int id = -1;
	if (key == options->keyUp.key) id = KEY_UP;
	if (key == options->keyDown.key) id = KEY_DOWN;
	if (key == options->keyLeft.key) id = KEY_LEFT;
	if (key == options->keyRight.key) id = KEY_RIGHT;
	if (key == options->keyJump.key) id = KEY_JUMP;
	if (key == options->keySneak.key) id = KEY_SNEAK;
	if (key == options->keyCraft.key) id = KEY_CRAFT;
	if (key == options->keyDrop.key) id = KEY_DROP;
	if (key == options->keyBuild.key) id = KEY_BUILD;
	//printf("key: %d\n", id);
	if (id >= 0) {
		keys[id] = state;
	}
}

void KeyboardInput::releaseAllKeys()
{
	xa = 0;
	ya = 0;

	for (int i = 0; i < NumKeys; i++) {
		keys[i] = false;
	}
	wantUp = wantDown = false;
}

void KeyboardInput::tick( Player* player )
{
	xa = 0;
	ya = 0;

	if (keys[KEY_UP]) ya++;
	if (keys[KEY_DOWN]) ya--;
	if (keys[KEY_LEFT]) xa++;
	if (keys[KEY_RIGHT]) xa--;
	jumping = keys[KEY_JUMP];
	sneaking = keys[KEY_SNEAK];
	if (sneaking) {
		xa *= 0.3f;
		ya *= 0.3f;
	}

	// Set wantUp and wantDown for flight control
	wantUp = jumping;
	wantDown = sneaking;

	if (keys[KEY_CRAFT])
		player->startCrafting((int)player->x, (int)player->y, (int)player->z, Recipe::SIZE_2X2);

	if (keys[KEY_DROP]) {
		ItemInstance* item = player->inventory->getSelected();
		if (item && !item->isNull()) {
			ItemInstance* dropItem = new ItemInstance(*item);
			dropItem->count = 1;
			item->count--;
			if (item->count <= 0) {
				player->inventory->clearSlot(player->inventory->selected);
			}
			player->drop(dropItem);
		}
	}

	if (keys[KEY_BUILD]) {
		// Open inventory/backpack
		// This will be handled in Minecraft's keyboard processing
	}

	//printf("\n>- %f %f\n", xa, ya);
}

