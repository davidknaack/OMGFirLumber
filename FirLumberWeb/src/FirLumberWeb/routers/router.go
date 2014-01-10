package routers

import (
	"FirLumberWeb/controllers"
	"github.com/astaxie/beego"
)

func init() {
    beego.Router("/", &controllers.MainController{})
	beego.Router("/ws", &controllers.WSController{})
}
