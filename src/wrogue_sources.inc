#
# Warp Rogue Source Files
#


#
# default to POSIX style path separator
#
SEP:=/


#
# library sources
#
SRC_LIB=\
  lib$(SEP)box.c \
  lib$(SEP)c_ext.c \
  lib$(SEP)cellcave.c \
  lib$(SEP)cellpnt.c \
  lib$(SEP)llist.c \
  lib$(SEP)path.c \
  lib$(SEP)rdb.c \
  lib$(SEP)tt.c \
  lib$(SEP)shake.c \
  lib$(SEP)wicca.c 


#
# main program sources
#
SRC_MAIN=\
  ai$(SEP)ai.c \
  ai$(SEP)ai_combat.c \
  ai$(SEP)ai_combat_options.c \
  ai$(SEP)ai_equipment.c \
  ai$(SEP)ai_movement.c \
  ai$(SEP)ai_psychic.c \
  ai$(SEP)ai_states.c \
  ai$(SEP)ai_tactics.c \
  ai$(SEP)ai_target.c \
  ai$(SEP)pathfinder.c \
  areas$(SEP)area.c \
  areas$(SEP)area_cave.c \
  areas$(SEP)area_dungeon.c \
  areas$(SEP)area_generation.c \
  areas$(SEP)area_sector.c \
  areas$(SEP)area_transition.c \
  characters$(SEP)character.c \
  characters$(SEP)character_actions.c \
  characters$(SEP)character_advancement.c \
  characters$(SEP)character_attributes.c \
  characters$(SEP)character_career.c \
  characters$(SEP)character_death.c \
  characters$(SEP)character_equipment.c \
  characters$(SEP)character_generation_manual.c \
  characters$(SEP)character_generation_random.c \
  characters$(SEP)character_inventory.c \
  characters$(SEP)character_movement.c \
  characters$(SEP)character_perception.c \
  characters$(SEP)character_perks.c \
  characters$(SEP)character_race.c \
  characters$(SEP)character_stats.c \
  characters$(SEP)character_talk.c \
  combat$(SEP)combat.c \
  combat$(SEP)combat_close.c \
  combat$(SEP)combat_ranged.c \
  data_files$(SEP)character_files.c \
  data_files$(SEP)data_files.c \
  data_files$(SEP)info_files.c \
  data_files$(SEP)object_files.c \
  data_files$(SEP)planet_files.c \
  data_files$(SEP)race_files.c \
  data_files$(SEP)rule_files.c \
  data_files$(SEP)terrain_files.c \
  data_files$(SEP)ui_files.c \
  debug$(SEP)debug.c \
  debug$(SEP)log.c \
  dice$(SEP)dice.c \
  effects$(SEP)effect.c \
  events$(SEP)event.c \
  factions$(SEP)faction.c \
  galaxy$(SEP)galaxy.c \
  game$(SEP)game.c \
  help$(SEP)help.c \
  honour$(SEP)honour.c \
  loadsave$(SEP)loadsave.c \
  messages$(SEP)message_dynamic.c \
  messages$(SEP)message_grammar.c \
  objects$(SEP)object.c \
  objects$(SEP)object_attributes.c \
  objects$(SEP)object_drugs.c \
  parties$(SEP)party.c \
  planets$(SEP)planet.c \
  program_manager$(SEP)program_manager.c \
  psychic$(SEP)psychic.c \
  psychic$(SEP)psychic_powers.c \
  quests$(SEP)quest.c \
  random_names$(SEP)random_names.c \
  randomiser$(SEP)randomiser.c \
  rng$(SEP)rng.c \
  scenario$(SEP)scenario.c \
  scripts$(SEP)script.c \
  scripts$(SEP)script_functions.c \
  shops$(SEP)shop.c \
  terrain$(SEP)terrain.c \
  terrain$(SEP)terrain_attributes.c \
  timing$(SEP)timing.c \
  ui$(SEP)command.c \
  ui$(SEP)macro.c \
  ui$(SEP)targeter.c \
  ui$(SEP)ui.c \
  ui$(SEP)ui_colour.c \
  ui$(SEP)ui_command.c \
  ui$(SEP)ui_command_bar.c \
  ui$(SEP)ui_game.c \
  ui$(SEP)ui_gen_adv.c \
  ui$(SEP)ui_info.c \
  ui$(SEP)ui_input.c \
  ui$(SEP)ui_keys.c \
  ui$(SEP)ui_menu.c \
  ui$(SEP)ui_message.c \
  ui$(SEP)ui_misc.c \
  ui$(SEP)ui_symbols.c \
  ui$(SEP)ui_vision.c \
  utilities$(SEP)utility.c

