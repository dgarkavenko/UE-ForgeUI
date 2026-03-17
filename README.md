# ForgeUI

A standalone, Lyra-style 4-layer UI stack plugin for Unreal Engine 5. Built on top of CommonUI and CommonGame, ForgeUI provides a ready-to-use UI layer architecture: drop it into any UE5 project, set the policy in config, and get a working layered UI stack system with HUD visibility sync and input mode management.

## Layer Architecture

```
┌─────────────────────────────────────────┐
│  Modal Layer (ForgeUI.Layer.Modal)      │  <- Top: dialogs, confirmations
├─────────────────────────────────────────┤
│  Menu Layer (ForgeUI.Layer.Menu)        │  <- Main menu, frontend screens
├─────────────────────────────────────────┤
│  GameMenu Layer (ForgeUI.Layer.GameMenu)│  <- Pause, inventory, settings
├─────────────────────────────────────────┤
│  Game Layer (ForgeUI.Layer.Game)        │  <- Bottom: HUD, in-game widgets
└─────────────────────────────────────────┘
```

Each layer is a `UCommonActivatableWidgetStack`. Widgets pushed to higher layers receive input priority and can suspend layers below them.

## Quick Start

### 1. Enable Plugins

Copy `Plugins/ForgeUI/` into your project. The following plugins must also be present and enabled:

- **CommonUI** (Engine plugin)
- **CommonGame** — provides `UGameUIManagerSubsystem`, `UGameUIPolicy`, `UPrimaryGameLayout`
- **CommonUser** — required by CommonGame
- **ModularGameplayActors** — provides `AModularPlayerController` (base for `ACommonPlayerController`)

Add to your game module's `Build.cs`:

```csharp
PublicDependencyModuleNames.Add("ForgeUI");
```

### 2. Set Up Project Base Classes

CommonGame requires your project to use its base classes for GameInstance, LocalPlayer, and PlayerController. Create minimal subclasses if you don't already have them:

**GameInstance** — must extend `UCommonGameInstance`: | GameInstanceClass in config
**LocalPlayer** — must extend `UCommonLocalPlayer`: | Local Player Class

**PlayerController** — must extend `ACommonPlayerController` (which extends `AModularPlayerController`). This is required because `ACommonPlayerController` broadcasts the `OnPlayerControllerSet` delegate to `UCommonLocalPlayer`, which triggers the UI layout creation chain


### 3. Configure
In `DefaultEngine.ini`, point the engine to ForgeUI's viewport client:

```ini
[/Script/Engine.Engine]
GameViewportClientClassName="/Script/ForgeUI.ForgeGameViewportClient"
```

In `DefaultGame.ini`
```ini
[/Script/ForgeUI.ForgeManagerSubsystem]
DefaultUIPolicyClass=/ForgeUI/Framework/BP_ForgeUIPolicy.BP_ForgeUIPolicy_C
```


### 6. Push Widgets

**C++:**
```cpp
#include "Framework/ForgeLayers.h"
#include "CommonUIExtensions.h"

UCommonUIExtensions::PushContentToLayer_ForPlayer(
    LocalPlayer,
    ForgeLayers::TAG_UI_LAYER_MENU,
    UMyMenuWidget::StaticClass()
);
```

**Blueprint:**
Use the `Push Content to Layer for Player` node, selecting a `ForgeUI.Layer.*` gameplay tag.

**AngelScript:**
```angelscript
UCommonUIExtensions::PushContentToLayer_ForPlayer(
    LocalPlayer,
    n"ForgeUI.Layer.Menu",
    MyWidgetClass
);
```

## Base Classes

| Class | Extends | Purpose |
|-------|---------|---------|
| `UForgeUIPolicy` | `UGameUIPolicy` | UI policy with safe `GetWorld()` override (prevents editor CDO crash) |
| `UForgeManagerSubsystem` | `UGameUIManagerSubsystem` | Manages UI policy lifecycle and syncs HUD visibility with root layout |
| `UForgePrimaryGameLayout` | `UPrimaryGameLayout` | Root layout with 4 pre-registered layer stacks |
| `UForgeGameViewportClient` | `UCommonGameViewportClient` | Viewport client for CommonUI input routing |
| `UForgeActivatableWidget` | `UCommonActivatableWidget` | Activatable widget with simplified input mode config (Default/Game/Menu/GameAndMenu) |
| `UForgeButtonBase` | `UCommonButtonBase` | Button with text override, input-method-aware text refresh, and style callbacks |

## Dependencies

- **CommonUI** (Engine Plugin)
- **CommonGame** (Engine/Project Plugin)
- **CommonUser** (Engine/Project Plugin) — required by CommonGame
- **ModularGameplayActors** (Engine/Project Plugin) — required by CommonGame
- **GameplayTags** (Engine Module)
