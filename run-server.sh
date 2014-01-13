#!/bin/bash
export GOPATH="$(dirname "$(pwd)/$0")"/FirLumberWeb

set -x
cd "$GOPATH/src/FirLumberWeb"
"$GOPATH/bin/bee" run FirLumberWeb
