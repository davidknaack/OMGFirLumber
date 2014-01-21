/*
WebSocket message framing.

All messages are converted to JSON. Every message has a 'Type' field.
 */
package controllers

import "time"

// Tag interface for doc purposes. In all other code you could just
// as well replace this type with 'interface {}' and it'd work the
// same.
type WSMessage interface{}

// Message types
const (
	RACE_START = "RaceStart"
	TRAP_TIME  = "TrapTime"
	LANE_TIME  = "LaneTime"
	RACE_END   = "RaceEnd"
)

type RaceStart struct {
	Type      string
}

type TrapTime struct {
	Type     string
	Time     time.Duration
}

type LaneTime struct {
	Type     string
	Lane     string
	Time     time.Duration
}

type RaceEnd struct {
	Type    string
}
