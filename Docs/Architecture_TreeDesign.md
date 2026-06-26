# Blue-Hand-Command — 树形架构设计

> 架构完全参照 Tiny Trouble VR：**SaveGame → GameInstance → GameModes**
> 所有关卡切换统一走 `RequestLevelTransition()`，GameInstance 是树干。

---

## 一、架构总览

```
━━━ 树根 (Tree Roots) — 数据源 ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  SaveGame (持久化数据)
  ├── bTutorialCompleted     是否完成教学
  ├── bAssaultUnlocked       突击关卡是否解锁
  ├── TotalMissionsPlayed    总游玩次数
  ├── BestRank              最高评价
  ├── MasterVolume          音量设置
  ├── SFXVolume             音效设置
  ├── GestureProfilePath    手势模板路径
  └── UnlockedMissions      已解锁关卡列表

  Home 关卡 (关卡选择)
  └── 生成 FTinyTroubleLevelRequest → 告诉树干去哪个关卡

  结算数据 (任务结果)
  └── 任务评分/用时/队友状态 → 传给结算关卡


━━━ 树干 (Trunk) — GameInstance ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  UBlueHandCommandGameInstance
  ├── RequestLevelTransition()  ← 所有关卡切换唯一入口
  ├── GamePhase 状态机
  ├── OnGamePhaseChanged 广播
  ├── LoadOrCreateSaveGame()
  ├── SaveGame()
  └── 便捷函数:
      ├── GoToTutorial()
      ├── GoToHome()
      ├── StartMission(FName MissionMapName)
      └── GoToMissionResult(const FString& ResultsMessage)


━━━ 树枝 (Branches) — GameModes ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  ┌──────────────────────────────────────────────────┐
  │  StartupLoadingGameMode  →  启动加载              │
  │       ↓                                          │
  │  TutorialGameMode       →  教学关卡 (第1关)       │
  │       ↓                                          │
  │  HomeGameMode           →  主菜单 / 关卡选择      │
  │       ↓                                          │
  │  ├── AssaultGameMode    →  突击关卡 (正式游玩)    │
  │  ├── [Locked]           →  未来关卡 (锁定)        │
  │  ├── [Locked]           →  未来关卡 (锁定)        │
  │  └── MissionResultGameMode →  结算页面            │
  └──────────────────────────────────────────────────┘
```

---

## 二、GamePhase 状态机

对应 Tiny Trouble 的 `ETinyTroubleGamePhase`：

```cpp
UENUM(BlueprintType)
enum class EBlueHandCommandGamePhase : uint8
{
    None            UMETA(DisplayName = "None"),
    Tutorial        UMETA(DisplayName = "Tutorial"),
    Home            UMETA(DisplayName = "Home"),
    Assault         UMETA(DisplayName = "Assault"),
    MissionResult   UMETA(DisplayName = "Mission Result")
};
```

**流程**：
```
None → Tutorial → Home → Assault → MissionResult → Home → (循环)
                    ↘ 锁定关卡 (提示"即将开放")
```

---

## 三、关卡切换请求结构体

对应 Tiny Trouble 的 `FTinyTroubleLevelRequest`：

```cpp
USTRUCT(BlueprintType)
struct FBlueHandCommandLevelRequest
{
    GENERATED_BODY()

    /** 目标关卡 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Request")
    FName TargetLevelName;

    /** 目标游戏阶段 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Request")
    EBlueHandCommandGamePhase TargetPhase = EBlueHandCommandGamePhase::None;

    /** 扩展数据（如任务结果、评价等） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Request")
    TMap<FName, FString> MetaData;
};
```

---

## 四、GameInstance — 树干的核心逻辑

```
RequestLevelTransition(请求)
├── 验证 TargetLevelName 不为空
├── 从 MetaData 中提取额外数据（如任务结果）
├── 调用 SetGamePhase(TargetPhase) → 广播 OnGamePhaseChanged
│   └── UI 层监听到阶段变化，更新界面
├── UGameplayStatics::OpenLevel(TargetLevelName)
└── 返回 true
```

**关卡配置**（可在蓝图编辑）：

| 变量 | 默认值 | 说明 |
|------|--------|------|
| TutorialLevelName | `Tutorial_map` | 教学关卡 |
| HomeLevelName | `Home_map` | 主菜单/关卡选择 |
| DefaultMissionLevelName | `Assault_map` | 默认突击关卡 |
| MissionResultLevelName | `MissionResult_map` | 结算页面 |

