package main

import (
	"github.com/go-martini/martini"
	"github.com/jmoiron/sqlx"
	"github.com/martini-contrib/render"
	"github.com/martini-contrib/sessions"

	"log"
	"net/http"

	"controllers"
)

type User struct {
	User string
}

func indexGetHandler(ren render.Render) {
	ren.HTML(200, "index", nil)
}

func requireLogin(rw http.ResponseWriter, req *http.Request, s sessions.Session, db *sqlx.DB, c martini.Context) {
	username := s.Get("username")
	if username == nil {
		http.Redirect(rw, req, "/adminlogin", http.StatusFound)
		return
	}

	user := User{username.(string)}
	c.Map(user)
}

func loginGetHandler(ren render.Render) {
	ren.HTML(200, "adminlogin", nil)
}

func loginPostHandler(rw http.ResponseWriter, req *http.Request, s sessions.Session) (int, string) {
	user, pass := req.FormValue("user"), req.FormValue("pass")
	if user == "admin" && pass == "123" {
		s.Set("username", user)
		http.Redirect(rw, req, "/", http.StatusFound)
		return 200, "OK"
	}

	log.Printf("Failed login attempt: %v", req)
	return 401, "Not Authorized"
}

func logoutGetHandler(rw http.ResponseWriter, req *http.Request, s sessions.Session) {
	s.Delete("username")
	http.Redirect(rw, req, "/", http.StatusFound)
}

func main() {
	log.SetFlags(log.Lshortfile | log.Ldate | log.Ltime)
	log.Printf("Starting")

	m := martini.Classic()
	store := sessions.NewCookieStore([]byte("firlumberweb store auth"))
	m.Use(sessions.Sessions("firlumber", store))
	//m.Map(sqlx.NewDb(authdb.GetAuthDB(), "mysql"))
	m.Use(render.Renderer(render.Options{Layout: "layout"}))

	log.Printf("Registering endpoints")
	m.Get("/adminlogout", logoutGetHandler)
	m.Post("/adminlogin", loginPostHandler)
	m.Get("/adminlogin", loginGetHandler)
	m.Get("/", indexGetHandler)
	m.Get("/ws", controllers.WSGet)

	log.Printf("Listening")
	http.ListenAndServe(":8000", m)
}
