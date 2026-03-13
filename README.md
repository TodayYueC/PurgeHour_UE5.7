# PurgeHour 面试技术架构说明

> 版本：1.0  
> 引擎：Unreal Engine 5.7  
> 语言：C++17 + Blueprints  
> 核心框架：Gameplay Ability System (GAS)


---

## 1）项目定位

### 1.1 项目类型
- 第三人称混合战斗框架（TPS 射击 + ACT 近战）。
- 以“可扩展工程架构”为优先目标，而非一次性功能堆砌。

### 1.2 核心玩法循环
- 探索 -> 拾取武器/剑 -> 切换战斗姿态 -> 远程射击或近战连招 -> 命中/死亡反馈 -> UI 实时同步。

### 1.3 技术栈
- Unreal Engine 5.7
- C++17：核心状态、数据流、网络所有权、高频逻辑
- Blueprints：资产编排（GA、GameplayCue、Montage、UI）
- GAS：技能、属性、效果统一管线
- Enhanced Input：输入抽象与可配置映射
- MotionWarping：近战对目标对齐

---

## 2）架构设计理念

### 2.1 设计原则
- 逻辑/表现分离：
    - C++：权威逻辑、状态机、边界校验、网络同步职责。
    - 蓝图：视觉表现、资源配置、动画窗口、数值调参。
- 数据驱动：
    - 枪械与剑行为通过 DataAsset 配置，避免硬编码分支膨胀。
- 面向多人联机：
    - ASC 挂在 `PlayerState`，Character 作为 Avatar，便于占有切换与复制稳定性。

### 2.2 分层架构

```text
输入层（Controller + EnhancedInput）
  -> 领域层（Hero/Weapon/Sword/Enemy C++）
    -> 能力层（GA/GE/AttributeSet, GAS）
      -> 同步层（PlayerState 广播 + ASC 复制）
        -> 表现层（Widget/AnimBP/GameplayCue 蓝图）
```

---

## 3）运行时主链路（端到端）

### 3.1 角色启动链路
1. `AHero::BeginPlay()` 初始化角色状态与朝向策略。
2. `AHero::InitGEToSelf()` 应用初始化 GE，完成属性冷启动。
3. 占有与复制阶段：
    - `AHero::PossessedBy()` 与 `AHero::OnRep_PlayerState()` 都会调用 `InitAbilityActorInfo(Owner=PlayerState, Avatar=Hero)`。
    - 保证服务端与客户端均完成 GAS ActorInfo 绑定。

### 3.2 捡枪 -> 开火/换弹
1. `AWeaponBase::OnPickupBeginOverlap()` 触发拾取。
2. `Hero->PickUpWeapon(this)`：
    - 附加到 `WeaponSocket`。
    - 设置状态为 `HoldingWeapon`。
    - 授予开火/换弹 GA 并缓存 Handle。
3. `AHeroPlayerState` 广播 UI 初始信息：
    - 武器名
    - 当前弹药快照
4. 输入触发 Hero 接口，Hero 激活对应 GA。

### 3.3 捡剑 -> 近战/连招
1. `ASwordBase::OnPickupBeginOverlap()` 触发拾取。
2. `Hero->PickUpSword(this)`：
    - 附加到 `SwordSocket`。
    - 设置状态为 `HoldingSword`。
    - 授予近战/连招 GA。
3. 连招输入在 GA 活跃时转为 GameplayEvent（`Input.Combo`），避免重复激活。

---

## 4）核心模块深度解析

### 4.1 技能与战斗系统（GAS 中心化）

### C++ 职责
- GA 基类抽象：
    - `UBaseGA`：统一 InstancingPolicy（`InstancedPerActor`）与启动标签扩展位。
    - `UBaseWeaponGA` / `UBaseSwordGA`：提供类型安全的当前装备访问。
- Hero 运行时调度：
    - 通过 `EHeroState` 进行技能前置门控。
    - 管理 Ability Handle 的授予、激活、取消。
- 属性不变量：
    - `UHeroAttributeSet` 对血量/弹夹/备弹做范围约束。
    - Post 阶段再次收敛，避免脏数据扩散。

### 蓝图职责
- 配置具体 GA 资产（`GA_Fire`、`GA_Reload`、`GA_Melee`、`GA_Combo`、`GA_Dodge`）。
- 配置 GameplayCue（`GCN_Weapon_Fire`、`GCN_Weapon_Impact`、`GCN_Sword`）。
- 组织 Montage 与 Notify 窗口（连招窗口、近战追踪窗口）。

### 关键工程决策
- 将开火逻辑从 `WeaponBase` 的计时器/Tick 模式迁移到 GA 生命周期。
- 收益：
    - 生命周期确定性更强
    - 中断与状态切换更可控
    - 网络权威边界更清晰

---

