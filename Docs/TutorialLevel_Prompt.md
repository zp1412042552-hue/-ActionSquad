# 教程关卡 — AI 绘图提示词

## 一、提示词说明

这个提示词用来生成一张**俯瞰全景图**（4 个区域全部可见），和四张**各区域的单独特写**（用于抠细节）。

生成的图片将作为**建模蓝图**——所有的物体都要从图中抠出来单独建模。

---

## 二、全景俯瞰提示词（Overview）

> 用于生成整个训练场的鸟瞰图，展示 4 个区域的布局和所有物体。

```text
Low-poly stylized top-down isometric view of an indoor VR tactical training facility, cel-shaded cartoon art style, clean geometric shapes, flat shading with soft rim lighting. Bird's eye view 45-degree angle looking down, showing the entire training hall layout.

[低多边形风格化俯视等轴测图，室内 VR 战术训练设施，卡通渲染艺术风格，干净的几何形状，平面着色加柔和轮廓光。鸟瞰 45 度视角，展示整个训练大厅布局]

ENVIRONMENT OVERVIEW:
A single large rectangular indoor warehouse, light gray concrete floor with visible grid lines, white walls with dark gray baseboards. The hall is divided into 4 distinct zones从左到右 arranged left to right. Orange safety markings and yellow-black warning stripes on the floor separating each zone. Industrial ceiling with exposed steel trusses and fluorescent light bars. No windows, no natural light.

[环境总览：单一大型矩形室内仓库，浅灰色混凝土地面可见网格线，白色墙面配深灰色踢脚线。大厅从左到右分为 4 个不同区域，地板上有橙色安全标线和黄黑警示条纹分隔各区。工业天花板有暴露的钢桁架和荧光灯管。无窗户，无自然光]

ZONE 1 — GESTURE TRAINING (leftmost section, approximately 8m x 10m):
[区域 1 — 手势训练（最左侧，约 8×10 米）]

Objects list:
- Wall-mounted gesture instruction panel: A large white rectangular sign board on the back wall, printed with 6 hand-gesture icons in order: (1) single index finger up ☝️, (2) two fingers ✌️, (3) finger pointing down 👇, (4) palm facing forward palm up ✋, (5) both palms pushing forward 🤚, (6) fist pulled back ✊. Each icon has a number below it (1-6) and Chinese text label. The icons are drawn in neon blue (#00D4FF) on white background.
- Recognition feedback light bar: A row of 6 circular LED lights mounted below the gesture panel, one under each icon. Red when not recognized, green when the gesture is matched. Simple flat circles.
- Two training mannequins: Simple low-poly human-shaped dummies, plain white color with no face features, standing side by side in the center of the zone. One has a blue shoulder stripe (Teammate A), one has a yellow shoulder stripe (Teammate B). Both have simplified rifle-shaped accessory across chest.
- A flat screen display on a thin pole stand: Floating wall monitor style, thin rectangular screen showing large text "GESTURE RECOGNITION" at top, with a gesture icon preview area below.
- Floor marking: A white painted semicircle on the floor about 1.5m radius, labeled "STAND HERE" (站位) in the center, indicating where the player should stand.
- Simple training barrier: One waist-high orange traffic barrier (like construction barrier) separating this zone from the next.

[物体列表：
- 墙上手势指示板：后墙上的大型白色标志板，印有 6 个手势图标——(1)食指指天、(2)V字、(3)食指指地、(4)手掌竖起、(5)双掌前推、(6)握拳拉回。每个图标下有数字编号 1-6 和中文文字标签。图标为霓虹蓝色 #00D4FF，白底。
- 识别反馈灯条：手势板下方的 6 个圆形 LED 灯，每个图标下一个。未识别时红色，识别成功时绿色。简单的扁平圆圈。
- 两个训练假人：简单的低多边形人体形状假人，纯白色无面部特征，并排站在区域中央。一个有蓝色肩带（队友 A），一个有黄色肩带（队友 B）。胸前有简化步枪形状配件。
- 立式平板显示器：细杆支架上的壁挂式屏幕，薄矩形屏幕，顶部显示大字"手势识别"，下方有手势图标预览区。
- 地面标记：地面上的白色半圆，半径约 1.5 米，中心标有"站位"，指示玩家站立位置。
- 橙色交通隔离栏：一个齐腰高的橙色施工隔离栏，分隔此区域与下一区域]

ZONE 2 — COMMAND TRAINING (second from left, approximately 10m x 12m):
[区域 2 — 指挥基础（左二，约 10×12 米）]

Objects list:
- Three numbered floor markers: Three large circular markers on the floor, labeled "A" (blue circle), "B" (yellow circle), "C" (green circle). Each about 1m diameter. Arranged in a triangle pattern about 3m apart.
- Two training mannequins (same as Zone 1): Standing near marker A. One with blue shoulder stripe, one with yellow stripe. Same low-poly white dummy design.
- "RETURN" assembly point: A large yellow circle painted on the floor near the entrance side of this zone, labeled "集合" (Assemble) in Chinese. Diameter about 2m.
- A waist-high concrete barrier wall: A simple L-shaped low wall (1m high, 2m long each side) made of light gray concrete blocks with visible seams, positioned near marker C. Used as a "cover" position for the Watch command training.
- Safety barrier tape: Yellow-black warning tape on thin poles around the perimeter of this zone. 4 poles with tape strung between them.
- Instruction sign: A small metal sign on a pole near the zone entrance, with text "COMMAND TRAINING" and 4 simple pictograms showing: select → point → watch → return. Flat sign, low-poly.
- A simple wooden crate prop: One brown wooden crate (1m x 1m x 1m) placed near the L-shaped wall as additional cover object. Visible edge lines, flat brown color.

[物体列表：
- 三个编号地面标记：地板上三个大型圆形标记，标有"A"（蓝色圆）、"B"（黄色圆）、"C"（绿色圆）。每个直径约 1 米。呈三角形排列，间距约 3 米。
- 两个训练假人（同区域 1）：站在标记 A 附近，一个有蓝色肩带，一个有黄色肩带。同样是低多边形白色假人设计。
- "集合"点：区域入口侧地面上的大型黄色圆圈，标有"集合"。直径约 2 米。
- 齐腰高混凝土挡墙：简单的 L 形矮墙（1 米高，每边 2 米长），浅灰色混凝土砌块可见接缝，位于标记 C 附近。用作"警戒"命令训练的掩体位置。
- 安全警戒带：细杆上的黄黑警示带围栏，4 根杆子中间拉警戒带。
- 指示牌：区域入口附近的杆上小金属牌，文字"指挥训练"加 4 个简单示意图：选择→指向→警戒→集合。扁平标牌，低多边形。
- 木箱道具：一个棕色木箱（1×1×1 米），放在 L 形墙附近作为额外的掩体物体。可见边缘线，纯棕色]

ZONE 3 — BREACH TRAINING (third from left, approximately 8m x 12m):
[区域 3 — 破门训练（左三，约 8×12 米）]

Objects list:
- Training door and frame: A standard single door (1m wide x 2.2m tall) set in a white painted door frame mounted against a partition wall. The door is a simple flat rectangle with a doorknob, painted dark gray. Above the door frame, a red sign "BREACH TRAINING" (破门训练) in white text. The door has a red/green status light on the frame: currently red (locked).
- Small room behind the door: Visible through a window cutout (50cm x 50cm) in the wall beside the door. The room interior is visible: approximately 4m x 4m, same light gray floor, empty except for one paper silhouette target.
- Paper silhouette target in room: A simple flat cardboard-cutout-style human silhouette target on a sliding rail. White paper with printed human outline, some target zone rings (5 rings) on the chest area. Simple low-poly.
- Window cutout: A square opening in the wall beside the door, about 50cm x 50cm, with a simple sill. Thick wall cross-section visible (about 30cm thick).
- One training mannequin (same style): Standing near the door on the outside, positioned as if awaiting breach command. Blue shoulder stripe.
- Door status indicator light: A small panel beside the door with two round indicator lights — one red (locked), one green (breached). Currently red is lit.
- Rubber training rifle on the floor: A simple blue gun-shaped prop lying on the floor near the training mannequin, to be picked up. Low-poly, blocky shape.

[物体列表：
- 训练门与门框：隔断墙上的标准单门（1 米宽 × 2.2 米高），白色门框。门为简单扁平矩形带门把手，深灰色涂装。门框上方红色标牌"破门训练"白字。门框上有红/绿状态灯（当前为红色=已锁）。
- 门后小房间：通过门旁墙上的窗口开口（50×50cm）可见房内。房间约 4×4 米，同款浅灰地板，空无一物只有一纸人靶。
- 房内纸人形靶：滑轨上的扁平纸板剪裁风格人形靶。白色纸印有人形轮廓，胸部有靶环区（5 环）。简单低多边形。
- 窗口开口：门旁墙上的方形开口，约 50×50cm，带简单窗台。可见厚墙剖面（约 30cm 厚）。
- 一个训练假人（同款）：站在门外侧，姿势像在等待破门命令。蓝色肩带。
- 门状态指示灯：门旁的小面板，两个圆形指示灯——一个红色（已锁），一个绿色（已破开）。当前红灯亮。
- 地面训练步枪：训练假人附近地面上的蓝色枪形道具（低多边形块状），待拾取]

ZONE 4 — FINAL EXAM (rightmost section, approximately 10m x 14m):
[区域 4 — 综合考核（最右侧，约 10×14 米）]

Objects list:
- Short corridor: A 2m wide passage with walls on both sides, about 4m long, leading from Zone 3 entrance to two doorways.
- Door 1 (left): Simple door frame without door (open doorway), dark interior visible. Red sign above: "ROOM 1" (房间1).
- Door 2 (right): Same style, open doorway. Red sign above: "ROOM 2" (房间2).
- Room 1 interior (visible from outside): Small room 4m x 4m. Contains one standing enemy target — a low-poly human-shaped dummy in dark uniform (solid dark gray, no details), holding a simplified rifle-shaped object pointed downward. The target is standing still facing the door. Floor is same light gray.
- Room 2 interior (visible from outside): Same size as Room 1. Contains two enemy targets of the same style, positioned in two corners of the room. Both facing the door.
- Timer display: A large wall-mounted rectangular screen visible from the corridor entrance. Shows "00:00" in large red LED-style numbers. Flashing colon. Simple low-poly screen.
- Scoreboard: A whiteboard-style panel next to the timer, showing "BEST TIME" and three rows of text: "01:23", "01:45", "02:10". Drawn with simple black marker-style lines.
- Pop-up target mechanism (on wall of corridor): A rectangular mechanism box mounted on the wall, with a folded-down paper target attached. When activated, it would spring up. Currently in folded-down position. Simple flat rectangle shape.
- Two metal folding chairs: Simple low-poly gray metal folding chairs placed against the wall near the scoreboard, for observers. Crossed legs, flat seat.
- Zone completion indicator: A tall thin pole with a green light at the top, placed at the far end of Zone 4. Green light when the exam is passed. Currently off.

[物体列表：
- 短走廊：两侧有墙的 2 米宽通道，长约 4 米，从区域 3 入口通向两个门口。
- 门 1（左）：无门的门框（开口），可见内部黑暗。上方红色标牌"房间 1"。
- 门 2（右）：同款开口门框。上方红色标牌"房间 2"。
- 房间 1 内部（从外可见）：4×4 米小房间。内有一个站姿敌人靶——深色制服的低多边形人体假人（纯深灰色无细节），持简化步枪形物体向下指。面朝门静止站立。地板为同款浅灰色。
- 房间 2 内部（从外可见）：同房间 1 大小。内含两个同款敌人靶，位于房间两个角落。均面朝门。
- 计时器：走廊入口处可见的大型壁挂式矩形屏幕。显示"00:00"大号红色 LED 数字。冒号闪烁。简单的低多边形屏幕。
- 成绩板：计时器旁的白板式面板，显示"最佳成绩"和三行文字："01:23"、"01:45"、"02:10"。简单黑色马克笔线条绘制。
- 弹出式靶机构（走廊墙面）：墙上的矩形机构盒，带折叠纸靶。激活时会弹起。当前为折叠状态。简单扁平矩形。
- 两把金属折叠椅：简单的低多边形灰色金属折叠椅靠在成绩板附近的墙上，供观察者使用。交叉腿，扁平座位。
- 区域完成指示灯：细长杆顶端带绿灯，放置在区域 4 远端。考核通过时绿灯亮。当前为熄灭状态]

OVERALL DECORATIONS (across all zones):
[全场景通用装饰物]

- Safety fire extinguisher: Bright red cylinder on a wall bracket, located near the Zone 1-2 boundary. Simple cylinder with a small gauge dial.
- Ceiling lights: 8 rows of 4 LED panel lights in the ceiling (32 total), simple white rectangles giving off cool white ambient glow.
- Emergency exit sign: Green glowing "EXIT" sign above the far right end of Zone 4. Simple green rectangle with white text.
- Wall junction boxes: Two small gray electrical boxes on walls, about 20cm x 30cm, with a single switch each.
- Numbered zone markers: Four white rectangle signs at the top of each zone's back wall, numbered "1", "2", "3", "4" in large black bold text.
- Baseboards: Dark gray simple strips along the bottom of all walls, 10cm high.

[通用装饰物：
- 灭火器：区域 1-2 边界墙支架上的亮红色圆柱体。简单圆柱带小仪表盘。
- 天花板灯：天花板上 8 排 ×4 个 LED 面板灯（共 32 个），简单的白色矩形，发出冷白色环境光。
- 紧急出口标志：区域 4 最右端上方的绿色发光"出口"标志。简单绿色矩形白字。
- 墙面接线盒：两个灰色小型电箱挂在墙上，约 20×30cm，各带一个开关。
- 区域编号标志：每个区域后端墙上方的 4 个白色矩形标志，大号黑色粗体标着"1""2""3""4"。
- 踢脚线：所有墙面底部的深灰色简单条带，10cm 高]

COLOR PALETTE:
Floor: Light gray #C0C0C0, Walls: White #F0F0F0, Baseboards: Dark gray #555555, Safety marks: Orange #FF6600 and Yellow-Black stripes, Training props: White #FFFFFF and Brown #8B4513 (crate), Equipment: Dark blue #1A1A3E (tactical gear), Hand glow: Neon blue #00D4FF (the only bright accent color). All colors are flat, no gradients, no complex textures.

[色彩方案：
地板：浅灰 #C0C0C0，墙面：白 #F0F0F0，踢脚线：深灰 #555555，安全标识：橙 #FF6600 和黄黑条纹，训练道具：白 #FFFFFF 和棕 #8B4513（箱子），装备：深蓝 #1A1A3E（战术装备），手部发光：霓虹蓝 #00D4FF（唯一亮色点缀）。所有颜色纯平，无渐变，无复杂纹理]

ART STYLE:
Clean low-poly with flat shading, faint dark edge outlines visible on all objects (ink border style, 1px lines), no detailed textures anywhere — all color comes from flat materials. Quest VR optimized aesthetic. Inspired by "Tiny Trouble VR" visual style but indoor tactical theme. Isometric game screenshot view showing entire facility.

[艺术风格：干净低多边形 + 平面着色，所有物体可见微弱深色边缘轮廓线（墨水边框风格，1 像素线），任何地方无细节纹理——颜色全部来自纯色材质。Quest VR 优化美学。参考 Tiny Trouble VR 的视觉风格但改为室内战术主题。显示整个设施的等轴测游戏截图视角]
```

