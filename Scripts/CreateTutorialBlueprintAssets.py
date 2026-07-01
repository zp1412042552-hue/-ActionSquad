import unreal


ASSET_DIR = "/Game/Blueprints/Tutorial"
TUTORIAL_LEVEL = "/Game/Level/Tutorial"

PLACED_LABELS = {
    "AS_TutorialPawn_Player",
    "AS_TeamA_Draggable",
    "AS_TeamB_Draggable",
    "AS_TutorialInstructionScreen",
    "AS_TutorialGestureDisplay",
    "AS_TutorialDoor_Target",
}


def create_blueprint(asset_name: str, parent_class_path: str):
    unreal.EditorAssetLibrary.make_directory(ASSET_DIR)
    asset_path = f"{ASSET_DIR}/{asset_name}"

    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        unreal.log(f"{asset_path} already exists")
        return asset

    parent_class = unreal.load_class(None, parent_class_path)
    if not parent_class:
        raise RuntimeError(f"Could not load parent class: {parent_class_path}")

    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    asset = asset_tools.create_asset(asset_name, ASSET_DIR, unreal.Blueprint, factory)
    if not asset:
        raise RuntimeError(f"Failed to create {asset_path}")

    unreal.EditorAssetLibrary.save_loaded_asset(asset)
    unreal.log(f"Created {asset_path} from {parent_class_path}")
    return asset


create_blueprint("BP_TeamA", "/Script/ActionSquad.TutorialTeamAActor")
create_blueprint("BP_TeamB", "/Script/ActionSquad.TutorialTeamBActor")
create_blueprint("BP_TutorialPawn", "/Script/ActionSquad.TutorialPawn")
create_blueprint("BP_TutorialWeapon", "/Script/ActionSquad.TutorialWeaponActor")
create_blueprint("BP_TutorialBallisticEffect", "/Script/ActionSquad.TutorialBallisticEffectActor")
create_blueprint("BP_TutorialShellCasing", "/Script/ActionSquad.TutorialShellCasingActor")
create_blueprint("BP_TutorialInstructionScreen", "/Script/ActionSquad.TutorialInstructionActor")
create_blueprint("BP_TutorialGestureDisplay", "/Script/ActionSquad.TutorialGestureDisplayActor")
create_blueprint("BP_TutorialDoor", "/Script/ActionSquad.TutorialDoorActor")

unreal.EditorAssetLibrary.save_directory(ASSET_DIR)


def load_generated_class(asset_path: str):
    asset_name = asset_path.rsplit("/", 1)[-1]
    class_path = f"{asset_path}.{asset_name}_C"
    generated_class = unreal.load_class(None, class_path)
    if not generated_class:
        raise RuntimeError(f"Could not load generated class: {class_path}")
    return generated_class


def set_label(actor, label: str):
    actor.set_actor_label(label, mark_dirty=True)
    actor.set_folder_path("ActionSquad_TutorialRuntime")


def configure_tutorial_pawn_defaults():
    try:
        pawn_class = load_generated_class(f"{ASSET_DIR}/BP_TutorialPawn")
        weapon_class = load_generated_class(f"{ASSET_DIR}/BP_TutorialWeapon")
        ballistic_effect_class = load_generated_class(f"{ASSET_DIR}/BP_TutorialBallisticEffect")
        shell_casing_class = load_generated_class(f"{ASSET_DIR}/BP_TutorialShellCasing")
        pawn_cdo = unreal.get_default_object(pawn_class)
        pawn_cdo.set_editor_property("player_weapon_class", weapon_class)
        pawn_cdo.set_editor_property("muzzle_flash_effect_class", ballistic_effect_class)
        pawn_cdo.set_editor_property("bullet_tracer_effect_class", ballistic_effect_class)
        pawn_cdo.set_editor_property("impact_effect_class", ballistic_effect_class)
        pawn_cdo.set_editor_property("shell_casing_class", shell_casing_class)
        pawn_cdo.set_editor_property("enable_hand_touch_fire_input", True)
        pawn_cdo.set_editor_property("hand_touch_fire_distance", 2.5)
        pawn_cdo.set_editor_property("hand_touch_fire_release_distance", 5.0)
        pawn_cdo.set_editor_property("require_high_confidence_hands_for_fire", True)
        pawn_cdo.set_editor_property("gun_forward_start_pitch", 25.0)
        pawn_cdo.set_editor_property("gun_forward_stop_pitch", 14.0)
        pawn_cdo.set_editor_property("gun_backward_start_pitch", -50.0)
        pawn_cdo.set_editor_property("gun_backward_stop_pitch", -28.0)
        unreal.EditorAssetLibrary.save_asset(f"{ASSET_DIR}/BP_TutorialPawn")
        unreal.log("Configured BP_TutorialPawn player_weapon_class -> BP_TutorialWeapon")
    except Exception as exc:
        unreal.log_warning(f"Could not configure BP_TutorialPawn weapon class: {exc}")


