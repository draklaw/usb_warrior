/*
 *  Copyright (C) 2015 the authors
 *
 *  This file is part of usb_warrior.
 *
 *  usb_warrior is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  usb_warrior is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with usb_warrior.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _UW_SCENE_H_
#define _UW_SCENE_H_


#include <memory>
#include <vector>
#include <unordered_map>
#include <map>
#include <typeinfo>
#include <typeindex>

#include "utils.h"
#include "game.h"
#include "game_object.h"
#include "component.h"


// A temporary constant used to pre-allocate memory
#define SCENE_ARRAYS_MAX_SIZE 1024


typedef std::unordered_map<std::string,std::string> EntityData;
class MainState;
class Level;
class TileMap;
class InputManager;
class Input;


class Scene {
public:
	typedef void (*Command)(Scene*, unsigned, const char**);

public:
	Scene(Game* game, MainState* state);

	inline Game*         game()   { return _game; }
	// Temporary / should not be usefull.
	inline MainState*    state()  { return _state; }
	inline InputManager* inputs() { return _inputs.get(); }

	// Temporary logic / to be replaced when object adress may change.
	inline GameObject* get(GameObject* obj) { return obj; }
	GameObject* getByName(const std::string& name);

	GameObject* addObject(const char* name = nullptr);

	/// \brief Construct a component of type T in-place, forwarding args to its
	/// constructor.
	template < typename T, typename... Args >
	T* addComponent(GameObject* obj, Args... args);

	/// \brief Returns an iterator range over the components of type T.
	///
	/// \todo Use iterators that skip destroyed elements.
//	template < typename T >
//	IterRange<T> iter();
	template < typename T, typename F >
	void forEachComponent(const F& func);

	void loadLevel(const char* filename);

	GameObject* createSpriteObject(const EntityData& data, const TileMap& tileMap);
	GameObject* createPlayer      (const EntityData& data);
	GameObject* createTrigger     (const EntityData& data);
	GameObject* createBotStatic   (const EntityData& data);
	GameObject* createWall        (const EntityData& data);

	void clear();

	inline bool debug() const { return _debugView; }
	void setDebug(bool debug);

	inline Level* level() { return _level.get(); }

	void addCommand(const char* action, Command cmd);
	void exec(const char* cmd);

	void beginUpdate();

	void beginRender();
	void endRender();
	void render(double interp, Boxf viewBox, Boxf screenBox);
	void renderLevelLayer(unsigned layer, Boxf viewBox, Boxf screenBox);

	inline Game* game() const { return _game; }

private:
	// Note: even if these types are std::vector, they are intended to be
	// of fixed size to avoid reallocation that would invalidate pointers.
	//
	// This is a temporary solution until we implement a container type that
	// allocates objects by (big) blocks to ensure locality without
	// invalidating pointers.
	typedef std::vector<GameObject> ObjectArray;

	// Map a type to an array (see above) of element of the specified type.

	// TODO: Implement this with dense storage (see commented code)
	typedef std::unique_ptr<Component>      CompPtr;
	typedef std::vector<CompPtr>            CompVector;
	typedef std::unordered_map<std::type_index, CompVector>  CompMap;

//	typedef std::vector<std::uint8_t> ByteVector;
//	typedef std::unordered_map<std::type_index, ByteVector> CompArrayMap;

	typedef std::unordered_map<std::string, GameObject*> NameObjectMap;
	typedef std::unordered_map<std::string, Command>     CommandMap;

private:
	Game*         _game;
	MainState*    _state;

	bool          _debugView;

	std::unique_ptr<Level>  _level;

	CommandMap    _commandMap;

	std::unique_ptr<InputManager>  _inputs;

	std::size_t   _objectCounter;
	ObjectArray   _objects;
	NameObjectMap _objectsByName;
	CompMap       _compsMap;
};


//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//


template < typename T, typename... Args >
T* Scene::addComponent(GameObject* obj, Args... args) {
	if(obj->getComponent<T>()) {
		_game->warning("Object \"", obj->name(), "\" has already a component of type \"",
		               typeid(T).name(), "\". Do nothing.");
		return obj->getComponent<T>();
	}

	// More magic
	std::type_index compIndex(typeid(T));
	auto& compsVector = _compsMap[compIndex];
	compsVector.emplace_back(new T(this, obj, std::forward<Args>(args)...));
	T* comp = static_cast<T*>(compsVector.back().get());
	obj->_getComponent<T>() = comp;
	return comp;
}

//template < typename T >
//IterRange<T> Scene::iter() {
//	auto& comps = _compsMap[std::type_info(typeid(T))];
//	return IterRange<T>(comps.begin(), comps.end());
//}


template < typename T, typename F >
inline void Scene::forEachComponent(const F& func) {
	auto& comps = _compsMap[std::type_index(typeid(T))];
	for(CompPtr& comp: comps) {
		func(static_cast<T*>(comp.get()));
	}
}


#endif
