package controllers

import (
	"bufio"
	"encoding/json"
	"fmt"
	"github.com/gorilla/websocket"
	"github.com/tarm/goserial"
	"log"
	"math/rand"
	"net/http"
	"strconv"
	"time"
)

const (
	// Time allowed to write a message to the client.
	writeWait = 10 * time.Second

	// Time allowed to read the next message from the client.
	readWait = 60 * time.Second

	// Send pings to client with this period. Must be less than readWait.
	pingPeriod = (readWait * 9) / 10

	// Maximum message size allowed from client.
	maxMessageSize = 512
)

func init() {
	rand.Seed(time.Now().UTC().UnixNano())
	go h.run()
	go fakemonitorSerial()
}

func monitorSerial() {
	c0 := &serial.Config{Name: "COM4", Baud: 115200}

	s, err := serial.OpenPort(c0)
	if err != nil {
		log.Fatal(err)
	}

	reader := bufio.NewReader(s)
	for {
		buf, err := reader.ReadBytes('\x0a')
		if err != nil {
			panic(err)
		}
		handleSerialLine(string(buf[:]))
	}
}

func fakemonitorSerial() {
	go func() {
		for i := 0; ; i++ {
			time.Sleep(5000 * time.Millisecond)

			t := rand.Intn(1000000)
			a := t + 1000000
			b := a + 250000

			handleSerialLine("s" + "\r")

			time.Sleep(time.Duration(t/1000) * time.Millisecond)
			handleSerialLine("t:" + strconv.Itoa(t) + "\r")

			time.Sleep(1000 * time.Millisecond)
			handleSerialLine("a:" + strconv.Itoa(a) + "\r")

			time.Sleep(250 * time.Millisecond)
			handleSerialLine("b:" + strconv.Itoa(b) + "\r")

			handleSerialLine("e" + "\r")
		}
	}()
}

func getDuration(s string) time.Duration {
	t, e := strconv.Atoi(s)
	if e != nil {
		log.Println("time format error: " + s)
		return time.Duration(0)
	}
	//log.Println( "time format converted: " + s)
	return time.Duration(t) * time.Microsecond
}

func handleSerialLine(inp string) {
	//log.Println(inp)
	inp = inp[:len(inp)-2]
	if len(inp) == 0 {
		return
	}

	switch string(inp[0]) {
	case "s":
		h.broadcast <- RaceStart{Type: RACE_START}
	case "t":
		h.broadcast <- TrapTime{Type: TRAP_TIME, Time: getDuration(inp[2:])}
	case "a", "b":
		h.broadcast <- LaneTime{Type: LANE_TIME, Lane: string(inp[0]), Time: getDuration(inp[2:])}
	case "e":
		h.broadcast <- RaceEnd{Type: RACE_END}
	}
}

// connection is an middleman between the websocket connection and the hub.
type connection struct {
	username string

	// The websocket connection.
	ws *websocket.Conn

	// Buffered channel of outbound messages.
	send chan []byte
}

// readPump pumps messages from the websocket connection to the hub.
func (c *connection) readPump() {
	log.Println("readPump start")
	defer func() {
		h.unregister <- c
		c.ws.Close()
	}()
	c.ws.SetReadLimit(maxMessageSize)
	c.ws.SetReadDeadline(time.Now().Add(readWait))
	c.ws.SetPongHandler(func(string) error { c.ws.SetReadDeadline(time.Now().Add(readWait)); return nil })
	for {
		_, message, err := c.ws.ReadMessage()
		if err != nil {
			log.Println("websocket read error")
			break
		}
		log.Println(fmt.Sprintf("websocket read: %s", string(message)))
		h.broadcast <- []byte(c.username + "_" + time.Now().Format("15:04:05") + ":" + string(message))
	}
	log.Println("readPump stop")
}

// write writes a message with the given opCode and payload.
func (c *connection) write(opCode int, payload []byte) error {
	c.ws.SetWriteDeadline(time.Now().Add(writeWait))
	return c.ws.WriteMessage(opCode, payload)
}

// writePump pumps messages from the hub to the websocket connection.
func (c *connection) writePump() {
	log.Println("write pump start")
	ticker := time.NewTicker(pingPeriod)
	defer func() {
		ticker.Stop()
		c.ws.Close()
	}()
	for {
		select {
		case message, ok := <-c.send:
			if !ok {
				log.Println("connection not ok, closing")
				c.write(websocket.CloseMessage, []byte{})
				return
			}
			log.Println(fmt.Sprintf("websocket send: %s", string(message)))
			if err := c.write(websocket.TextMessage, message); err != nil {
				return
			}
		case <-ticker.C:
			//c.write(websocket.OpText, []byte("ping"))
			if err := c.write(websocket.PingMessage, []byte{}); err != nil {
				log.Println("ticker ping err ")
				return
			}
		}
	}
	log.Println("write pump stop")
}

type hub struct {
	// Registered connections.
	connections map[*connection]bool

	// Inbound messages from the connections.
	broadcast chan WSMessage

	// Register requests from the connections.
	register chan *connection

	// Unregister requests from connections.
	unregister chan *connection
}

var h = &hub{
	broadcast:   make(chan WSMessage),
	register:    make(chan *connection, 1),
	unregister:  make(chan *connection, 1),
	connections: make(map[*connection]bool),
}

func (h *hub) run() {
	log.Println("hub run start")
	for {
		select {
		case c := <-h.register:
			h.connections[c] = true
		case c := <-h.unregister:
			delete(h.connections, c)
			close(c.send)
		case m := <-h.broadcast:
			log.Printf("hub.broadcast %v\n", m)
			jsonByteBuffer, err := json.Marshal(m)
			if err != nil {
				log.Printf("Unable to json.Marshal this: %#v\n", m)
				panic(err)
			}
			for c := range h.connections {
				select {
				case c.send <- jsonByteBuffer:
				default:
					close(c.send)
					delete(h.connections, c)
				}
			}
		}
	}
	log.Println("hub run stop")
}

func WSGet(w http.ResponseWriter, req *http.Request) {
	log.Printf("WSGet")
	ws, err := websocket.Upgrade(w, req, nil, 1024, 1024)
	if _, ok := err.(websocket.HandshakeError); ok {
		http.Error(w, "Not a websocket handshake", 400)
		return
	} else if err != nil {
	}

	c := &connection{send: make(chan []byte, 256), ws: ws, username: randomString(10)}
	h.register <- c
	go c.writePump()
	c.readPump()
}

func randomString(l int) string {
	bytes := make([]byte, l)
	for i := 0; i < l; i++ {
		bytes[i] = byte(randInt(65, 90))
	}
	return string(bytes)
}

func randInt(min int, max int) int {
	return min + rand.Intn(max-min)
}
