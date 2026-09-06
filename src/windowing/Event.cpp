export module Dusk.windowing:Event;

namespace Dusk::windowing {

/**
 * Window events
 */
export enum class Event {
  MouseDown,
  MouseUp,
};

/**
 * Mouse buttons
 */
export enum class MouseButton { Left, Right, Middle };

}  // namespace Dusk::windowing