---

## 三、各区域特写提示词（用于抠细节）

### 区域 ① 特写

```text
Low-poly stylized close-up of Zone 1 — Gesture Training Area. VR first-person view standing at the "STAND HERE" floor marker. Directly ahead is the large wall-mounted gesture instruction panel showing 6 neon blue hand gesture icons: single finger up, two fingers up, finger down, palm up, palms forward, fist pull-back. Below each icon is a circular LED light — the second one glowing green. Two white training mannequins stand to the right, one with blue shoulder stripe (Teammate A) and one with yellow (Teammate B). A thin-pole flat screen monitor on the left shows "手势识别" with a large gesture preview. Orange barrier separates this zone. Neon blue(#00D4FF) glowing hands visible at bottom of frame making a two-finger gesture. Clean low-poly flat shading, Quest VR style, cartoon aesthetic, no textures.
```

### 区域 ② 特写

```text
Low-poly stylized close-up of Zone 2 — Command Training Area. VR perspective view. On the floor: three large circular markers labeled A(blue), B(yellow), C(green) arranged in a triangle. Two white training mannequins (blue and yellow shoulder stripes) standing near marker A. A large yellow circle on the floor labeled "集合". An L-shaped low concrete barrier wall near marker C with a brown wooden crate beside it. Yellow-black warning tape on poles around the zone edge. A small instruction sign on a pole showing 4 pictograms: select → point → watch → return. Player's neon blue hands visible at frame bottom making a pointing gesture, with a cyan holographic line extending toward marker B. Clean low-poly flat shading, cartoon aesthetic.
```

