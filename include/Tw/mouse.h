
#ifndef _TW_MOUSE_H
#define _TW_MOUSE_H


/* mouse events stuff */

#define BUTTON_N_MAX		5 /* number of supported mouse buttons */

#define BUTTON_BITSHIFT		0
#define BUTTON_			((udat)1 << BUTTON_BITSHIFT)
#define BUTTON_CODE(n)		(BUTTON_ + (n)) /* n is 0,1,2,3,4 */
#define BUTTON_LEFT		BUTTON_CODE(0)
#define BUTTON_MIDDLE		BUTTON_CODE(1)
#define BUTTON_RIGHT		BUTTON_CODE(2)
#define BUTTON_WHEEL_REV	BUTTON_CODE(3)
#define BUTTON_WHEEL_FWD	BUTTON_CODE(4)
#define BUTTON_ANY		(BUTTON_LEFT|BUTTON_MIDDLE|BUTTON_RIGHT|BUTTON_WHEEL_REV|BUTTON_WHEEL_FWD)

#define HOLD_BITSHIFT		3
#define HOLD_			((udat)1 << HOLD_BITSHIFT)
#define HOLD_CODE(n)		(HOLD_ << (n)) /* n is 0,1,2,3,4 */
#define HOLD_LEFT		HOLD_CODE(0)
#define HOLD_MIDDLE		HOLD_CODE(1)
#define HOLD_RIGHT		HOLD_CODE(2)
#define HOLD_WHEEL_REV		HOLD_CODE(3)
#define HOLD_WHEEL_FWD		HOLD_CODE(4)
#define HOLD_ANY		(HOLD_LEFT|HOLD_MIDDLE|HOLD_RIGHT|HOLD_WHEEL_REV|HOLD_WHEEL_FWD)

#define MOVE_			((udat)0x00)
#define MOVE_MOUSE		MOVE_
#define MOVE_ANY		MOVE_MOUSE


#define PRESS_BITSHIFT		8
#define PRESS_			((udat)1 << PRESS_BITSHIFT)
#define PRESS_CODE(n)		(PRESS_ | BUTTON_CODE(n)) /* n is 0,1,2,3,4 */
#define PRESS_LEFT		PRESS_CODE(0)
#define PRESS_MIDDLE		PRESS_CODE(1)
#define PRESS_RIGHT		PRESS_CODE(2)
#define PRESS_WHEEL_REV		PRESS_CODE(3)
#define PRESS_WHEEL_FWD		PRESS_CODE(4)
#define PRESS_ANY		(PRESS_LEFT|PRESS_MIDDLE|PRESS_RIGHT|PRESS_WHEEL_REV|PRESS_WHEEL_FWD)

#define RELEASE_BITSHIFT	9
#define RELEASE_		((udat)1 << RELEASE_BITSHIFT)
#define RELEASE_CODE(n)		(RELEASE_ | BUTTON_CODE(n)) /* n is 0,1,2,3,4 */
#define RELEASE_LEFT		RELEASE_CODE(0)
#define RELEASE_MIDDLE		RELEASE_CODE(1)
#define RELEASE_RIGHT		RELEASE_CODE(2)
#define RELEASE_WHEEL_REV	RELEASE_CODE(3)
#define RELEASE_WHEEL_FWD	RELEASE_CODE(4)
#define RELEASE_ANY		(RELEASE_LEFT|RELEASE_MIDDLE|RELEASE_RIGHT|RELEASE_WHEEL_REV|RELEASE_WHEEL_FWD)


#define MOUSE_ACTION_MASK	(MOVE_ | PRESS_ | RELEASE_)
#define MOUSE_ACTION_ANY	(MOVE_ANY | PRESS_ANY | RELEASE_ANY)
#define MOUSE_ANY		(MOVE_ANY | PRESS_ANY | RELEASE_ANY | HOLD_ANY)

#define MOUSE_CODE_MAX		(RELEASE_CODE(BUTTON_N_MAX) | (HOLD_ANY & ~HOLD_CODE(BUTTON_N_MAX)))

#define isMOVE(code)		(((code) & MOUSE_ACTION_MASK) == MOVE_)
#define isPRESS(code)		(((code) & MOUSE_ACTION_MASK) == PRESS_)
#define isRELEASE(code)		(((code) & MOUSE_ACTION_MASK) == RELEASE_)

#define isSINGLE_MOVE(code)	(isMOVE(code) && ((code) == (MOVE_|HOLD_LEFT) || \
						  (code) == (MOVE_|HOLD_MIDDLE) || \
						  (code) == (MOVE_|HOLD_RIGHT) || \
						  (code) == (MOVE_|HOLD_WHEEL_REV) || \
						  (code) == (MOVE_|HOLD_WHEEL_FWD)))
#define isSINGLE_PRESS(code)	(isPRESS(code) && !((code) & HOLD_ANY))
#define isSINGLE_RELEASE(code)	(isRELEASE(code) && !((code) & HOLD_ANY))

/*
 * These macros can be used only for proper mouse codes.
 * The button numbers are: 0 (Left), 1 (Middle), 2 (Right), 3 (Wheel Up), 4 (Wheel Down)
 */

/* if (code & HOLD_ANY) then HOLD_N(n) returns the lowest button # pressed in code */
#define HOLD_N(code)	(ffs(((code) & HOLD_ANY) >> HOLD_BITSHIFT) - 1)

/*
 * if (isPRESS(code) || isRELEASE(code)) then BUTTON_N(n) returns
 * the button # activated (pressed or released) in code
 */
#define BUTTON_N(code)	(((code) & BUTTON_ANY) - BUTTON_CODE(0))
#define PRESS_N(code)	BUTTON_N(code)
#define RELEASE_N(code)	BUTTON_N(code)

/* if (isSINGLE_MOVE(code)) then MOVE_N(n) return the button # pressed in code */
#define MOVE_N(code)	HOLD_N(code)



#endif /* _TW_MOUSE_H */