### 4.2 输入系统与连招判定

### 输入绑定模型
- `AHeroController::SetupInputComponent()` 一次性绑定 Enhanced Input。
- Controller 仅负责意图转发，不承载重逻辑。
- Hero 作为领域入口：`Fire` / `StopFire` / `Reload` / `Melee` / `Combo` / `Dodge` / `ToggleFacingMode`。

### 连招衔接策略
- 当连招 GA 已激活：发送 `Input.Combo` 事件标签。
- 当连招 GA 未激活：直接激活连招 GA Handle。
- 避免问题：
    - Ability 实例频繁创建销毁
    - 依赖 Tick 轮询输入窗口
    - Controller 侧分支失控

### 价值点
- 体现事件驱动连招架构（而非每帧轮询）。
- 输入采集与状态推进职责边界明确。

---

### 4.3 网络同步与 UI 同步（PlayerState 枢纽化）

### 所有权策略
- `AHeroPlayerState` 持有 ASC 与 AttributeSet。
- Hero 通过 `GetPlayerState<AHeroPlayerState>()` 获取 ASC。
- 复制模式按战斗数据同步需求设置。

### UI 广播策略
- 在 `BindAttributeCallbacks()` 一次性注册属性变化监听。
- 回调将 GAS 属性变化转为 UI 友好的动态多播：
    - Health / MaxHealth
    - Ammo / MaxAmmo / ReserveAmmo
    - WeaponName
- `UFightWidget::NativeConstruct()` 中绑定委托并执行开局刷新。

### 为什么不继续使用旧 Subsystem 广播
- PlayerState 天然是“每玩家状态”的正确宿主。
- 多人联机、旁观、重连场景语义更一致。

---

### 4.4 后坐力系统（手感与维护性的折中）

### 当前实现
- 在 `AHero::Tick()` 中按时间窗消费 Pending Pitch/Yaw（`RecoilApplyTime`）。
- `ApplyRecoil()` 每发注入武器参数：
    - 水平后坐力基值
    - 垂直后坐力基值
    - 最大后坐力计数
- 行为特性：
    - 前几发缓升（更准）
    - 超阈值后垂直抑制
    - 水平随机漂移增强
    - 独立恢复延迟与恢复速度

### 架构价值
- 后坐力统一收敛在 Hero 侧，武器只提供数据。
- 任意枪械 GA 均可复用同一后坐力入口。

---

### 4.5 视角模式与战斗状态耦合

### 需求
- 空手/持剑：可切换“随移动朝向”与“始终朝向控制器”。
- 持枪：强制“始终朝向控制器”。

### 实现
- `ToggleFacingMode()` 在 `HoldingWeapon` 下直接禁用切换。
- `ApplyFacingModeByState()` 在状态变化时统一落地。
- `GetCurrentFacingMode()` 对调试/UI/蓝图分支可见。

---

### 4.6 敌人 GAS 与死亡管线

### 敌人初始化
- `AEnemyBase` 创建 `UEnemyASC` 与 `UEnemyAttributeSet`。
- BeginPlay 初始化 ActorInfo，并应用一次 `InitGE`。

### 死亡触发链
- AttributeSet Post 阶段检测 `Health <= 0`。
- 调用 `AEnemyBase::SetDead()`：
    - 设置死亡标记
    - 关闭胶囊与网格碰撞
    - 触发死亡 GA

### 下游收益
- `UpdateWarpTarget` 可跳过死亡目标。
- 解决死亡体阻挡角色与锁定异常问题。

---

## 5）数据驱动设计

### 5.1 枪械 DataAsset（`UWeaponData`）
- 网格资源
- 射速与伤害
- 弹药容量元数据
- 后坐力参数
- 图标与文本说明

### 5.2 剑 DataAsset（`USwordData`）
- 静态网格
- 近战伤害
- 连招蒙太奇数组
- UI 展示文本与描述

### 价值
- 工程定义 Schema，策划/技美填资产。
- 新装备可通过资产实例化接入，代码改动最小化。

---

## 6）性能优化与工程质量

### 6.1 Tick 预算控制
- `ACharacterBase` 与武器 Actor 默认关闭 Tick。
- 仅在必要场景开启（如 `AHero` 后坐力与恢复）。

### 6.2 避免蓝图 Tick 承载高频逻辑
- 状态门控、后坐力累计、换弹计算、属性校验放在 C++。
- 蓝图专注事件响应与表现层。

### 6.3 内存与生命周期安全
- UObject 引用使用 `TObjectPtr`。
- ASC/AttributeSet/Owner 使用前做空指针保护。
- 死亡态与碰撞关闭避免 PendingKill 访问风险。

### 6.4 编译/运行期稳定性经验
- 属性变化回调签名与委托严格一致（`FOnAttributeChangeData`）。
- 头文件声明与实现保持完全匹配。
- Overlap/Pickup 流程中先做有效性检查再转换与广播。