### 区域 ③ 特写

```text
Low-poly stylized close-up of Zone 3 — Breach Training Area. VR perspective view facing a training door. The door is dark gray in a white frame with a doorknob. Red sign above door: "BREACH TRAINING". Door frame has red status LED lit. Beside the door is a square window cutout (50x50cm) showing a small room inside with a paper silhouette target on a rail. A white training mannequin (blue stripe) stands beside the door holding a simplified rifle. A blue rubber training rifle prop lies on the floor nearby. Player's neon blue glowing hands visible making a fist gesture (breach command). The fist has a slight cyan glow trajectory toward the door. Clean low-poly flat shading, cartoon aesthetic.
```

### 区域 ④ 特写

```text
Low-poly stylized close-up of Zone 4 — Final Exam Area. VR perspective view from corridor entrance. Short corridor with two open doorways on left and right. Left doorway has red sign "ROOM 1" — inside visible one dark gray enemy dummy standing still holding a rifle. Right doorway has red sign "ROOM 2" — inside visible two enemy dummies in corners. A large wall-mounted timer screen on the corridor wall showing "12:45" in red LED digits. A white scoreboard beside it with "BEST TIME" and three times written in marker style. A folded target mechanism box on the wall. Two metal folding chairs against the wall. Green zone completion pole at the far end, currently off. Player's neon blue hands visible making a two-finger gesture selecting a teammate. Clean low-poly flat shading, cartoon aesthetic.
```

