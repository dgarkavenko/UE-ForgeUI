# UFUProjectedFrameLayer

Deferred batch renderer for projected frames.

`UFUProjectedWidget_Frame` is the right path for one hovered or selected target. For 10-20+ simultaneous frames, use one future `UFUProjectedFrameLayer` widget instead of spawning one projected widget per target.

## UFUProjectedWidget_Frame slot contract

`UFUProjectedWidget_Frame` sets `bSelfManagedSlotLayout = true` (declared on the `UFUScreenProjectedWidget` base) and owns its own canvas-slot position/size and render transform — `UFUProjectedWidgetHost::NativeTick` skips its per-tick `SetRenderScale`/`SetAlignment`/`SetPosition`/`SetZOrder` writes for any widget with this flag set; z-order is applied once at registration instead. The frame widget updates its slot itself in `NativeOnProjectionLayoutUpdated`, skipping the `SetPosition`/`SetSize` call when the projected rect hasn't moved by more than 0.1px since the last update.

It remains a normal `Blueprintable UUserWidget` — the canvas root fills the projected rect exactly, so subclasses (Blueprint or AngelScript) are free to add child widgets (bound text, icons, etc.) anchored around or inside that root without fighting host-driven layout.

The layer should own a `TArray<FFUProjectedFrameTarget>`. Each target stores a weak source component, item index, target mode, and frame settings. During one paint/update pass, the layer resolves actor, component, or ISM/HISM instance bounds for every visible target using the same `FFUProjectedWidgetLayoutContext`, projects them into the marker canvas, and draws all rectangles with `FSlateDrawElement::MakeLines`.

Expected benefit: one widget, one layout context, one paint pass, and no per-target canvas-slot updates. This is better for enemy groups, sensors, debug overlays, and multi-selection, but it is unnecessary for the current single hover frame.
