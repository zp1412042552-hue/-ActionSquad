# Blue-Hand-Command VR

**手势指挥 AI 队友，完成战术突入**

《Blue-Hand-Command》是一款以**手势指挥 AI 小队**为核心交互特色的 VR 战术游戏。玩家扮演小队指挥官，通过虚拟蓝色手势指挥两名 AI 队友，在室内完成搜索、警戒、突入与任务目标。

## 核心特色

- 🖐️ **手势指挥** — 用手势代替菜单，选择队友、移动、警戒、突入
- 👥 **AI 队友系统** — 两名 AI 队友配合玩家完成战术行动
- 🏠 **室内清房** — 逐房间搜索，每扇门后都可能是危险
- 🎮 **短局制** — 每局 5~10 分钟，适合 VR 平台体验

## 开发技术

- **引擎**: Unreal Engine 5.4+
- **平台**: Meta Quest 3S / Quest 3
- **XR**: Meta XR Plugin / OpenXR

## 项目状态

🚧 开发中 — MVP 原型阶段

## 项目文档

- [突击小组.txt](突击小组.txt) — 完整项目立项书
- [Docs/HandGestureSystem_Porting.md](Docs/HandGestureSystem_Porting.md) — 手部姿势识别系统移植方案（来自 Tiny Trouble VR）
- [Docs/AndroidVR_PerformanceSettings.md](Docs/AndroidVR_PerformanceSettings.md) — Quest VR 性能优化配置（Lumen/Nanite/阴影等关闭项）
- [Docs/Architecture_TreeDesign.md](Docs/Architecture_TreeDesign.md) — 树形架构设计（GameInstance 树干 + SaveGame 树根 + GameMode 树枝）
- [Docs/MissionEvaluationSystem.md](Docs/MissionEvaluationSystem.md) — 任务评价体系（5维评分 S~D 级 + 结算画面 VR UI 三级递进展示）
- [Docs/AITeammateBehaviorDesign.md](Docs/AITeammateBehaviorDesign.md) — AI 队友行为系统设计（3层状态 + Behavior Tree + NavMesh）
- [Docs/HandPoseRecordingLevel.md](Docs/HandPoseRecordingLevel.md) — 手部姿势录制关卡设计（纯黑录制环境 + 5秒倒计时 + 自发光手部材质）
- [Docs/TutorialModeDesign.md](Docs/TutorialModeDesign.md) — 教程模式设计（4阶段递进教学：手势识别→指挥基础→破门训练→综合考核）
