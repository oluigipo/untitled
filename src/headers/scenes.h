#pragma once
#include "types.h"

#ifndef FIRST_SCENE

// NOTE(luigi): for release builds, FIRST_SCENE will always be 'scene_main'.
//              you can change this safely to run a specific scene.
#  define FIRST_SCENE scene_main

#endif

func uint scene_main(void);
func uint scene_menu(void);
func uint scene_game(void);

