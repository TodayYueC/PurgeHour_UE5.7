------

# ⚔️ PurgeHour — UE5 高阶混合战斗框架 技术说明文档

> **引擎**: Unreal Engine 5.7 | **语言**: C++17 & Blueprints | **最后更新**: 2026-03-11

---

## 目录

1. [项目概述](#1-项目概述)
2. [技术栈总览](#2-技术栈总览)
3. [目录结构与模块划分](#3-目录结构与模块划分)
4. [核心架构设计](#4-核心架构设计)
5. [角色系统](#5-角色系统)
6. [输入系统](#6-输入系统)
7. [武器系统 — 枪械](#7-武器系统--枪械)
8. [武器系统 — 近战剑](#8-武器系统--近战剑)
9. [Gameplay Ability System (GAS) 架构](#9-gameplay-ability-system-gas-架构)
10. [后坐力系统](#10-后坐力系统)
11. [UI 与数据绑定](#11-ui-与数据绑定)
12. [动画系统（蓝图层）](#12-动画系统蓝图层)
13. [GameplayCue 表现层（蓝图层）](#13-gameplaycue-表现层蓝图层)
14. [物理材质与射击反馈](#14-物理材质与射击反馈)
15. [蓝图资产清单与职责](#15-蓝图资产清单与职责)
16. [GameplayTag 注册表](#16-gameplaytag-注册表)
17. [架构演进记录](#17-架构演进记录)

---

## 1. 项目概述

**PurgeHour** 是一个基于 **Unreal Engine 5.7** 开发的第三人称混合战斗系统底层框架，支持 **TPS 射击** 与 **ACT 近战** 的无缝切换。

项目全面采用 **Gameplay Ability System (GAS)** 进行架构设计，严格遵循：

- **逻辑与表现分离** — 服务器算逻辑，客户端跑表现
- **数据驱动** — 武器属性、动画、特效全部由 DataAsset 配置
- **网络同步优先** — ASC 挂载于 PlayerState，天然支持多人联机

---

## 2. 技术栈总览

| 领域         | 技术                                                         |
| ------------ | ------------------------------------------------------------ |
| **引擎版本** | Unreal Engine 5.7                                            |
| **编程语言** | C++17 + Blueprints（混合开发）                               |
| **核心框架** | Gameplay Ability System (GAS)                                |
| **输入系统** | Enhanced Input System (EIS)                                  |
| **动画技术** | Animation Blueprint, BlendSpace, Anim Montage, IK Retarget, Layered Animation, Anim Notify State |
| **运动扭曲** | MotionWarping Plugin                                         |
| **视觉表现** | GameplayCue (GCN), Niagara VFX, Decal（弹孔贴花）            |
| **物理反馈** | Physical Materials (6类自定义物理表面)                       |
| **渲染管线** | DX12 / SM6, Lumen (GI+反射), Virtual Shadow Maps, Substrate 材质, 光线追踪 |
| **构建依赖** | Core, CoreUObject, Engine, InputCore, EnhancedInput, GameplayAbilities, GameplayTags, GameplayTasks, MotionWarping |

---

## 3. 目录结构与模块划分

### 3.1 C++ 源码目录 (`Source/PurgeHour/`)

```
Source/PurgeHour/
├── PurgeHour.Build.cs                    # 模块构建定义 & 依赖声明
│
├── Public/
│   ├── AbilitySystem/
│   │   ├── BaseAttributeSet.h            # 属性集基类（空壳，预留扩展）
│   │   ├── HeroASC.h                     # ASC 基类
│   │   ├── HeroYueASC.h                  # ASC 子类（实际挂载）
│   │   ├── HeroAttributeSet.h            # 英雄属性集（Health/Ammo/ReserveAmmo）
│   │   └── GA/
│   │       ├── BaseGA.h                  # GA 基类（InstancedPerActor + StartUpTag）
│   │       ├── BaseWeaponGA.h            # 枪械 GA 基类（获取武器 + 换弹计算）
│   │       └── BaseSwordGA.h             # 近战 GA 基类（获取剑引用）
│   │
│   ├── Characters/
│   │   ├── CharacterBase.h               # 角色基类（通用射线检测工具）
│   │   └── Hero/
│   │       └── Hero.h                    # 主角类（武器管理/后坐力/GAS接口/MotionWarping）
│   │
│   ├── Data/
│   │   ├── WeaponData.h                  # 枪械 DataAsset（网格体/射速/伤害/弹药/后坐力/音效/特效）
│   │   └── SwordData.h                   # 近战 DataAsset（网格体/伤害/蒙太奇数组/Combo序列）
│   │
│   ├── PureData/
│   │   └── HeroState.h                   # 角色状态枚举（EmptyHanded/HoldingWeapon/HoldingSword）
│   │
│   ├── Subsystem/
│   │   └── DelegatesSubsystem.h          # [已废弃] 旧委托子系统，广播已迁移至 PlayerState
│   │
│   ├── System/
│   │   ├── MainGameMode.h                # 游戏模式（空壳，蓝图配置 DefaultPawnClass 等）
│   │   ├── HeroController.h              # 玩家控制器（Enhanced Input 绑定层）
│   │   ├── HeroPlayerState.h             # 玩家状态（ASC 宿主 + UI 广播中枢）
│   │   └── FightHUD.h                    # HUD（创建并管理 FightWidget）
│   │
│   ├── UI/
│   │   ├── UserWidgetBase.h              # Widget 基类（空壳，预留扩展）
│   │   └── FightWidget.h                 # 战斗 HUD Widget（武器名/弹药/血条）
│   │
│   └── Weapon/
│       ├── WeaponBase.h                  # 枪械实体（数据载体 + 拾取碰撞 + GA 配置）
│       └── SwordBase.h                   # 近战剑实体（数据载体 + 拾取碰撞 + GA 配置）
│
└── Private/
    └── (与 Public 对应的 .cpp 实现文件)
```

### 3.2 蓝图内容目录 (`Content/_PurgeHour/`)

```
Content/_PurgeHour/
├── Blueprints/
│   ├── Animation/
│   │   ├── ABP_Hero.uasset              # 主角动画蓝图（状态机 + 分层动画）
│   │   ├── BS_HeroHoldWeapon.uasset     # 持枪混合空间
│   │   ├── BS_HeroNoWeapon.uasset       # 空手混合空间
│   │   ├── BS_HeroSword.uasset          # 持剑混合空间
│   │   ├── AM/
│   │   │   └── AM_ShootOnce.uasset      # 开枪蒙太奇（分层上半身）
│   │   ├── IK/                           # IK Retarget 资产（自动生成）
│   │   ├── Notify/
│   │   │   ├── ANS_ComboWindow.uasset   # 连招窗口通知状态
│   │   │   └── ANS_MeleeTrace.uasset    # 近战逐帧物理追踪通知状态
│   │   └── Sword/
│   │       ├── Combo/                    # 连招蒙太奇序列 (AM_Combo1_1~3, combo_01~06)
│   │       ├── Melee/                    # 普通攻击蒙太奇 (attack_01~05_Montage)
│   │       └── Move/                     # 持剑移动动画 (idle/walk/run 8方向)
│   │
│   ├── Characters/
│   │   ├── Hero/
│   │   │   └── BP_Hero.uasset           # 主角蓝图（配置 Mesh/SpringArm/GE/GA）
│   │   └── BP_Monster.uasset            # 怪物蓝图
│   │
│   ├── GameplayAbility/
│   │   ├── GA/
│   │   │   ├── GA_Fire.uasset           # 射击 GA（异步连发状态机）
│   │   │   ├── GA_Reload.uasset         # 换弹 GA（SetByCaller 原子换弹）
│   │   │   ├── GA_Melee.uasset          # 普通近战攻击 GA
│   │   │   └── GA_Combo.uasset          # 连招 GA（事件驱动 + 输入缓冲）
│   │   ├── GE/
│   │   │   ├── GE_AmmoCost.uasset       # 弹药消耗 GE（Instant, -1 Ammo）
│   │   │   ├── GE_HeroAttributes.uasset # 属性初始化 GE（Override 初始血量/弹药）
│   │   │   └── GE_Reload.uasset         # 换弹 GE（SetByCaller 注入弹药值）
│   │   └── Cue/
│   │       ├── GCN_Weapon_Fire.uasset   # 开火表现 Cue（枪口特效/音效/弹壳）
│   │       ├── GCN_Weapon_Impact.uasset # 命中表现 Cue（弹孔贴花/材质粒子/音效）
│   │       └── GCN_Sword.uasset         # 近战表现 Cue（挥砍特效/音效）
│   │
│   ├── System/
│   │   ├── BP_MainGameMode.uasset       # 游戏模式蓝图
│   │   ├── BP_HeroController.uasset     # 控制器蓝图（配置 IMC + IA）
│   │   ├── BP_HeroPlayerState.uasset    # PlayerState 蓝图
│   │   └── BP_FightHUD.uasset           # HUD 蓝图（配置 FightWidgetClass）
│   │
│   ├── UI/
│   │   └── WBP_FightWidget.uasset       # 战斗 UI Widget（武器名/弹药/血条布局）
│   │
│   ├── Weapon/
│   │   ├── BP_WeaponBase.uasset         # 枪械基类蓝图
│   │   ├── BP_AK47.uasset              # AK47 蓝图实例（配置 DataAsset + GA 类）
│   │   ├── BP_SwordBase.uasset          # 剑基类蓝图
│   │   └── BP_Sword.uasset             # 剑蓝图实例（配置 DataAsset + GA 类）
│   │
│   └── Test/
│       └── BP_TestWall.uasset           # 测试用墙体
│
├── DataAsset/
│   ├── DA_AK47.uasset                   # AK47 武器数据资产
│   └── DA_Sword.uasset                  # 剑数据资产
│
├── Input/
│   ├── IMC_Hero.uasset                  # 输入映射上下文
│   └── IA/
│       ├── IA_HeroMove.uasset           # 移动 (WASD, Value2D)
│       ├── IA_HeroLook.uasset           # 视角 (鼠标, Value2D)
│       ├── IA_HeroJump.uasset           # 跳跃 (Space)
│       ├── IA_Fire.uasset               # 开火 (鼠标左键)
│       ├── IA_Focus.uasset              # 瞄准/聚焦 (鼠标右键)
│       ├── IA_Reload.uasset             # 换弹 (R)
│       ├── IA_Melee.uasset              # 近战攻击 (鼠标左键/指定键)
│       └── IA_Combo.uasset              # 连招 (指定键)
│
├── Map/
│   └── TestMap.uasset                   # 测试关卡
│
└── PA/                                   # 物理材质 (Physical Assets)
    ├── PM_Body.uasset                   # 身体
    ├── PM_Dirt.uasset                   # 泥土
    ├── PM_Grass.uasset                  # 草地
    ├── PM_Metal.uasset                  # 金属
    ├── PM_Rock.uasset                   # 岩石
    └── PM_Wood.uasset                   # 木材
```

---

## 4. 核心架构设计

### 4.1 GAS 宿主解耦 — Owner/Avatar 分离

```
┌─────────────────────────────────────────────────────────┐
│                    AHeroPlayerState                      │
│                  (Owner Actor — 逻辑枢纽)                 │
│                                                         │
│  ┌──────────────────┐   ┌──────────────────────────┐    │
│  │  UHeroYueASC     │   │  UHeroAttributeSet       │    │
│  │  (ASC 组件)       │   │  ├ Health / MaxHealth    │    │
│  │  Replicated:Mixed│   │  ├ Ammo / MaxAmmo        │    │
│  │                  │   │  └ ReserveAmmo           │    │
│  └──────────────────┘   └──────────────────────────┘    │
│                                                         │
│  ┌──────────────────────────────────────────────────┐   │
│  │  UI 广播委托 (Dynamic Multicast Delegates)         │   │
│  │  ├ OnHealthChanged       → FightWidget            │   │
│  │  ├ OnMaxHealthChanged    → FightWidget            │   │
│  │  ├ OnAmmoChanged         → FightWidget            │   │
│  │  ├ OnMaxAmmoChanged      → FightWidget            │   │
│  │  ├ OnReserveAmmoChanged  → FightWidget            │   │
│  │  └ OnWeaponNameChanged   → FightWidget            │   │
│  └──────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────┘
                          │
                   InitAbilityActorInfo
                          │
                          ▼
┌─────────────────────────────────────────────────────────┐
│                       AHero                             │
│                  (Avatar Actor — 表现化身)                 │
│                                                         │
│  SpringArm + Camera（TPS 视角）                           │
│  WeaponSocket / SwordSocket（武器挂载点）                   │
│  后坐力系统（Pending + Tick 线性消耗）                       │
│  MotionWarpingComponent（攻击吸附）                        │
│  GA Handle 管理（Fire/Reload/Attack/Combo）               │
└─────────────────────────────────────────────────────────┘
```

**设计意图**：

- ASC 与 AttributeSet 挂载在 `AHeroPlayerState` 上，确保角色死亡重生后 Buff/冷却/弹药数据**持久化**
- `AHero` 仅作为表现化身，在 `PossessedBy` 和 `OnRep_PlayerState` 中调用 `InitAbilityActorInfo(PlayerState, this)` 完成双向绑定
- ASC 复制模式为 `Mixed`，本地玩家可实现完美预测，模拟 Actor 仅接收最小化广播

### 4.2 角色状态机

```cpp
UENUM(BlueprintType)
enum class EHeroState : uint8
{
    EmptyHanded,     // 空手
    HoldingWeapon,   // 持枪（TPS 射击模式）
    HoldingSword,    // 持剑（ACT 近战模式）
};
```

**状态切换时机**：

| 触发条件             | 目标状态        | 附加行为                                                     |
| -------------------- | --------------- | ------------------------------------------------------------ |
| 角色 BeginPlay       | `EmptyHanded`   | 初始状态                                                     |
| 碰触枪械触发 Overlap | `HoldingWeapon` | 吸附至 WeaponSocket，`bUseControllerRotationYaw = true`，赋予 GA_Fire + GA_Reload |
| 碰触剑触发 Overlap   | `HoldingSword`  | 吸附至 SwordSocket，`bOrientRotationToMovement = true`，赋予 GA_Attack + GA_Combo |

**技能释放状态守卫**：

所有技能释放函数（`Fire`/`StopFire`/`Reload`/`Melee`/`Combo`）在入口处强制检查当前状态：

```
Fire / StopFire / Reload  → 必须 HoldingWeapon 且 CurrentWeapon 有效
Melee / Combo             → 必须 HoldingSword 且 CurrentSword 有效
```

### 4.3 ASC 继承链

```
UAbilitySystemComponent
    └── UHeroASC          (C++ 基类，预留扩展钩子)
         └── UHeroYueASC  (C++ 子类，实际挂载实例)
```

### 4.4 AttributeSet 继承链

```
UAttributeSet
    └── UBaseAttributeSet       (C++ 基类空壳)
         └── UHeroAttributeSet  (英雄属性集)
              ├── Health        (当前血量, Clamp [0, MaxHealth])
              ├── MaxHealth     (最大血量)
              ├── Ammo          (当前弹夹弹药, Clamp [0, MaxAmmo])
              ├── MaxAmmo       (弹夹上限)
              └── ReserveAmmo   (备弹数量, Clamp [0, ∞))
```

**属性保护**：

- `PreAttributeBaseChange`：在属性被修改前执行 Clamp（`Health` ∈ [0, MaxHealth]，`Ammo` ∈ [0, MaxAmmo]，`ReserveAmmo` ≥ 0）
- `PostGameplayEffectExecute`：GE 执行后的二次保护（Health 死亡判定入口）

---

## 5. 角色系统

### 5.1 继承链

```
ACharacter
    └── ACharacterBase  (Tick 关闭, 通用射线检测 UniDoLineTrace)
         └── AHero      (Tick 开启, 主角全部逻辑)
```

### 5.2 ACharacterBase — 角色基类

| 功能                                    | 说明                                                      |
| --------------------------------------- | --------------------------------------------------------- |
| `PrimaryActorTick.bCanEverTick = false` | 基类关闭 Tick，节省性能                                   |
| `UniDoLineTrace()`                      | 通用蓝图可调用射线检测工具，支持物理材质返回 + Debug 绘制 |

### 5.3 AHero — 主角类

**构造函数配置**：

| 配置项                    | 值               | 说明                 |
| ------------------------- | ---------------- | -------------------- |
| SpringArm TargetArmLength | 250              | TPS 摄像机距离       |
| SpringArm SocketOffset    | (0, 65, 40)      | 右肩偏移（肩射视角） |
| bUsePawnControlRotation   | true (SpringArm) | 弹簧臂跟随控制器旋转 |
| bUseControllerRotationYaw | true (初始)      | 角色朝向视角方向     |
| JumpMaxHoldTime           | 0.2s             | 跳跃蓄力时间         |
| GravityScale              | 2.0              | 加强下落手感         |
| AirControl                | 0.5              | 空中移动控制力       |

**核心职责**：

1. **武器管理** — `PickUpWeapon()` / `PickUpSword()` 处理拾取、吸附、状态切换、GA 赋予
2. **技能路由** — `Fire()` / `StopFire()` / `Reload()` / `Melee()` / `Combo()` 转发到 ASC
3. **后坐力系统** — `ApplyRecoil()` / `ResetRecoil()` + Tick 线性消耗（详见第10节）
4. **攻击吸附** — `UpdateWarpTarget()` 使用球形重叠检测 + 点积筛选前方120°目标
5. **GAS 接口** — 实现 `IAbilitySystemInterface`，从 PlayerState 获取 ASC
6. **GE 初始化** — `InitGEToSelf()` 在 BeginPlay 时应用初始属性 GE

---

## 6. 输入系统

### 6.1 Enhanced Input 架构

```
IMC_Hero (InputMappingContext)
    │
    ├── IA_HeroMove    → WASD        → HeroController::Move()
    ├── IA_HeroLook    → 鼠标        → HeroController::Look()
    ├── IA_HeroJump    → Space       → HeroController::Jump() / JumpEnd()
    ├── IA_Fire        → 鼠标左键    → HeroController::Fire() / StopFire()
    ├── IA_Focus       → 鼠标右键    → HeroController::FocusAimOpen() / FocusAimClose()
    ├── IA_Reload      → R           → HeroController::Reload()
    ├── IA_Melee       → 指定键      → HeroController::Melee()
    └── IA_Combo       → 指定键      → HeroController::Combo()
```

### 6.2 AHeroController — 输入处理流程

```
玩家按键
    → Enhanced Input 触发 IA
    → HeroController 回调函数
    → 获取 AHero 引用
    → 调用 Hero 对应函数
    → Hero 内部状态守卫检查
    → 通过 ASC 激活/取消 GA
```

**特殊处理**：

- **Fire**: `Started` → `Hero::Fire()` (设 `bIsFiring=true`, 激活 GA) | `Completed` → `Hero::StopFire()` (设 `bIsFiring=false`, 标记 `InputPressed=false`, 取消 GA)
- **FocusAim**: 仅在 `HoldingWeapon` 状态下允许开镜，调用蓝图实现事件 `AimFocusOpen`/`AimFocusClose`
- **Combo**: 若 GA_Combo 已激活，通过 `HandleGameplayEvent` 发送 `Input.Combo` Tag 实现输入缓冲；否则直接激活

---

## 7. 武器系统 — 枪械

### 7.1 设计哲学：数据载体 + 能力工厂

```
AWeaponBase (Actor)
    │
    ├── [组件]
    │   ├── USceneComponent (Root)
    │   ├── UStaticMeshComponent (地面展示用)
    │   ├── USkeletalMeshComponent (持有后枪械骨骼动画)
    │   └── USphereComponent (拾取碰撞触发器)
    │
    ├── [数据]
    │   └── UWeaponData* WeaponDataAsset    ← 所有配置集中于此
    │
    ├── [GAS 配置]
    │   ├── TSubclassOf<UBaseWeaponGA> GAFireClass    ← 开火 GA 图纸
    │   └── TSubclassOf<UBaseWeaponGA> GAReloadClass  ← 换弹 GA 图纸
    │
    └── [已废弃] 开火/停火/射击循环/子弹管理/后坐力/特效音效 → 全部迁移至 GAS
```

### 7.2 UWeaponData — 枪械数据资产

| 字段                 | 类型                  | 说明                       |
| -------------------- | --------------------- | -------------------------- |
| `WeaponName`         | FName                 | 内部标识名                 |
| `WeaponDisplayName`  | FText                 | UI 显示名（可本地化）      |
| `StaticMesh`         | UStaticMesh*          | 地面展示静态网格           |
| `SkeletalMesh`       | USkeletalMesh*        | 持有后骨骼网格（枪械动画） |
| `WeaponActorClass`   | TSubclassOf\<AActor\> | 武器蓝图类                 |
| `AttackSpeed`        | float                 | 开火间隔（秒）             |
| `Damage`             | float                 | 单发伤害                   |
| `InitBulletNum`      | int32                 | 初始弹夹子弹数             |
| `MaxWeaponBulletNum` | int32                 | 弹夹上限                   |
| `FireSound`          | USoundCue*            | 开火音效                   |
| `FireAnimation`      | UAnimationAsset*      | 枪械开火动画               |
| `BulletEffect`       | UParticleSystem*      | 弹壳抛出特效               |
| `WeaponPic`          | UTexture2D*           | 武器缩略图                 |
| `RecoilPitchOffset`  | float                 | 垂直后坐力基值             |
| `RecoilYawOffset`    | float                 | 水平后坐力基值             |
| `RecoilMaxCount`     | int32                 | 最大后坐力计数阈值         |
| `WeaponDescription`  | FText                 | 武器描述文本               |

### 7.3 拾取流程

```
1. 玩家 Capsule 进入 PickupCollision Overlap
2. WeaponBase::OnPickupBeginOverlap()
   ├── Cast<AHero> 确认拾取者
   ├── 调用 Hero::PickUpWeapon(this)
   │   ├── 设置 CurrentWeapon
   │   ├── 切换状态 → HoldingWeapon
   │   ├── 关闭碰撞
   │   ├── AttachToComponent → WeaponSocket (SnapToTarget)
   │   ├── 设置旋转模式 → 朝视角方向
   │   ├── ASC->GiveAbility(GAFireClass)  → 存储 GAFireHandle
   │   └── ASC->GiveAbility(GAReloadClass) → 存储 GAReloadHandle
   ├── 通过 PlayerState 广播武器名到 UI
   ├── 通过 PlayerState 广播弹药到 UI
   └── 销毁 StaticMesh + PickupCollision 组件
```

---

## 8. 武器系统 — 近战剑

### 8.1 ASwordBase 结构

```
ASwordBase (Actor)
    │
    ├── [组件]
    │   ├── USceneComponent (Root)
    │   ├── UStaticMeshComponent (剑体网格)
    │   └── USphereComponent (拾取碰撞, Radius=100, OverlapAllDynamic)
    │
    ├── [数据]
    │   └── USwordData* SwordDataAsset
    │
    └── [GAS 配置]
        ├── TSubclassOf<UBaseSwordGA> GAAttackClass    ← 普攻 GA
        └── TSubclassOf<UBaseSwordGA> GAComboClass     ← 连招 GA
```

### 8.2 USwordData — 近战数据资产

| 字段               | 类型                    | 说明                      |
| ------------------ | ----------------------- | ------------------------- |
| `SwordName`        | FName                   | 内部标识名                |
| `SwordDisplayName` | FText                   | UI 显示名                 |
| `StaticMesh`       | UStaticMesh*            | 剑体静态网格              |
| `SwordActorClass`  | TSubclassOf\<AActor\>   | 剑蓝图类                  |
| `Damage`           | float                   | 基础伤害                  |
| `MeleeMontage1~3`  | UAnimMontage*           | 普通攻击蒙太奇（3种变体） |
| `SkillsMontage1`   | UAnimMontage*           | 技能蒙太奇                |
| `Combo1`           | TArray\<UAnimMontage*\> | 连招序列蒙太奇数组        |
| `SwordDescription` | FText                   | 武器描述                  |

### 8.3 拾取流程

```
1. 玩家进入 PickupCollision Overlap
2. SwordBase::OnPickupBeginOverlap()
   ├── Cast<AHero> 确认拾取者
   ├── Hero::PickUpSword(this)
   │   ├── 设置 CurrentSword
   │   ├── 切换状态 → HoldingSword
   │   ├── 关闭碰撞
   │   ├── AttachToComponent → SwordSocket (SnapToTarget)
   │   ├── 切换旋转模式 → 朝运动方向 (bOrientRotationToMovement = true)
   │   ├── ASC->GiveAbility(GAAttackClass) → 存储 GAAttackHandle
   │   └── ASC->GiveAbility(GAComboClass)  → 存储 GAComboHandle
   ├── 通过 PlayerState 广播剑名到 UI
   └── 销毁 PickupCollision 组件
```

---

## 9. Gameplay Ability System (GAS) 架构

### 9.1 GA 继承体系

```
UGameplayAbility
    └── UBaseGA (C++)
         │   ├── InstancingPolicy = InstancedPerActor
         │   └── FGameplayTag StartUpTag (配置用)
         │
         ├── UBaseWeaponGA (C++)
         │   ├── GetCurrentWeapon() → 从 AvatarActor 获取枪械引用
         │   ├── CalculateReload()  → 计算换弹数学 + 应用 SetByCaller GE
         │   └── TSubclassOf<UGameplayEffect> ReloadGEClass
         │   │
         │   ├── GA_Fire (蓝图)    → 异步连发状态机
         │   └── GA_Reload (蓝图)  → 换弹动画 + 调用 CalculateReload
         │
         └── UBaseSwordGA (C++)
             ├── GetCurrentWeapon() → 从 AvatarActor 获取剑引用
             │
             ├── GA_Melee (蓝图)   → 播放攻击蒙太奇
             └── GA_Combo (蓝图)   → 事件驱动连招状态机
```

### 9.2 GA_Fire — 射击 GA（蓝图实现）

**异步连发状态机设计**：

```
ActivateAbility
    │
    ▼
┌─ CommitAbility (验证 + 扣除弹药 GE_AmmoCost) ──────┐
│                                                      │
│  触发 GameplayCue.Weapon.Fire (枪口特效/音效)          │
│  触发 GameplayCue.Weapon.Impact (弹孔/材质反馈)       │
│  调用 Hero::ApplyRecoil (后坐力)                      │
│  播放 FireMontage (分层上半身开枪动画)                  │
│                                                      │
│  Wait (AttackSpeed 射速间隔)                          │
│                                                      │
│  Check: InputPressed == true ?                       │
│  ├── Yes → 循环回 CommitAbility                      │
│  └── No  → EndAbility                               │
└──────────────────────────────────────────────────────┘
```

**关键机制**：

- `CommitAbility` 同时承担"验证弹药是否足够"和"扣除弹药"两个职责，保证原子性
- `GE_AmmoCost` 为 Instant GE，`Ammo` 属性 -1
- `InputPressed` 由 C++ 侧 `Fire()`/`StopFire()` 控制，实现按住连发、松开停止

### 9.3 GA_Reload — 换弹 GA（蓝图实现）

```
ActivateAbility
    │
    ▼
播放换弹蒙太奇 (Wait for Montage End)
    │
    ▼
调用 C++ CalculateReload()
    │
    ├── 读取弹夹上限 (WeaponData->MaxWeaponBulletNum)
    ├── 读取当前弹夹 (AttributeSet->Ammo)
    ├── 读取当前备弹 (AttributeSet->ReserveAmmo)
    ├── 计算: AmmoNeeded = MagazineSize - CurrentAmmo
    ├── 计算: AmmoToTransfer = Min(AmmoNeeded, ReserveAmmo)
    ├── 构造 GE Spec (GE_Reload)
    ├── SetByCallerMagnitude("Data.Weapon.Ammo", NewCurrentAmmo)
    ├── SetByCallerMagnitude("Data.Weapon.ReserveAmmo", NewReserveAmmo)
    └── ApplyGameplayEffectSpecToSelf
    │
    ▼
EndAbility
```

### 9.4 GA_Melee — 普攻 GA（蓝图实现）

```
ActivateAbility
    │
    ▼
从 SwordData 选取攻击蒙太奇
    │
    ▼
PlayMontageAndWait
    │  (蒙太奇中嵌入 ANS_MeleeTrace 逐帧物理追踪)
    │  (蒙太奇中嵌入 ANS_ComboWindow 连招窗口)
    │
    ▼
触发 GameplayCue.Sword.Attack
    │
    ▼
EndAbility
```

### 9.5 GA_Combo — 连招 GA（蓝图实现）

```
ActivateAbility
    │
    ▼
播放 Combo1[0] 蒙太奇
    │
    ▼
Wait Gameplay Event (Tag: Input.Combo)  ←── 来自 Hero::Combo() 的事件
    │
    ├── 在 ANS_ComboWindow 激活期间收到事件
    │   → 播放 Combo1[1] 蒙太奇
    │   → 继续等待下一个 Input.Combo 事件
    │
    ├── 在 ANS_ComboWindow 激活期间收到事件
    │   → 播放 Combo1[2] 蒙太奇
    │   → ...
    │
    └── 蒙太奇播放完毕 / 无后续输入
        → EndAbility
```

**C++ 侧 Combo 路由逻辑**（`Hero::Combo()`）：

```cpp
if (GA_Combo 已激活) {
    // 发送 Gameplay Event 实现输入缓冲
    HandleGameplayEvent(Tag "Input.Combo", EventData);
} else {
    // 首次激活
    TryActivateAbility(GAComboHandle);
}
```

### 9.6 GE 清单

| GE                  | 类型                  | 用途                                                         |
| ------------------- | --------------------- | ------------------------------------------------------------ |
| `GE_HeroAttributes` | Override (Instant)    | 初始化角色全部属性（Health/MaxHealth/Ammo/MaxAmmo/ReserveAmmo） |
| `GE_AmmoCost`       | Instant               | 每次开火扣 1 发弹夹弹药 (Ammo -1)                            |
| `GE_Reload`         | Instant + SetByCaller | 换弹时原子设置弹夹和备弹值                                   |

### 9.7 GameplayTag 注册表

| Tag                         | 用途                                   |
| --------------------------- | -------------------------------------- |
| `Data.Weapon.Ammo`          | GE_Reload SetByCaller 键: 换弹后弹夹值 |
| `Data.Weapon.MaxAmmo`       | 预留: 最大弹夹                         |
| `Data.Weapon.ReserveAmmo`   | GE_Reload SetByCaller 键: 换弹后备弹值 |
| `Event.ComboEnd`            | 连招结束事件                           |
| `Event.ComboStart`          | 连招开始事件                           |
| `Event.ComboWindow`         | 连招窗口事件                           |
| `Event.Melee`               | 近战攻击事件                           |
| `GameplayCue.Sword.Attack`  | 近战表现 Cue                           |
| `GameplayCue.Weapon.Fire`   | 开火表现 Cue                           |
| `GameplayCue.Weapon.Impact` | 命中表现 Cue                           |
| `Input.Combo`               | 连招输入缓冲事件 Tag                   |
| `Sword.Attack`              | 剑攻击技能 Tag                         |
| `Sword.Combo`               | 剑连招技能 Tag                         |
| `Sword.Melee`               | 剑近战技能 Tag                         |
| `Weapon.Fire`               | 开火技能 Tag                           |

---

## 10. 后坐力系统

### 10.1 设计原则

后坐力系统**完全运行在角色 (AHero) 上**，由 GA 在每次开火时调用 `ApplyRecoil()`，角色 Tick 负责平滑施加。

### 10.2 核心参数

| 参数                  | 默认值 | 说明                                         |
| --------------------- | ------ | -------------------------------------------- |
| `RecoilApplyTime`     | 0.03s  | 每次后坐力在多长时间内施加完毕（越小越干脆） |
| `RecoilRecoveryDelay` | 0.3s   | 停止射击后多久开始恢复后坐力计数             |
| `RecoilRecoverySpeed` | 8.0 /s | 每秒恢复多少"发"的后坐力计数                 |

### 10.3 算法流程

```
ApplyRecoil(HorizontalRecoil, VerticalRecoil, MaxRecoilCount)
    │
    ├── RecoilShotCount++ (连发计数递增)
    │
    ├── 斜坡系数 RampAlpha = 1 - exp(-0.7 * (ShotCount - 1))
    │   ├── 第1发: ~0.08 (几乎无后坐力)
    │   ├── 第2发: ~0.25
    │   ├── 第3发: ~0.55
    │   └── 第4发+: ~1.0 (满后坐力)
    │
    ├── 垂直衰减: ShotCount > MaxRecoilCount → VerticalScale = 0.15
    │
    ├── 水平漂移: ShotCount > MaxRecoilCount → HorizontalScale = 2.5 + 随机方向
    │
    ├── 累加到 PendingRecoilPitch / PendingRecoilYaw
    ├── 重置 RecoilApplyTime 计时器
    └── 重置 TimeSinceLastShot = 0 (阻止恢复)

Tick(DeltaTime)
    │
    ├── [施加阶段] 在 RecoilApplyTime 内线性消耗 Pending
    │   ├── ApplyPitch = Pending * Min(DeltaTime / TimeRemaining, 1.0)
    │   ├── AddControllerPitchInput(-ApplyPitch) (向上抬)
    │   └── 同理处理 Yaw
    │
    └── [恢复阶段] TimeSinceLastShot > RecoveryDelay 后
        └── RecoilShotCount -= RecoverySpeed * DeltaTime
            (逐帧递减，实现"停枪一段时间后重新开枪前几发又是准的")

ResetRecoil() — 停火时调用
    └── 清空 Pending 余量，不重置计数 (计数由 Tick 自动恢复)
```

### 10.4 手感体验

| 场景                         | 行为                                             |
| ---------------------------- | ------------------------------------------------ |
| 点射（1~2发）                | 几乎无后坐力，射击精准                           |
| 连射（3~4发）                | 后坐力逐渐上升到标准值                           |
| 长按压枪（> MaxRecoilCount） | 垂直后坐力大幅衰减（不再猛抬），水平随机漂移增大 |
| 停止射击 0.3s 后             | 后坐力计数开始以 8发/秒 恢复                     |
| 停止射击 ~1s 后              | 计数归零，再次开枪恢复初始精准度                 |

---

## 11. UI 与数据绑定

### 11.1 架构概览

```
UHeroAttributeSet 属性变化
    │
    ▼
ASC::GetGameplayAttributeValueChangeDelegate()
    │
    ▼
AHeroPlayerState::OnXxxAttributeChanged() (C++ 回调)
    │
    ▼
Dynamic Multicast Delegate 广播
    │
    ▼
UFightWidget (在 NativeConstruct 时绑定)
    ├── UpdateWeaponName()    → TextBlock "WeaponName"
    ├── UpdateCurrentBullets()→ TextBlock "CurrentBullets"
    ├── UpdateReserveAmmo()   → TextBlock "AllBullets"
    ├── UpdateHealth()        → ProgressBar "HealthBar"
    └── UpdateMaxHealth()     → 缓存 MaxHealth 用于百分比计算
```

### 11.2 数据流详解

1. **属性变化拦截**：`HeroPlayerState::BindAttributeCallbacks()` 在 BeginPlay 时注册5个属性变化委托
2. **广播分发**：每个回调函数将新值通过 `DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam` 广播
3. **UI 订阅**：`FightWidget::NativeConstruct()` 获取 PlayerState 并绑定所有委托
4. **初始刷新**：NativeConstruct 时立即读取 AttributeSet 当前值刷新一次（确保开局 UI 正确）
5. **武器信息**：拾取武器/剑时，由 `PlayerState::BroadcastWeaponName()` 和 `BroadcastAmmo()` 主动广播

### 11.3 Widget 绑定控件

| 控件名 (BindWidget) | 类型         | 绑定数据                        |
| ------------------- | ------------ | ------------------------------- |
| `WeaponName`        | UTextBlock   | 当前武器/剑显示名               |
| `CurrentBullets`    | UTextBlock   | 弹夹内子弹数                    |
| `AllBullets`        | UTextBlock   | 备弹数量                        |
| `HealthBar`         | UProgressBar | 血量百分比 (Health / MaxHealth) |

### 11.4 HUD 创建链

```
GameMode 指定 HUD 类 → AFightHUD
    │
    AFightHUD::BeginPlay()
    │
    ├── CreateWidget<UFightWidget>(FightWidgetClass)
    └── AddToViewport()
```

---

## 12. 动画系统（蓝图层）

### 12.1 ABP_Hero — 主角动画蓝图

**状态机架构**（推断自蓝图资产与 C++ 状态枚举）：

```
AnimGraph
    │
    ├── [Base Layer — 下半身/全身移动]
    │   │
    │   └── State Machine: Locomotion
    │       ├── State: Idle / Walk / Run
    │       │   根据 HeroState 切换不同 BlendSpace:
    │       │   ├── EmptyHanded  → BS_HeroNoWeapon (空手移动)
    │       │   ├── HoldingWeapon → BS_HeroHoldWeapon (持枪移动)
    │       │   └── HoldingSword  → BS_HeroSword (持剑移动)
    │       │
    │       ├── State: Jump
    │       └── State: Fall
    │
    ├── [Upper Body Layer — 分层动画 (Layered Blend per Bone)]
    │   │
    │   └── Slot: DefaultSlot (用于播放蒙太奇)
    │       ├── AM_ShootOnce    — 上半身开枪动画（腰部以上混合）
    │       ├── attack_01~03    — 近战攻击蒙太奇
    │       └── AM_Combo1_1~3   — 连招蒙太奇
    │
    └── [Aim Offset — 瞄准偏移]
        └── 根据控制器 Pitch/Yaw 混合上半身瞄准方向
```

### 12.2 BlendSpace 配置

| 资产                | 状态          | 轴                | 说明              |
| ------------------- | ------------- | ----------------- | ----------------- |
| `BS_HeroNoWeapon`   | EmptyHanded   | Speed × Direction | 空手8方向移动混合 |
| `BS_HeroHoldWeapon` | HoldingWeapon | Speed × Direction | 持枪8方向移动混合 |
| `BS_HeroSword`      | HoldingSword  | Speed × Direction | 持剑8方向移动混合 |

### 12.3 Anim Notify State

| 资产              | 用途                                        | 触发位置                 |
| ----------------- | ------------------------------------------- | ------------------------ |
| `ANS_MeleeTrace`  | 逐帧物理追踪（MultiBoxTrace），检测挥砍命中 | 攻击蒙太奇的"发力帧"区间 |
| `ANS_ComboWindow` | 标记连招输入窗口，允许缓存下一段连招输入    | 攻击蒙太奇后摇阶段       |

### 12.4 IK Retarget

项目使用 UE5 的 IK Retarget 系统进行动画重定向：

- `IK_AutoGeneratedSource` / `IK_AutoGeneratedTarget` — 自动生成的 IK Rig
- `RTG_AutoGenerated` — Retarget 资产
- 持剑移动动画独立 IK 配置（`Sword/Move/` 下的 IK 资产）

### 12.5 持剑移动动画资产

8方向全套移动动画（Walk + Run），用于 `BS_HeroSword` 混合空间：

| 方向 | Walk             | Run             |
| ---- | ---------------- | --------------- |
| 前   | move_walk_front  | move_run_front  |
| 后   | move_walk_back   | move_run_back   |
| 左   | move_walk_left   | move_run_left   |
| 右   | move_walk_right  | move_run_right  |
| 左前 | move_walk_frontL | move_run_frontL |
| 右前 | move_walk_frontR | move_run_frontR |
| 左后 | move_walk_backL  | move_run_backL  |
| 右后 | move_walk_backR  | move_run_backR  |

Idle 动画：`N_idle`

---

## 13. GameplayCue 表现层（蓝图层）

严格遵循"服务器算逻辑，客户端跑表现"原则，所有视听反馈由 GameplayCue 处理。

### 13.1 GCN_Weapon_Fire — 开火表现

| 职责     | 实现方式                                      |
| -------- | --------------------------------------------- |
| 枪口闪光 | 在枪械 SkeletalMesh 的 Muzzle Socket 生成粒子 |
| 开火音效 | 从 WeaponData->FireSound 播放 SoundCue        |
| 弹壳抛出 | 从 WeaponData->BulletEffect 生成弹壳粒子      |
| 枪械动画 | 播放 WeaponData->FireAnimation                |

### 13.2 GCN_Weapon_Impact — 命中表现

| 职责         | 实现方式                                   |
| ------------ | ------------------------------------------ |
| 物理材质判定 | 从 FHitResult 获取 PhysMaterial，分支处理  |
| 弹孔贴花     | 在命中点生成 Decal Component，法线对齐     |
| 材质粒子     | 根据物理材质选择对应 Niagara/Particle 系统 |
| 命中音效     | 根据物理材质播放对应 SoundCue，位置播放    |

### 13.3 GCN_Sword — 近战表现

| 职责     | 实现方式                  |
| -------- | ------------------------- |
| 挥砍特效 | 刀光轨迹粒子              |
| 挥砍音效 | 位置播放音效              |
| 命中反馈 | 顿帧 (HitStop) + 镜头震动 |

---

## 14. 物理材质与射击反馈

### 14.1 自定义物理表面

在 `DefaultEngine.ini` 中注册6类物理表面：

| SurfaceType  | 名称  | 用途        |
| ------------ | ----- | ----------- |
| SurfaceType1 | Dirt  | 泥土/沙地   |
| SurfaceType2 | Grass | 草地        |
| SurfaceType3 | Rock  | 岩石/混凝土 |
| SurfaceType4 | Metal | 金属        |
| SurfaceType5 | Wood  | 木材        |
| SurfaceType6 | Body  | 肉体        |

### 14.2 物理材质资产 (`Content/_PurgeHour/PA/`)

每种物理表面对应一个 `PhysicalMaterial` 资产（`PM_Dirt` / `PM_Grass` / `PM_Rock` / `PM_Metal` / `PM_Wood` / `PM_Body`），在场景材质中指定。

### 14.3 音效资源体系 (`Content/Sound/Sound/`)

```
BulletImpact/
    ├── Body/       — 击中肉体音效
    ├── Concrete/   — 击中混凝土音效
    ├── Gravel/     — 击中砂砾音效
    ├── Metal/      — 击中金属音效
    ├── Rock/       — 击中岩石音效
    └── Wood/       — 击中木材音效

Weapon/              — 枪械操作音效（装弹、拉栓等）
Sword/               — 近战武器音效
Attack/ / Damage/    — 攻击/受伤音效
FootStep/            — 脚步音效
```

---

## 15. 蓝图资产清单与职责

### 15.1 系统蓝图

| 蓝图                 | C++ 父类         | 职责                                                         |
| -------------------- | ---------------- | ------------------------------------------------------------ |
| `BP_MainGameMode`    | AMainGameMode    | 配置 DefaultPawnClass、PlayerControllerClass、PlayerStateClass、HUDClass |
| `BP_HeroController`  | AHeroController  | 配置 IMC_Hero + 全部 IA 引用                                 |
| `BP_HeroPlayerState` | AHeroPlayerState | 无额外蓝图逻辑，纯 C++ 驱动                                  |
| `BP_FightHUD`        | AFightHUD        | 配置 FightWidgetClass = WBP_FightWidget                      |

### 15.2 角色蓝图

| 蓝图         | C++ 父类 | 职责                                                         |
| ------------ | -------- | ------------------------------------------------------------ |
| `BP_Hero`    | AHero    | 配置 SkeletalMesh/AnimBP/SpringArm参数/InitGE/FireMontage/EmptyMagazineSound |
| `BP_Monster` | (待确认) | 怪物 AI                                                      |

### 15.3 武器蓝图

| 蓝图            | C++ 父类    | 职责                                                 |
| --------------- | ----------- | ---------------------------------------------------- |
| `BP_WeaponBase` | AWeaponBase | 枪械基类蓝图（通常不直接放置）                       |
| `BP_AK47`       | AWeaponBase | AK47实例：配置 DA_AK47 + GAFireClass + GAReloadClass |
| `BP_SwordBase`  | ASwordBase  | 剑基类蓝图                                           |
| `BP_Sword`      | ASwordBase  | 剑实例：配置 DA_Sword + GAAttackClass + GAComboClass |

### 15.4 GAS 蓝图

| 蓝图                | C++ 父类           | 职责                                              |
| ------------------- | ------------------ | ------------------------------------------------- |
| `GA_Fire`           | UBaseWeaponGA      | 异步连发状态机：CommitAbility → Cue → Wait → 循环 |
| `GA_Reload`         | UBaseWeaponGA      | 播放换弹蒙太奇 → CalculateReload()                |
| `GA_Melee`          | UBaseSwordGA       | 播放攻击蒙太奇 → Cue                              |
| `GA_Combo`          | UBaseSwordGA       | 事件驱动连招：Wait Event → 播放下一段 → 循环      |
| `GE_HeroAttributes` | UGameplayEffect    | Override 初始化所有属性                           |
| `GE_AmmoCost`       | UGameplayEffect    | Instant, Ammo -1                                  |
| `GE_Reload`         | UGameplayEffect    | Instant + SetByCaller (Ammo + ReserveAmmo)        |
| `GCN_Weapon_Fire`   | UGameplayCueNotify | 枪口特效 + 音效 + 弹壳                            |
| `GCN_Weapon_Impact` | UGameplayCueNotify | 弹孔贴花 + 材质粒子 + 音效                        |
| `GCN_Sword`         | UGameplayCueNotify | 挥砍特效 + 音效                                   |

### 15.5 UI 蓝图

| 蓝图              | C++ 父类     | 职责                                                         |
| ----------------- | ------------ | ------------------------------------------------------------ |
| `WBP_FightWidget` | UFightWidget | UMG 布局：WeaponName / CurrentBullets / AllBullets / HealthBar |

### 15.6 数据资产

| 资产       | C++ 类      | 内容                                                 |
| ---------- | ----------- | ---------------------------------------------------- |
| `DA_AK47`  | UWeaponData | AK47全部参数：网格体/射速/伤害/弹药/后坐力/音效/特效 |
| `DA_Sword` | USwordData  | 剑全部参数：网格体/伤害/攻击蒙太奇/连招序列          |

---

## 16. GameplayTag 注册表

完整的 Tag 树形结构（`DefaultGameplayTags.ini`）：

```
Data
└── Weapon
    ├── Ammo              # SetByCaller: 弹夹弹药
    ├── MaxAmmo           # 预留: 弹夹上限
    └── ReserveAmmo       # SetByCaller: 备弹

Event
├── ComboEnd              # 连招结束
├── ComboStart            # 连招开始
├── ComboWindow           # 连招窗口
└── Melee                 # 近战攻击

GameplayCue
├── Sword
│   └── Attack            # 近战表现 Cue
└── Weapon
    ├── Fire              # 开火表现 Cue
    └── Impact            # 命中表现 Cue

Input
└── Combo                 # 连招输入缓冲

Sword
├── Attack                # 剑攻击技能标识
├── Combo                 # 剑连招技能标识
└── Melee                 # 剑近战技能标识

Weapon
└── Fire                  # 开火技能标识
```

---

## 17. 架构演进记录

本项目经历了多轮重构，以下记录关键架构变迁：

| 阶段               | 旧方案                                | 新方案                                      | 原因                                            |
| ------------------ | ------------------------------------- | ------------------------------------------- | ----------------------------------------------- |
| 开火逻辑           | WeaponBase::Fire() + TimerHandle 轮询 | GA_Fire 蓝图异步状态机                      | 帧率无关、可预测、支持网络同步                  |
| 子弹管理           | WeaponBase 内 int32 成员变量          | HeroAttributeSet (Ammo/MaxAmmo/ReserveAmmo) | 统一属性系统管理、自动 Clamp、支持 GE 修改      |
| 后坐力             | WeaponBase::Tick 内 FInterpTo 插值    | Hero::ApplyRecoil() + Tick 线性消耗         | 脱离武器类、武器 Tick 已关、在角色统一管理      |
| 特效/音效          | WeaponBase 内直接 Spawn + PlaySound   | GameplayCue (GCN_Weapon_Fire / Impact)      | 逻辑表现分离、仅客户端执行、节省带宽            |
| UI 广播            | GameInstance Subsystem 内 Delegate    | PlayerState 内 Dynamic Multicast Delegate   | 支持多人联机数据隔离、跟随 PlayerState 生命周期 |
| 弹孔贴花           | WeaponBase 内生成 Decal               | GCN_Weapon_Impact 内生成                    | 逻辑表现分离                                    |
| 旋转模式           | 始终朝视角方向                        | 持枪→朝视角 / 持剑→朝运动方向               | 射击 vs 近战操作体验差异化                      |
| 武器 Tick          | WeaponBase Tick 开启（后坐力插值）    | WeaponBase Tick 关闭                        | 后坐力迁移到角色后不再需要                      |
| CharacterBase Tick | 开启                                  | 关闭（仅 Hero 开启）                        | 基类无逐帧逻辑，节省性能                        |
| 委托子系统         | UDelegatesSubsystem (GameInstance)    | 已废弃，保留空壳                            | 广播迁移至 PlayerState                          |

---

## 附录 A: 模块依赖关系图

```
PurgeHour.Build.cs 依赖:
┌────────────────────────────────────────────────┐
│  Core, CoreUObject, Engine, InputCore          │
│  EnhancedInput                                 │
│  GameplayAbilities, GameplayTags, GameplayTasks│
│  MotionWarping                                 │
└────────────────────────────────────────────────┘
```

## 附录 B: 类关系总览图

```
AGameModeBase
    └── AMainGameMode

APlayerController
    └── AHeroController (Enhanced Input 绑定)

APlayerState + IAbilitySystemInterface
    └── AHeroPlayerState (ASC + AttributeSet 宿主, UI 广播中枢)

ACharacter
    └── ACharacterBase (通用工具)
         └── AHero + IAbilitySystemInterface (主角)

AActor
    ├── AWeaponBase (枪械实体)
    └── ASwordBase (剑实体)

AHUD
    └── AFightHUD (创建 FightWidget)

UUserWidget
    └── UUserWidgetBase
         └── UFightWidget (战斗 HUD)

UAbilitySystemComponent
    └── UHeroASC
         └── UHeroYueASC

UAttributeSet
    └── UBaseAttributeSet
         └── UHeroAttributeSet

UGameplayAbility
    └── UBaseGA
         ├── UBaseWeaponGA (枪械 GA 基类)
         │   ├── GA_Fire (蓝图)
         │   └── GA_Reload (蓝图)
         └── UBaseSwordGA (近战 GA 基类)
             ├── GA_Melee (蓝图)
             └── GA_Combo (蓝图)

UPrimaryDataAsset
    └── UWeaponData

UDataAsset
    └── USwordData

UGameInstanceSubsystem
    └── UDelegatesSubsystem [已废弃]
```

---

## 附录 C: 渲染管线配置

| 设置     | 值                                              | 说明            |
| -------- | ----------------------------------------------- | --------------- |
| 图形 API | DX12                                            | 启用 SM6 着色器 |
| 全局光照 | Lumen (r.DynamicGlobalIlluminationMethod=1)     | 硬件光追 GI     |
| 反射     | Lumen (r.ReflectionMethod=1)                    | 硬件光追反射    |
| 阴影     | Virtual Shadow Maps (r.Shadow.Virtual.Enable=1) | 虚拟阴影贴图    |
| 材质系统 | Substrate (r.Substrate=True)                    | UE5 新材质管线  |
| 光线追踪 | 开启 (r.RayTracing=True)                        | 硬件光追支持    |
| 静态光照 | 关闭 (r.AllowStaticLighting=False)              | 全动态光照      |
| 距离场   | 开启 (r.GenerateMeshDistanceFields=True)        | Lumen 依赖      |
| 皮肤缓存 | 开启 (r.SkinCache.CompileShaders=True)          | 光追皮肤着色    |

---

## 附录 D: MotionWarping 攻击吸附

`Hero::UpdateWarpTarget()` 实现前方锥形索敌 + 运动扭曲：

```
1. SphereOverlapActors (半径 350, ObjectType: Pawn, 忽略自身)
2. 遍历结果:
   ├── 计算方向向量 DotProduct > 0.5 (前方约120°)
   ├── 取最近有效目标
   └── 注入 MotionWarpingComponent:
       ├── WarpLocation = TargetLocation + 面向自身方向 * 80 (目标前方80单位)
       └── WarpRotation = 面向目标方向 (Pitch=0)
3. 无有效目标 → 移除 "CombatTarget" Warp Target
```

---

> **文档维护说明**: 本文档基于 `Source/PurgeHour/` 全部 C++ 源码与 `Content/_PurgeHour/` 蓝图资产结构自动生成。蓝图内部逻辑（如 GA 节点图、动画蓝图状态机细节）基于 C++ 接口与资产命名推断，如有出入请以编辑器内实际配置为准。