def spawn_named(label: str, class_path: str, location, rotation=(0.0, 0.0, 0.0)):
    actor_class = load_generated_class(class_path)
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        actor_class,
        unreal.Vector(*location),
        unreal.Rotator(*rotation),
    )
    if not actor:
        raise RuntimeError(f"Could not spawn {label} from {class_path}")
    set_label(actor, label)
    return actor


def remove_previous_runtime_actors():
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        if actor.get_actor_label() in PLACED_LABELS:
            unreal.EditorLevelLibrary.destroy_actor(actor)


def place_tutorial_runtime_actors():
    unreal.EditorLevelLibrary.load_level(TUTORIAL_LEVEL)
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        raise RuntimeError("Could not get editor world")

    world_settings = world.get_world_settings()
    tutorial_game_mode = unreal.load_class(None, "/Script/ActionSquad.TutorialGameMode")
    if tutorial_game_mode:
        world_settings.set_editor_property("default_game_mode", tutorial_game_mode)

    remove_previous_runtime_actors()

    pawn = spawn_named(
        "AS_TutorialPawn_Player",
        f"{ASSET_DIR}/BP_TutorialPawn",
        (0.0, -360.0, 92.0),
        (0.0, 90.0, 0.0),
    )
    pawn.set_editor_property("auto_possess_player", unreal.AutoReceiveInput.PLAYER0)

    team_a = spawn_named(
        "AS_TeamA_Draggable",
        f"{ASSET_DIR}/BP_TeamA",
        (-150.0, 120.0, 0.0),
        (0.0, -90.0, 0.0),
    )

    team_b = spawn_named(
        "AS_TeamB_Draggable",
        f"{ASSET_DIR}/BP_TeamB",
        (150.0, 120.0, 0.0),
        (0.0, -90.0, 0.0),
    )

    instruction = spawn_named(
        "AS_TutorialInstructionScreen",
        f"{ASSET_DIR}/BP_TutorialInstructionScreen",
        (0.0, 255.0, 170.0),
        (0.0, -90.0, 0.0),
    )

    gesture_display = spawn_named(
        "AS_TutorialGestureDisplay",
        f"{ASSET_DIR}/BP_TutorialGestureDisplay",
        (0.0, 225.0, 115.0),
        (0.0, 90.0, 0.0),
    )
    instruction.set_editor_property("gesture_display_actor", gesture_display)

    spawn_named(
        "AS_TutorialDoor_Target",
        f"{ASSET_DIR}/BP_TutorialDoor",
        (335.0, 140.0, 0.0),
        (0.0, -90.0, 0.0),
    )

    unreal.EditorLevelLibrary.save_current_level()
    unreal.EditorAssetLibrary.save_directory("/Game/Level")
    unreal.log("Placed Action Squad tutorial runtime actors in /Game/Level/Tutorial")


if "-PlaceTutorialActors" in unreal.SystemLibrary.get_command_line():
    configure_tutorial_pawn_defaults()
    place_tutorial_runtime_actors()
    unreal.log("Tutorial blueprint assets and level placements are ready.")
else:
    configure_tutorial_pawn_defaults()
    unreal.log("Tutorial blueprint assets are ready. Level placement was skipped.")