---

## 四、可提取建模的元素清单

从生成的图片中可以抠出来单独建模的所有物体：

| 编号 | 物体 | 来自区域 | 用途 |
|------|------|---------|------|
| 01 | 🖐️ 蓝色自发光手（左右各一） | 全部 | 玩家手部模型 |
| 02 | 👤 训练假人 A（蓝肩带） | ①②③ | 队友 A 的外观 |
| 03 | 👤 训练假人 B（黄肩带） | ①② | 队友 B 的外观 |
| 04 | 🔫 简化步枪（队友用） | ②③④ | 队友武器 |
| 05 | 🎯 纸人形靶（站立） | ①③④ | 敌人/靶子 |
| 06 | 🚪 训练门（带门框+把手） | ③ | 破门目标 |
| 07 | 🪟 窗口开口（墙剖面） | ③ | 观察口 |
| 08 | 🧱 L 形混凝土挡墙 | ② | 掩体 |
| 09 | 📦 棕色木箱 | ② | 掩体/道具 |
| 10 | 🪑 金属折叠椅 | ④ | 环境道具 |
| 11 | 🚧 橙色交通隔离栏 | ①② | 区域分隔 |
| 12 | ⛔ 黄黑警示带围栏 | ② | 安全围栏 |
| 13 | 🪧 手势指示板（墙面） | ① | 教学提示 |
| 14 | 💡 圆形 LED 反馈灯（红/绿） | ① | 识别反馈 |
| 15 | 📺 立式平板显示器 | ① | 教学屏幕 |
| 16 | ⏱️ 计时器屏幕（墙面） | ④ | 考核计时 |
| 17 | 📋 成绩白板 | ④ | 成绩展示 |
| 18 | 🔴 门状态指示灯面板 | ③ | 门状态 |
| 19 | 🅰️🅱️🅲 地面标记 A/B/C | ② | 指挥目标点 |
| 20 | 🟡 集合圈（地面黄色圆） | ② | 集合点 |
| 21 | 🔵 蓝色训练步枪（地面） | ③ | 可拾取武器 |
| 22 | 🧯 灭火器（墙面红色） | ①-②边界 | 环境装饰 |
| 23 | 🚪 紧急出口标志（绿色） | ④远端 | 环境装饰 |
| 24 | 🔌 墙面接线盒 | 各处 | 环境装饰 |
| 25 | 🏗️ 钢桁架天花板 | 全部 | 环境装饰 |
| 26 | ⬜ LED 面板灯（天花板） | 全部 | 环境灯光 |
| 27 | 📍 站位地面标记（半圆） | ① | 玩家站位 |
| 28 | 🔢 区域编号标志 1-4 | 各区域 | 区域标识 |
| 29 | 👤 敌人假人（深灰色制服） | ④ | 目标敌人 |
| 30 | 🔄 弹出式靶机构（墙面） | ④ | 训练装置 |
| 31 | 🎯 靶环（人形靶胸部） | ③④ | 命中区域 |
