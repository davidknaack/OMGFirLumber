#!/bin/bash
export GOPATH=$PWD
export PATH=$PATH:$GOPATH/bin

go get github.com/gorilla/websocket
go get github.com/go-martini/martini
go get github.com/martini-contrib/render
go get github.com/martini-contrib/sessions
go get github.com/tarm/goserial
go get github.com/codegangsta/gin
go get github.com/jmoiron/sqlx
