import os
import unreal


DESTINATION_PATH = "/Game/Audio/Tutorial"
SOURCE_DIR = os.path.join(unreal.Paths.project_content_dir(), "Audio", "Tutorial")


def import_wav(path: str) -> None:
    task = unreal.AssetImportTask()
    task.set_editor_property("filename", path)
    task.set_editor_property("destination_path", DESTINATION_PATH)
    task.set_editor_property("automated", True)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("save", True)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])


def main() -> None:
    if not os.path.isdir(SOURCE_DIR):
        unreal.log_warning(f"Tutorial SFX source directory not found: {SOURCE_DIR}")
        return

    wavs = [
        os.path.join(SOURCE_DIR, name)
        for name in sorted(os.listdir(SOURCE_DIR))
        if name.lower().endswith(".wav")
    ]
    for wav in wavs:
        import_wav(wav)
        unreal.log(f"Imported tutorial SFX: {wav}")

    unreal.EditorAssetLibrary.save_directory(DESTINATION_PATH)
    unreal.log(f"Imported {len(wavs)} tutorial SFX files into {DESTINATION_PATH}")


main()
