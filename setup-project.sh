#!/bin/bash
export GOPATH="$(dirname "$(pwd)/$0")"/FirLumberWeb

set -ex
git submodule init
git submodule update

go install github.com/beego/bee
go install github.com/smartystreets/goconvey
