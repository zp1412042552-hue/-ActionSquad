import csv
import math
import random
import wave
from pathlib import Path


SAMPLE_RATE = 44100
OUT_DIR = Path(__file__).resolve().parents[1] / "Content" / "Audio" / "Tutorial"


def clamp(value: float) -> float:
    return max(-1.0, min(1.0, value))


def write_wav(path: Path, samples: list[float]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    peak = max((abs(s) for s in samples), default=1.0)
    scale = 0.92 / peak if peak > 0.92 else 1.0
    with wave.open(str(path), "wb") as f:
        f.setnchannels(1)
        f.setsampwidth(2)
        f.setframerate(SAMPLE_RATE)
        frames = bytearray()
        for sample in samples:
            value = int(clamp(sample * scale) * 32767)
            frames += value.to_bytes(2, byteorder="little", signed=True)
        f.writeframes(frames)


def env(t: float, attack: float, decay: float) -> float:
    if t < attack:
        return t / max(attack, 0.0001)
    return math.exp(-(t - attack) / max(decay, 0.0001))


def noise() -> float:
    return random.random() * 2.0 - 1.0


def synth(duration: float, fn) -> list[float]:
    total = int(duration * SAMPLE_RATE)
    return [fn(i / SAMPLE_RATE, i) for i in range(total)]


def sine(freq: float, t: float) -> float:
    return math.sin(2.0 * math.pi * freq * t)


def square(freq: float, t: float) -> float:
    return 1.0 if sine(freq, t) >= 0.0 else -1.0


def weapon_fire(seed: int, suppressed: bool = False) -> list[float]:
    random.seed(seed)
    duration = 0.34 if suppressed else 0.42

    def fn(t: float, _: int) -> float:
        snap = noise() * math.exp(-t / 0.018)
        body = noise() * math.exp(-t / (0.08 if suppressed else 0.12))
        low = sine(82.0, t) * math.exp(-t / 0.09)
        mech = square(1800.0, t) * math.exp(-t / 0.012)
        return 0.95 * snap + 0.42 * body + 0.22 * low + 0.11 * mech

    return synth(duration, fn)


def shell(seed: int) -> list[float]:
    random.seed(seed)
    duration = 0.52

    def fn(t: float, _: int) -> float:
        hit1 = env(t, 0.001, 0.035) * (sine(2600, t) + 0.35 * sine(3850, t))
        hit2 = env(max(0.0, t - 0.19), 0.001, 0.055) * (sine(1700, t) + 0.5 * noise())
        ring = sine(7200, t) * math.exp(-t / 0.25) * 0.14
        return 0.45 * hit1 + 0.35 * hit2 + ring

    return synth(duration, fn)


def impact(seed: int, character: bool = False) -> list[float]:
    random.seed(seed)
    duration = 0.24 if character else 0.18

    def fn(t: float, _: int) -> float:
        crack = noise() * math.exp(-t / 0.018)
        thud = sine(110 if character else 190, t) * math.exp(-t / 0.06)
        grit = noise() * math.exp(-t / 0.07) * (0.35 if character else 0.55)
        return 0.5 * crack + 0.45 * thud + 0.32 * grit

    return synth(duration, fn)


def ui_tone(seed: int, base: float, up: bool = True, duration: float = 0.22) -> list[float]:
    random.seed(seed)

    def fn(t: float, _: int) -> float:
        glide = base + (base * (0.45 if up else -0.28)) * min(1.0, t / duration)
        tone = sine(glide, t) + 0.35 * sine(glide * 2.0, t)
        return tone * env(t, 0.012, duration * 0.32)

    return synth(duration, fn)


def marker_arrive(seed: int, base: float) -> list[float]:
    random.seed(seed)
    duration = 0.36

    def fn(t: float, _: int) -> float:
        first = sine(base, t) * env(t, 0.005, 0.06)
        second_t = max(0.0, t - 0.13)
        second = sine(base * 1.5, t) * env(second_t, 0.004, 0.09)
        return 0.6 * first + 0.5 * second

    return synth(duration, fn)


def door_breach(seed: int) -> list[float]:
    random.seed(seed)
    duration = 0.92

    def fn(t: float, _: int) -> float:
        kick = (noise() * 0.8 + sine(78, t) * 0.7) * math.exp(-t / 0.075)
        metal_t = max(0.0, t - 0.16)
        metal = (noise() * 0.45 + sine(260, t) * 0.35 + sine(520, t) * 0.16) * math.exp(-metal_t / 0.24)
        fall_t = max(0.0, t - 0.48)
        fall = (noise() * 0.35 + sine(95, t) * 0.45) * math.exp(-fall_t / 0.22)
        return kick + (metal if t >= 0.16 else 0.0) + (fall if t >= 0.48 else 0.0)

    return synth(duration, fn)


def movement(seed: int, start: bool) -> list[float]:
    random.seed(seed)
    duration = 0.28

    def fn(t: float, _: int) -> float:
        base = 220.0 if start else 180.0
        sweep = base + (120.0 if start else -65.0) * min(1.0, t / duration)
        return (sine(sweep, t) + 0.18 * noise()) * env(t, 0.01, 0.08)

    return synth(duration, fn)


def servo(seed: int, up: bool = True) -> list[float]:
    random.seed(seed)
    duration = 0.3

    def fn(t: float, _: int) -> float:
        sweep = 310.0 + (260.0 if up else -130.0) * min(1.0, t / duration)
        motor = sine(sweep, t) * 0.42 + sine(sweep * 2.04, t) * 0.12
        tick = square(36.0, t) * 0.08 * math.exp(-t / 0.22)
        return (motor + tick) * env(t, 0.01, 0.11)

    return synth(duration, fn)


def click(seed: int, heavy: bool = False) -> list[float]:
    random.seed(seed)
    duration = 0.16 if not heavy else 0.22

    def fn(t: float, _: int) -> float:
        snap = noise() * math.exp(-t / (0.009 if not heavy else 0.018))
        body = sine(900 if not heavy else 480, t) * math.exp(-t / 0.04)
        return 0.42 * snap + 0.34 * body

    return synth(duration, fn)


def sweep_alert(seed: int, urgent: bool = False) -> list[float]:
    random.seed(seed)
    duration = 0.42 if urgent else 0.34

    def fn(t: float, _: int) -> float:
        base = 420.0 if urgent else 300.0
        mod = 1.0 + 0.08 * sine(18.0, t)
        tone = sine((base + 380.0 * t) * mod, t) + 0.22 * sine((base * 2.0 + 160.0 * t), t)
        return tone * env(t, 0.008, 0.14)

    return synth(duration, fn)


def whiz(seed: int) -> list[float]:
    random.seed(seed)
    duration = 0.22

    def fn(t: float, _: int) -> float:
        sweep = 3600.0 - 1900.0 * min(1.0, t / duration)
        hiss = noise() * 0.2 * math.exp(-t / 0.07)
        return (sine(sweep, t) * 0.32 + hiss) * env(t, 0.003, 0.06)

    return synth(duration, fn)


def ambient_pulse(seed: int) -> list[float]:
    random.seed(seed)
    duration = 4.0

    def fn(t: float, _: int) -> float:
        hum = sine(54.0, t) * 0.07 + sine(108.0, t) * 0.035
        air = noise() * 0.018
        pulse = sine(0.5, t) * sine(330.0, t) * 0.018
        return hum + air + pulse

    return synth(duration, fn)


def main() -> int:
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    assets = [
        ("AS_SFX_Weapon_Fire", "sharp tactical weapon shot", weapon_fire(1001, False)),
        ("AS_SFX_Weapon_Fire_Soft", "short soft close weapon shot", weapon_fire(1002, True)),
        ("AS_SFX_Shell_Eject", "brass shell casing eject and land", shell(1003)),
        ("AS_SFX_Bullet_Surface", "bullet impact on concrete or metal", impact(1004, False)),
        ("AS_SFX_Bullet_Character", "stylized orange character impact", impact(1005, True)),
        ("AS_SFX_Command_Select_A", "select teammate A bright cue", ui_tone(1006, 520, True, 0.2)),
        ("AS_SFX_Command_Select_B", "select teammate B warm cue", ui_tone(1007, 390, True, 0.2)),
        ("AS_SFX_Command_Move_Issue", "movement command accepted", ui_tone(1008, 460, True, 0.24)),
        ("AS_SFX_Command_Invalid", "invalid command rejected", ui_tone(1009, 260, False, 0.25)),
        ("AS_SFX_Marker_Arrive_A", "teammate A arrived marker", marker_arrive(1010, 620)),
        ("AS_SFX_Marker_Arrive_B", "teammate B arrived marker", marker_arrive(1011, 470)),
        ("AS_SFX_Step_Advance", "tutorial step completed", ui_tone(1012, 660, True, 0.28)),
        ("AS_SFX_Tutorial_Complete", "tutorial complete confirmation", marker_arrive(1013, 740)),
        ("AS_SFX_Door_Breach_Kick", "door breach kick and metal fall", door_breach(1014)),
        ("AS_SFX_Locomotion_Start", "player locomotion start", movement(1015, True)),
        ("AS_SFX_Locomotion_Stop", "player locomotion stop", movement(1016, False)),
        ("AS_SFX_UI_Open", "tutorial panel appears", ui_tone(1017, 330, True, 0.24)),
        ("AS_SFX_UI_Confirm", "small UI confirmation", ui_tone(1018, 610, True, 0.18)),
        ("AS_SFX_Team_Selected", "teammate selected shared cue", ui_tone(1019, 540, True, 0.18)),
        ("AS_SFX_Team_Move_Start", "teammate starts moving", servo(1021, True)),
        ("AS_SFX_Team_Move_Stop", "teammate stops at destination", servo(1022, False)),
        ("AS_SFX_Door_Targeted", "door command target locked", sweep_alert(1023, True)),
        ("AS_SFX_Door_Breach_Ready", "teammate in breach position", click(1024, True)),
        ("AS_SFX_Hand_Touch_Armed", "hands close enough to arm fire", click(1025, False)),
        ("AS_SFX_Hand_Touch_Reset", "hands separated and fire rearmed", ui_tone(1026, 300, False, 0.16)),
        ("AS_SFX_Bullet_Tracer_Whiz", "fast bullet tracer pass", whiz(1027)),
        ("AS_SFX_Objective_Appear", "new objective appears", sweep_alert(1028, False)),
        ("AS_SFX_Objective_Success", "objective success", marker_arrive(1029, 680)),
        ("AS_SFX_Completion_Zone_Enter", "player enters completion zone", ui_tone(1030, 820, True, 0.32)),
        ("AS_SFX_Weapon_Dry_Click", "weapon dry mechanical click", click(1031, True)),
        ("AS_SFX_Command_Rearm", "same teammate command rearmed", ui_tone(1032, 480, True, 0.18)),
        ("AS_Tutorial_Ambient_Loop", "subtle tactical tutorial room ambience", ambient_pulse(1020)),
    ]

    manifest_path = OUT_DIR / "TutorialSfxManifest.csv"
    with manifest_path.open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["asset_name", "description", "relative_path"])
        for name, description, samples in assets:
            path = OUT_DIR / f"{name}.wav"
            write_wav(path, samples)
            writer.writerow([name, description, path.relative_to(OUT_DIR.parent.parent).as_posix()])
            print(f"generated {path}")

    print(f"manifest {manifest_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
