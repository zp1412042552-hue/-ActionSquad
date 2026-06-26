# 手部姿势识别系统 — 移植方案

## 概述

本方案描述如何将 Tiny Trouble VR 项目中的 **Oculus 手部姿势识别系统** 移植到 Blue-Hand-Command VR 项目。

Tiny Trouble 使用官方 Oculus Hand Tools 插件中的 `OculusHandPoseRecognition` 模块，通过 `UHandPoseRecognizer` 组件识别手部姿势，再结合 `BirdFlightGestureComponent` 将姿势映射为游戏命令。

---

## 一、系统架构

```
Oculus XR Plugin (手部追踪数据)
        ↓
UHandPoseRecognizer (左/右手各一个)
   └─ 加载姿势模板 (FHandPose)
   └─ 每帧计算手部角度误差
   └─ 返回置信度最高的姿势
        ↓
BirdFlightGestureComponent (我们的指挥层)
   ├─ 接收左右手识别结果
   ├─ 映射为战术命令 (选择队友/移动/警戒/突入/集合)
   ├─ 管理手势槽位 (Gesture Slots)
   └─ 触发 HUD 反馈
        ↓
玩家看到命令反馈 (蓝色手发光/指令线/队友确认)
```

---

## 二、需要移植的文件

### 2.1 插件层面 — 直接使用，无需移植

Tiny Trouble 的插件位于 `E:\Tiny Trouble VR\TinyTrouble\Plugins\`：

| 插件 | 路径 | 作用 |
|------|------|------|
| **OculusHandTools** | `Plugins/OculusHandTools/` | 包含 `OculusHandPoseRecognition` 模块 |
| **OculusUtils** | `Plugins/OculusUtils/` | 工具模块 |

**做法**：在您的项目 `Plugins/` 目录下直接复制这两个文件夹即可。

### 2.2 关键 C++ 模块依赖

在您的 `BlueHandCommand.Build.cs` 中需要添加：

```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    "Core",
    "CoreUObject",
    "Engine",
    "InputCore",
    "EnhancedInput",
    "HeadMountedDisplay",
    "HandInput",                    // ← Oculus 手部输入
    "OculusHandPoseRecognition",    // ← 手部姿势识别
    "OculusXRInput",                // ← Oculus XR 输入
    "Json",                         // ← 姿势模板文件读写
    "UMG", "Slate", "SlateCore"     // ← 可选，用于 HUD
});
```

### 2.3 C++ 源文件 — 不需要照搬，参考其设计

Tiny Trouble 的 `BirdFlightGestureComponent`（约 60KB）是核心指挥逻辑层。**您不需要直接复制它**，而是参考它的设计模式，为您的项目写一个 `CommandGestureComponent`。

关键参考点：

| Tiny Trouble 的实现 | 您的项目中对应的需求 |
|---------------------|---------------------|
| `UHandPoseRecognizer` (左右手各一个) | 同样需要两个，左右手各一个 |
| `FHandPose` 编码手部姿势 | 完全相同的机制，姿势模板不同 |
| 4 个手势槽位 (TakeoffReady/Launch/SpeedControl/Brake) | 6 个手势 (选队友A/选队友B/移动/警戒/突入/集合) |
| 录制系统 (JSON 姿势模板文件) | 相同的录制/加载机制 |
| `CameraHandInput` (手部骨骼修正) | 同样需要，如果使用 PoseableMesh 显示手 |

关于 `CameraHandInput` 和手部骨骼修正，请参考：
- `E:\Tiny Trouble VR\Doc\HomePawnHandTrackingFixup.md`
- 关键：`HandRootFixupRotation = FQuat(-0.5, -0.5, 0.5, 0.5)`

---

## 三、UHandPoseRecognizer 工作原理

### 3.1 姿势字符串格式

每个姿势被编码为一个字符串，例如"竖食指"（用于选择队友A）：

```
L T0-52-18+51 T1+13-8+30 ...
```

格式说明：
- `L` / `R` — 左手 / 右手
- `T0`~`T3` — 拇指关节
- `I1`~`I3` — 食指关节
- `M1`~`M3` — 中指关节
- `R1`~`R3` — 无名指关节
- `P0`~`P3` — 小指关节
- `W` — 手腕
- 每个关节后的三个数字 = Pitch, Yaw, Roll（角度）
- `+0` 忽略该轴，`*N` 给该关节加权

### 3.2 置信度计算

```
误差 = Σ(所有关节角度差的平方)
置信度 = EAMC / max(EAMC, 实际误差)
  其中 EAMC = Error At Max Confidence
