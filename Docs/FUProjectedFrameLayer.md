# UFUProjectedFrameLayer

Deferred batch renderer for projected frames.

`UFUProjectedWidget_Frame` is the right path for one hovered or selected target. For 10-20+ simultaneous frames, use one future `UFUProjectedFrameLayer` widget instead of spawning one projected widget per target.

The layer should own a `TArray<FFUProjectedFrameTarget>`. Each target stores a weak source component, item index, target mode, and frame settings. During one paint/update pass, the layer resolves actor, component, or ISM/HISM instance bounds for every visible target using the same `FFUProjectedWidgetLayoutContext`, projects them into the marker canvas, and draws all rectangles with `FSlateDrawElement::MakeLines`.

Expected benefit: one widget, one layout context, one paint pass, and no per-target canvas-slot updates. This is better for enemy groups, sensors, debug overlays, and multi-selection, but it is unnecessary for the current single hover frame.
