# Todo list

## Code

### Game core
- Tile z ordering
- SoundComponent for localized sounds ?
- Better object management (keep them in a dense array)
  * Problem is to deal with array realloc that invalidate pointers
- Better component management (same, + component access)
- Component properties to simplify GameObject initialization.
  * Could be double-buffered for interpolable values
- Game-event system to implement level logic
- Scene / MainState responsabilities are somewhat similar
  * Add an Object/Component manager that deals with Objects and component
  * Have component-related functionalities separated (rendering...)
  * Game states should drive it all
- Make font loading load the image too.

### Gameplay
- Move-enable bots
- Fighter bots
- Teleporters
- Magnetic shafts
- Dialogs - Add a story

## Level-Design
- Implement logic on existing maps
- More maps

## Graphism
- Various enhancements

## Sound
- Various enhancements
