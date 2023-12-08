﻿#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <fstream>

#include "amethyst/Log.h"
#include "amethyst/HookManager.h"
#include "MinHook.h"

#include "minecraft/src/common/world/item/Item.h"

HookManager hookManager;

Item::_appendFormattedHovertext _Item_appendFormattedHovertext;

static void Item_appendFormattedHovertext(Item* self, const ItemStackBase& itemStack, Level& level, std::string& text, uint8_t a5) {
    _Item_appendFormattedHovertext(self, itemStack, level, text, a5);
    uint64_t max = itemStack.mItem->getMaxDamage();

    if (max != 0) {
        uint64_t current = max - itemStack.mItem->getDamageValue(itemStack.mUserData);
        text.append(fmt::format("\nDurability: {} / {}", current, max));
    }

    auto itemNamespace = reinterpret_cast<std::string*>(reinterpret_cast<uint64_t>(self) + 248);

    std::string rawNameId;
    itemStack.getRawNameId(rawNameId);
    text.append(fmt::format("\n{}8{}:{}", "\xc2\xa7", *itemNamespace, rawNameId));

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}

extern "C" __declspec(dllexport) void Initialize() {
    MH_Initialize();

    hookManager.CreateHook(
        SigScan("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 49 8B F1 4C 89 44 24 ? 4C 8B F2 48 8B D9"),
        &Item_appendFormattedHovertext, reinterpret_cast<void**>(&_Item_appendFormattedHovertext)
    );

    Log::Info("Sizeof: {}\n", sizeof(std::string));
}

extern "C" __declspec(dllexport) void Shutdown() {
    hookManager.Shutdown();
}