```

### 3.3 录制流程

1. 摆好手势 → 2. 按录制键 → 3. 倒计时 → 4. 采集 1.25 秒数据 → 5. 计算平均值和范围 → 6. 写入 JSON 文件

生成的姿势模板保存在：`Saved/BirdFlightGestures/BirdFlightGestureProfile.json`

---

## 四、6 个手势设计建议

参考 Tiny Trouble 的四个手势，建议您的命令手势如下：

| 手势 | 功能 | 姿势设计思路 |
|------|------|-------------|
| ☝️ 伸 1 根食指 | **选择队友 A** | 右手食指指向天，其余握拳 |
| ✌️ 伸 2 根手指 | **选择队友 B** | 右手食指+中指伸出（V字） |
| 👆 食指指向下 | **移动到位置** | 右手食指指向地面方向 |
| ✋ 手掌竖起 | **原地警戒** | 右手手掌打开竖起（Stop 手势） |
| 🤚 手掌前推 | **进入/搜索** | 双手或单手手掌向前推 |
| ✊ 握拳拉回 | **集合** | 右手握拳向后拉回 |

> **注意**：以上是初始设计建议，实际姿势需要在 VR 中录制测试后才能确定最佳方案。

---

## 五、移植步骤

### 步骤 1：复制插件
```
复制 E:\Tiny Trouble VR\TinyTrouble\Plugins\OculusHandTools\
复制 E:\Tiny Trouble VR\TinyTrouble\Plugins\OculusUtils\
粘贴到您的项目 Plugins\ 目录
```

### 步骤 2：配置项目文件
```
编辑 .uproject → 添加插件引用 (参见 TinyTrouble.uproject 的 Plugins 段)
编辑 Build.cs → 添加模块依赖
```

### 步骤 3：创建指挥手势组件
参考 `BirdFlightGestureComponent` 的架构，写一个新的 `CommandGestureComponent`：
- 左右手各挂一个 `UHandPoseRecognizer`
- 6 个手势槽位映射到您的 6 个命令
- 手势确认延迟 0.2~0.4 秒（防误触）

### 步骤 4：录制姿势模板
- 在 VR Preivew 中摆姿势 → 录制
- 调整 EAMC 参数以控制识别灵敏度
- 测试不同光照条件下的稳定性

### 步骤 5：集成到手部显示
如果您想显示蓝色虚拟手（如立项书所述）：
- 参考 `CameraHandInput` 处理手部骨骼旋转
- 使用 `UPoseableMeshComponent` 显示手部网格
- 应用 `HandRootFixupRotation` 修正手腕方向

---

## 六、注意事项

1. **手部追踪版本**：在 `DefaultEngine.ini` 设置 `HandTrackingVersion=Default`（Quest 默认使用最新版本）
2. **追踪频率**：建议 `HandTrackingFrequency=HIGH`（60Hz 而不是 30Hz）
3. **误触防护**：所有命令必须有 0.2~0.4 秒确认时间，并有视觉反馈
4. **备用方案**：如果手势识别不稳定，提供备用的按钮操作方式
5. **姿势模板文件**：需要确保打包时随 APK 一起发布（在 `DefaultEngine.ini` 中 `DirectoriesToAlwaysStageAsNonUFS`）
