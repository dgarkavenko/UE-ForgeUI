# ForgeryUI

A standalone, Lyra-style 4-layer UI stack plugin for Unreal Engine 5. Built on top of CommonUI and CommonGame, ForgeryUI provides a ready-to-use UI layer architecture: drop it into any UE5 project, set the policy in config, and get a working layered UI stack system with HUD visibility sync and input mode management.

## Layer Architecture

```
┌─────────────────────────────────────────────┐
│  Modal Layer (ForgeryUI.Layer.Modal)        │  <- Top: dialogs, confirmations
├─────────────────────────────────────────────┤
│  Menu Layer (ForgeryUI.Layer.Menu)          │  <- Main menu, frontend screens
├─────────────────────────────────────────────┤
│  GameMenu Layer (ForgeryUI.Layer.GameMenu)  │  <- Pause, inventory, settings
├─────────────────────────────────────────────┤
│  Game Layer (ForgeryUI.Layer.Game)          │  <- Bottom: HUD, in-game widgets
└─────────────────────────────────────────────┘
```

Each layer is a `UCommonActivatableWidgetStack`. Widgets pushed to higher layers receive input priority and can suspend layers below them.

## Quick Start

### 1. Enable Plugins

Copy `Plugins/ForgeryUI/` into your project. The following plugins must also be present and enabled:

- **CommonUI** (Engine plugin)
- **CommonGame** — provides `UGameUIManagerSubsystem`, `UGameUIPolicy`, `UPrimaryGameLayout`
- **CommonUser** — required by CommonGame
- **ModularGameplayActors** — provides `AModularPlayerController` (base for `ACommonPlayerController`)

Add to your game module's `Build.cs`:

```csharp
PublicDependencyModuleNames.Add("ForgeryUI");
```

### 2. Set Up Project Base Classes

CommonGame requires your project to use its base classes for GameInstance, LocalPlayer, and PlayerController. Create minimal subclasses if you don't already have them:

**GameInstance** — must extend `UCommonGameInstance`: | GameInstanceClass in config
**LocalPlayer** — must extend `UCommonLocalPlayer`: | Local Player Class

**PlayerController** — must extend `ACommonPlayerController` (which extends `AModularPlayerController`). This is required because `ACommonPlayerController` broadcasts the `OnPlayerControllerSet` delegate to `UCommonLocalPlayer`, which triggers the UI layout creation chain


### 3. Configure
In `DefaultEngine.ini`, point the engine to ForgeryUI's viewport client:

```ini
[/Script/Engine.Engine]
GameViewportClientClassName="/Script/ForgeryUI.FUGameViewportClient"
```

In `DefaultGame.ini`
```ini
[/Script/ForgeryUI.FUManagerSubsystem]
DefaultUIPolicyClass=/ForgeryUI/Framework/BP_FUUIPolicy.BP_FUUIPolicy_C
```


### 6. Push Widgets

**C++:**
```cpp
#include "Framework/FULayers.h"
#include "CommonUIExtensions.h"

UCommonUIExtensions::PushContentToLayer_ForPlayer(
    LocalPlayer,
    FULayers::TAG_UI_LAYER_MENU,
    UMyMenuWidget::StaticClass()
);
```

**Blueprint:**
Use the `Push Content to Layer for Player` node, selecting a `ForgeryUI.Layer.*` gameplay tag.

**AngelScript:**
```angelscript
UCommonUIExtensions::PushContentToLayer_ForPlayer(
    LocalPlayer,
    n"ForgeryUI.Layer.Menu",
    MyWidgetClass
);
```

## Base Classes

| Class | Extends | Purpose |
|-------|---------|---------|
| `UFUUIPolicy` | `UGameUIPolicy` | UI policy with safe `GetWorld()` override (prevents editor CDO crash) |
| `UFUManagerSubsystem` | `UGameUIManagerSubsystem` | Manages UI policy lifecycle and syncs HUD visibility with root layout |
| `UFUPrimaryGameLayout` | `UPrimaryGameLayout` | Root layout with 4 pre-registered layer stacks |
| `UFUGameViewportClient` | `UCommonGameViewportClient` | Viewport client for CommonUI input routing |
| `UFUActivatableWidget` | `UCommonActivatableWidget` | Activatable widget with simplified input mode config (Default/Game/Menu/GameAndMenu) |
| `UFUButtonBase` | `UCommonButtonBase` | Button with text override, input-method-aware text refresh, and style callbacks |

## Dependencies

- **CommonUI** (Engine Plugin)
- **CommonGame** (Engine/Project Plugin)
- **CommonUser** (Engine/Project Plugin) — required by CommonGame
- **ModularGameplayActors** (Engine/Project Plugin) — required by CommonGame
- **GameplayTags** (Engine Module)