---

## 7）技术难点与解决方案

### 7.1 难点：GAS Owner/Avatar 生命周期一致性（联机高难点）
- 问题：ASC 挂载在 `PlayerState` 时，若仅在单一入口初始化 ActorInfo，会出现客户端能力上下文缺失、标签状态不同步、激活失败等时序问题。
- 方案：在 `AHero::PossessedBy()`（服务端）与 `AHero::OnRep_PlayerState()`（客户端）双路径执行 `InitAbilityActorInfo(Owner=PlayerState, Avatar=Hero)`，并保持 ASC 来源统一为 `AHeroPlayerState`。
- 结果：能力上下文初始化具备幂等与时序容错能力，联机场景下技能激活稳定性显著提升。

### 7.2 难点：Ability 授予/激活/取消的句柄生命周期管理
- 问题：武器与剑属于动态装备，若能力句柄管理不严格，会出现重复授予、悬空句柄、停火失败或异常重入。
- 方案：以 `FGameplayAbilitySpecHandle` 为唯一运行时标识，在拾取时集中 `GiveAbility`，输入阶段仅通过 Handle `TryActivateAbility`，停火场景显式 `CancelAbilityHandle`，并在状态门控（`EHeroState`）中限制可激活域。
- 结果：能力生命周期与装备生命周期解耦但可追踪，避免“输入状态与能力状态错位”的高频线上问题。

### 7.3 难点：属性复制一致性到 UI 的闭环同步
- 问题：GAS 属性更新发生在网络复制链路中，若 UI 直接依赖零散事件，容易出现开局未刷新、局部更新丢失、客户端显示滞后。
- 方案：在 `AHeroPlayerState::BindAttributeCallbacks()` 统一订阅 `FOnAttributeChangeData`，将 Health/Ammo/ReserveAmmo/WeaponName 全部收敛为 PlayerState 广播中枢；Widget 构造时先绑定再读取快照，完成“实时事件 + 初始值”双通道同步。
- 结果：属性驱动 UI 的一致性从“尽力而为”升级为“确定性更新”，支持多人联机下稳定展示。

### 7.4 难点：事件驱动连招与 SetByCaller 结算的原子性
- 问题：连招若靠 Tick 轮询窗口，输入与动画窗口容易错拍；换弹若分步修改多个属性，易出现弹夹/备弹短暂不一致。
- 方案：连招采用 GameplayEvent（`Input.Combo`）驱动 GA 状态推进，避免轮询；换弹在 `UBaseWeaponGA::CalculateReload()` 中计算后通过 SetByCaller 一次性注入 GE，原子更新 Ammo 与 ReserveAmmo。
- 结果：输入响应与技能状态机更紧密，弹药结算具备事务性，复杂战斗节奏下仍保持数据正确性。



---

## 8）C++ 与蓝图协作契约

### C++ 提供
- 稳定 API（`BlueprintCallable` / `BlueprintPure`）。
- 状态枚举（`EHeroState`、`EHeroMoveDirection`、`EHeroFacingMode`）供蓝图分支。
- Ability Handle 生命周期与状态切换控制。

### 蓝图负责
- GA/GE/Montage/Cue/Widget 等资产配置。
- 摄像机手感、特效变体、Notify 窗口等表现细节。

### 团队协作收益
- 工程与技术设计可并行迭代，互不阻塞。

---

<a id="code-index"></a>
## 9）代码走查文件索引

- 角色核心：`Source/PurgeHour/Public/Characters/Hero/Hero.h`，`Source/PurgeHour/Private/Characters/Hero/Hero.cpp`
- 输入路由：`Source/PurgeHour/Public/System/HeroController.h`，`Source/PurgeHour/Private/System/HeroController.cpp`
- 同步枢纽：`Source/PurgeHour/Public/System/HeroPlayerState.h`，`Source/PurgeHour/Private/System/HeroPlayerState.cpp`
- 属性约束：`Source/PurgeHour/Public/AbilitySystem/HeroAttributeSet.h`，`Source/PurgeHour/Private/AbilitySystem/HeroAttributeSet.cpp`
- 枪械系统：`Source/PurgeHour/Public/Weapon/WeaponBase.h`，`Source/PurgeHour/Private/Weapon/WeaponBase.cpp`
- 剑系统：`Source/PurgeHour/Public/Weapon/SwordBase.h`，`Source/PurgeHour/Private/Weapon/SwordBase.cpp`
- 敌人 GAS：`Source/PurgeHour/Public/Characters/EnemyBase.h`，`Source/PurgeHour/Private/Characters/EnemyBase.cpp`
- 构建依赖：`Source/PurgeHour/PurgeHour.Build.cs`

---

<a id="oral"></a>