**Gating（门控）**：
```
CanGoToHome()      → SaveGame.bTutorialCompleted == true
CanStartMission()  → SaveGame.bTutorialCompleted == true
```

---

## 五、SaveGame — 树根持久化数据

```cpp
UCLASS()
class UBlueHandCommandSaveGame : public USaveGame
{
    // --- 玩家进度 ---
    bool bTutorialCompleted = false;
    int32 TotalMissionsPlayed = 0;
    FString BestRank;

    // --- 关卡解锁 ---
    TArray<FName> UnlockedMissions;

    // --- 音频设置 ---
    float MasterVolume = 1.0f;
    float SFXVolume = 0.85f;

    // --- 手势配置 ---
    FString GestureProfilePath = TEXT("BlueHandGestures/BlueHandGestureProfile.json");
};
```

---

## 六、Home 关卡的关卡选择逻辑

Tiny Trouble 的 Home 关卡通过手势选择赛道。你的 Home 关卡同理：

```
玩家在 Home 关卡
├── 看到关卡列表（2个可用 + N个锁定）
├── 伸手选择关卡
│   ├── 教学关卡 → GameInstance.GoToTutorial()
│   ├── 突击关卡 → GameInstance.StartMission("Assault_map")
│   ├── 锁定关卡 → 弹出提示 "即将开放"
│   └── 设置选项 → 调整音量等（写入 SaveGame）
└── 手势选择确认
```

Tiny Trouble 的处理方式：
- **Home 关卡是树根**：它生成 `FTinyTroubleLevelRequest`，通过 GameInstance 的便捷函数提交
- **GameInstance 是树干**：接收请求，验证门控条件，切换关卡
- **GameMode 是树枝**：每个关卡有独立的 GameMode，专注自己的玩法逻辑

---

## 七、对照 Tiny Trouble 的映射

| Tiny Trouble | Blue-Hand-Command | 说明 |
|-------------|-------------------|------|
| `TinyTroubleGameInstance` | `BlueHandCommandGameInstance` | 树干，完全相同的模式 |
| `TinyTroubleSaveGame` | `BlueHandCommandSaveGame` | 树根持久化数据 |
| `TinyTroubleLevelRequest` | `BlueHandCommandLevelRequest` | 关卡切换请求结构 |
| `ETinyTroubleGamePhase` | `EBlueHandCommandGamePhase` | 阶段枚举 |
| `Bird_Tutorial_map` | `Tutorial_map` | 教学关卡 |
| `GameHome` | `Home_map` | 主菜单关卡 |
| `BirdRace_map` | `Assault_map` | 正式游玩关卡 |
| `RaceResults_map` | `MissionResult_map` | 结算关卡 |
| Tutorial 完成 → Home | Tutorial 完成 → Home | 完全相同的流程 |
| Home 选择赛道 → Race | Home 选择关卡 → Assault | 选择逻辑相同 |
| Race 结束 → RaceResults | Assault 结束 → MissionResult | 结算后回到 Home |

---

## 八、关卡流程总图

```
游戏启动
   │
   ▼
StartupLoading (启动加载)
   │
   ▼
───────────────────────────────────────────────────
                  首次游玩?                   是否完成教学?
               ┌───┴───┐                   ┌───┴───┐
              是       否                  否       是
               │       │                   │       │
               ▼       ▼                   ▼       ▼
           Tutorial   Home ←───  ────  Tutorial   Home
               │               完成               │
               ▼                                   ▼
           Home ───→ Assault ───→ MissionResult
                           ↑              │
                           └──── 回到Home ─┘

所有切换都走 GameInstance.RequestLevelTransition()
```

---

## 九、代码结构

```
Source/BlueHandCommand/
├── BlueHandCommand.h / .cpp              (入口)
├── BlueHandCommand.Build.cs
│
├── BlueHandCommandGameInstance.h / .cpp  (树干 — GameInstance)
├── BlueHandCommandSaveGame.h / .cpp      (树根 — SaveGame)
│
├── TutorialGameMode.h / .cpp             (树枝 — 教学)
├── HomeGameMode.h / .cpp                 (树枝 — 主菜单)
├── AssaultGameMode.h / .cpp              (树枝 — 突击)
├── MissionResultGameMode.h / .cpp        (树枝 — 结算)
│
└── CommandGestureComponent.h / .cpp      (手势指挥组件)
```
