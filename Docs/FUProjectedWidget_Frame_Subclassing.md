# UFUProjectedWidget_Frame — Subclassing Guide

How to add text, icons, and other elements around or inside the projected interaction frame by subclassing `UFUProjectedWidget_Frame` in AngelScript and binding widgets from a Widget Blueprint.

## The contract

`UFUProjectedWidgetHost` sizes the frame widget's canvas slot to the screen-projected bounding rect of its target every frame, and `NativePaint` draws the rectangle border *behind* the widget tree. The subclass's root is therefore a rectangle exactly matching the frame — everything added to the widget tree lays out relative to the frame for free, and draws on top of the border. See the slot contract section in `FUProjectedFrameLayer.md`.

## 1. AngelScript subclass

Declare the elements you want as bound widgets:

```angelscript
UCLASS(Abstract)
class UBSInteractionFrame : UFUProjectedWidget_Frame
{
	UPROPERTY(Meta = (BindWidget))
	UTextBlock NameLabel;

	UPROPERTY(Meta = (BindWidgetOptional))
	UTextBlock HintLabel;

	UPROPERTY(Meta = (BindWidgetOptional))
	UImage Icon;
}
```

`BindWidget` makes the Widget Blueprint fail to compile if the named element is missing; use `BindWidgetOptional` for elements a given skin may omit (and null-check before use).

## 2. Widget Blueprint

Create a Widget Blueprint reparented to the AngelScript class and assign it as the frame widget class at the registration site (e.g. `InteractionFrameWidgetClass` on the player controller).

Designer setup:

- **Root must be a CanvasPanel.** The widget itself is sized to the frame rect, so canvas anchors are normalized frame coordinates: anchor `(0,0)` is the frame's top-left corner, `(1,1)` bottom-right, `(0.5,0.5)` center.
- **Enable Size to Content** on each canvas child so labels keep their natural size while the frame stretches.
- **Leave Clipping = Inherit** (the default) on the widget and root canvas — otherwise elements placed outside the frame get clipped.
- Element names in the designer must match the AngelScript property names exactly.

### Positioning elements

Anchor picks the point on the frame; alignment picks the point on the element that sits there. Alignment pointing *into* the rect places the element inside; flipped *away* from the rect places it outside.

| Placement | Anchor | Alignment |
|---|---|---|
| Centered inside | (0.5, 0.5) | (0.5, 0.5) |
| Inside top-left corner | (0, 0) | (0, 0) |
| Centered above the frame | (0.5, 0) | (0.5, 1) |
| Centered below the frame | (0.5, 1) | (0.5, 0) |
| Right of the top-right corner | (1, 0) | (0, 0) |
| Hanging off bottom-right corner | (1, 1) | (0, 0) |

Add a small position offset for a gap beyond `FrameSettings.Padding` (the projected rect already includes the padding).

## 3. Driving content from script

`RegisterFrameWidget` returns the instance — cast it to the subclass:

```angelscript
UBSInteractionFrame Frame = Cast<UBSInteractionFrame>(
	UI::GetHUD().RegisterFrameWidget(Anchor, InteractionFrameWidgetClass));
Frame.NameLabel.SetText(TargetDisplayName);
```

For anything beyond a single label, prefer a setter method on the subclass (e.g. `SetTarget(FText Name, UTexture2D Icon)`) that also collapses the optional elements when unused.

## 4. Reacting to frame state

The `UFUScreenProjectedWidget` base exposes Blueprint events implementable with `UFUNCTION(BlueprintOverride)` in AngelScript:

- `OnDistanceStateChanged` — fires on Near/Normal/Far transitions; useful to fade or collapse labels at distance.
- `OnClampedChanged` — fires when the anchor enters/leaves the viewport clamp.
- `OnProjectionLayoutUpdated` — fires every layout tick. **Hot path** — keep any override trivial; prefer the change-driven events above.

## Caveats

- The widget is `HitTestInvisible` — nothing added can be interactive. Labels and icons only.
- The frame can shrink to `FrameSettings.MinSize` (default 12×12); inside-anchored elements will overlap on small targets. Text that must stay readable at any frame size belongs in the outside positions.
- For many simultaneous frames, per-target child widgets stop scaling — that is the future `UFUProjectedFrameLayer` batch path (see `FUProjectedFrameLayer.md`), where labels/icons become paint-time draw elements instead.
