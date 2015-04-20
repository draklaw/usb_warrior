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

#include <iostream>

#include "move_component.h"

#include "../main_state.h"

// FIXME: Should be dynamic.
#define TILESIZE 16.0

// THE MOVEMENT ALGORITHM (so far) :

/* walking :
- sets control speed to base speed, plus a momentum bonus
- increases momentum up to a maximum
*/
#define WALK_BASE_SPEED 0.3
#define MAX_MOMENTUM 60u
#define MOMENTUM_BONUS(m) (WALK_BASE_SPEED * (1.0 + m /(float) MAX_MOMENTUM))

/* not walking :
- sharply decreases momentum
*/
#define MOMENTUM_LOSS 10u

/* sprinting :
- multiplies control speed and further increases momentum
*/
#define DASH_FACTOR  3.0
#define DASH_BUILDUP 5u

/* on the ground :
- movement speed slows exponentially
- // TODO: if mspeed is below some threshold, stop now
- jumping gives control speed a kick up and starts _airTime
*/
#define GROUND_DRAG_FACTOR 0.8
#define BASE_JUMP_SPEED 5.0

/* in the air :
- _airTime increases (duh)
- jumping gives further impulsion (quickly vanishes as _airTime increases)
- (lack of) air control impedes horizontal control speed
- gravity accelerates down
- movement speed is slowed and capped to terminal velocity
*/
#define BONUS_JUMP_SPEED(t) (BASE_JUMP_SPEED / (t+1.0))
#define MAX_JUMP_AIRTIME 10u
#define AIR_CONTROL_FACTOR 1.0/5.0
#define GRAVITY_ACCEL 0.6
#define AIR_DRAG_FACTOR 0.99
#define TERMINAL_VELOCITY 20.0

/* control switches are reset

remaining movement speed is supplemented with control speed
the resulting movement speed takes you further
*/

// THE COLLISION ALGORITHM (draft) :
//TODO: Check if collision occurs in two opposite directions simultaneously.
// If so, panic in an appropriate fashion.

/* iterate until stability up to some limit :
for each intersection with a solid tile :
compute intersection surface
*/
#define MAX_PASSES 20u

//FIXME: Backtracking should occur immediately if ANY intersection is too big.
/* instersection is !reasonable (> 1/2 tile) :
- mark backtracking
- backtrack by a factor of current _mSpeed
- immediately start another pass
*/
#define UNREASONABLE_COLLISION ((TILESIZE * TILESIZE) / 2.0)
#define BACKTRACK_FACTOR 0.8

/* collision (any amount) occured below :
*/

/* intersection is !negligible :
- check collision axis
- horizontal collision :
> - bump along x axis by half the horizontal amount minus half a delta
> - neutralize horizontal mspeed
- vertical collision :
> - bump along y axis by half the vertical amount minus half a delta
> - neutralize vertical mspeed
> - reset _airTime and unflag dropping
- mark unstability
- start another pass
*/
#define SIGNIFICANT_COLLISION (0.5 * TILESIZE)
#define Y_X_VERTICALITY_FACTOR 2.0
#define COLLISION_DELTA (SIGNIFICANT_COLLISION / TILESIZE)

/* finally if backtracking :
- bounce (invert and cut down movement speed)
- stop backtracking
*/
#define BOUNCE_FACTOR -0.3

// if dropping from the ground, start _airTime

MoveComponent::MoveComponent(GameObject* obj)
    : LogicComponent(obj) {
	_puppet =  &_obj->geom(CURR_UP);
	_mSpeed = Vec2(0,0);
	_momentum = 0;
	_airTime = 0;
	_walking = DOWN;
	_running = false;
	_jumping = false;
}

void MoveComponent::setSpeed() {
	Vec2 cs;
	
	switch (_walking) {
	case LEFT:
		cs = Vec2(-WALK_BASE_SPEED - MOMENTUM_BONUS(_momentum),0);
		_momentum++;
		break;
	case RIGHT:
		cs = Vec2( WALK_BASE_SPEED + MOMENTUM_BONUS(_momentum),0);
		_momentum++;
		break;
	default:
		cs = Vec2(0,0);
		_running = false;
		_momentum -= MOMENTUM_LOSS;
	}
	
	if (_running)
	{
		cs *= DASH_FACTOR;
		_momentum += DASH_BUILDUP;
	}
	_momentum = std::min(_momentum,MAX_MOMENTUM);
	
	if (!_airTime)
	{
		_mSpeed *= GROUND_DRAG_FACTOR;
		
		if (_jumping)
		{
			cs += Vec2(0,-BASE_JUMP_SPEED);
			_airTime++;
		}
	}
	else
	{
		_airTime++;
		
		if (_jumping && _airTime < MAX_JUMP_AIRTIME)
			cs += Vec2(0,-BONUS_JUMP_SPEED(_airTime));
		
		cs.x() *= AIR_CONTROL_FACTOR;
		
		_mSpeed += Vec2(0,GRAVITY_ACCEL);
		
		_mSpeed *= AIR_DRAG_FACTOR;
		
		// Please don't break my game (too much).
		while (_mSpeed.norm() > TERMINAL_VELOCITY)
			_mSpeed *= 0.8;
	}
	
	_walking = DOWN;
	_running = false;
	_jumping = false;
	
	_mSpeed += cs;
	_puppet->pos += _mSpeed;
}

void MoveComponent::collide() {
	bool backtrack = false, stable = false, dropping = true;
	unsigned nbPasses = 0;
	
	while (!stable && nbPasses < MAX_PASSES) {
		//TODO: replace 0 with i in l->nLayers and merge lists
		CollisionList intersections = _obj->scene()->level().collide(0, _puppet->worldBox());
		
		stable = true;
		
		for (Boxf crash:intersections)
		{
			float amount = crash.volume();
			
			if (amount >= UNREASONABLE_COLLISION)
			{
				backtrack = true;
				_puppet->pos -= _mSpeed * BACKTRACK_FACTOR;
				nbPasses++;
				break;
			}
			
			if (amount >= SIGNIFICANT_COLLISION)
			{
				if (crash.sizes().y() > crash.sizes().x() * Y_X_VERTICALITY_FACTOR)
				{ // vertical crashbox, horizontal collision
					if ((crash.min().x()+crash.max().x())/2.0 > _puppet->pos.x())
						_puppet->pos.x() -= crash.sizes().x()/2.0 - COLLISION_DELTA/2.0;
					else
						_puppet->pos.x() += crash.sizes().x()/2.0 - COLLISION_DELTA/2.0;
					_mSpeed.x() = 0;
				}
				else // horizontal crashbox, vertical collision
				{
					if ((crash.min().y()+crash.max().y())/2.0 > _puppet->pos.y())
						_puppet->pos.y() -= crash.sizes().y()/2.0 - COLLISION_DELTA/2.0;
					else
						_puppet->pos.y() += crash.sizes().y()/2.0 - COLLISION_DELTA/2.0;
					_mSpeed.y() = 0;
					
					_airTime = 0;
					dropping = false;
				}
				
				stable = false;
				break;
			}
		}
		
		if (backtrack)
		{
			_mSpeed *= BOUNCE_FACTOR;
			backtrack = false;
		}
		
		nbPasses++;
	}
	
	if (!_airTime && dropping)
		_airTime = 1;
}


void MoveComponent::update() {
	setSpeed();
	collide();
}

void MoveComponent::jump() {
	_jumping = true;
}

void MoveComponent::walk(Direction d) {
	_walking = d;
}

void MoveComponent::sprint() {
	_running = true;
}
