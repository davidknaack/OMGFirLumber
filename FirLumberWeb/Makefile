.PHONY: dep package run clean db.init db.deploy db.destroy db.verify db.revert test test.integration

APP_NAME = firlumberweb

SQITCH_DIR=sqitch
SQITCH_CMD=SQITCH_CONFIG=$(SQITCH_DIR)/sqitch.conf sqitch --top-dir $(SQITCH_DIR)

BUILD_TIME := $(shell date +"%Y%m%d.%H%M%S")
LDFLAGS = -ldflags "-X main.buildTime $(BUILD_TIME)" 

EXEC_NAME= ./$(APP_NAME)
ifndef LOCAL_ARCHIVE
    LOCAL_ARCHIVE= $(EXEC_NAME)-$(BUILD_TIME).tar.gz
endif

$(EXEC_NAME):
	go build $(LDFLAGS) -o $(EXEC_NAME) main.go

$(LOCAL_ARCHIVE): $(EXEC_NAME)
	tar -cvzf $(LOCAL_ARCHIVE) $(EXEC_NAME) $(SQITCH_DIR) templates

package: $(LOCAL_ARCHIVE)

publish: package
	echo no publish infrastructure

dep:
	go get github.com/gorilla/websocket       
	go get github.com/go-martini/martini      
	go get github.com/martini-contrib/render  
	go get github.com/martini-contrib/sessions
	go get github.com/tarm/goserial           
	go get github.com/codegangsta/gin         
	go get github.com/jmoiron/sqlx            

check: test
	-go vet ./...
	golint ./...

test:
	go test -short ./...

clean:
	rm -f $(EXEC_NAME)
	rm -f $(EXEC_NAME)*.tar.gz

db.init:
	mysql --user=root < $(SQITCH_DIR)/dev_start.sql
	$(SQITCH_CMD) deploy

db.deploy:
	$(SQITCH_CMD) deploy

db.verify:
	$(SQITCH_CMD) verify

db.revert:
	$(SQITCH_CMD) revert

db.destroy:
	mysql --user=root < $(SQITCH_DIR)/dev_clear.sql

db.reset: db.destroy db.init
