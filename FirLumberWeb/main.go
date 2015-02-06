package main

import (
	"github.com/go-martini/martini"
	"github.com/martini-contrib/render"
	"github.com/martini-contrib/sessions"

	"log"
	"net/http"

	"controllers"
)

func IndexGetHandler(ren render.Render) {
	ren.HTML(200, "index", nil)
}

func main() {
	log.SetFlags(log.Lshortfile | log.Ldate | log.Ltime)
	log.Printf("Starting")

	m := martini.Classic()
	store := sessions.NewCookieStore([]byte("firlumberweb store auth"))
	m.Use(sessions.Sessions("firlumber", store))
	//m.Map(sqlx.NewDb(authdb.GetAuthDB(), "mysql"))
	m.Use(render.Renderer())
	//render.Options{ Layout: "layout", }))

	log.Printf("Registering endpoints")
	m.Get("/", IndexGetHandler)
	m.Get("/ws", controllers.WSGet)

	log.Printf("Listening")
	http.ListenAndServe(":8000", m)
}
