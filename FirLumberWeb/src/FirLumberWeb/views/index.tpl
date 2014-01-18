<!DOCTYPE html>

<html>
  	<head>
    	<title>FirLumber Rally</title>
    	<meta http-equiv="Content-Type" content="text/html; charset=utf-8">

		<script src="//ajax.googleapis.com/ajax/libs/jquery/2.0.3/jquery.min.js"></script>
		<script type="text/javascript">
			$(function() {

			var conn;
			var msg = $("#msg");
			var log = $("#log");
			var trap = $("#trap");

			function appendLog(msg) {
				var d = log[0]
				var doScroll = d.scrollTop == d.scrollHeight - d.clientHeight;
				msg.appendTo(log)
				if (doScroll) {
					d.scrollTop = d.scrollHeight - d.clientHeight;
				}
			}

			function appendSpeedTrap(msg) {
				trap.prepend("<div>"+msg+"</div>");
			}

			if (window["WebSocket"]) {
				conn = new WebSocket("ws://{{.host}}/ws");
				conn.onclose = function(evt) {
					appendLog($("<div><b>Connection closed.</b></div>"));
				}
				conn.onmessage = function(evt) {
                    var msg = $.parseJSON(evt.data);
                    console.log(msg);
                    if (msg.Type == "TrapTime") {
                        appendSpeedTrap("Trap Time: " + msg.Time);
                    } else if (msg.Type == "RaceStart") {
                        appendLog($("<div/>").text("Race Start"))
                    } else if (msg.Type == "RaceEnd") {
                        appendLog($("<div/>").text("Race End"))
                    } else if (msg.Type == "LaneTime") {
                        appendLog($("<div/>").text("Lane Time for " + msg.Lane + ": " + msg.Time));
                    }
				}
			} else {
				appendLog($("<div><b>Your browser does not support WebSockets.</b></div>"))
			}
			});
		</script>
	
		<style type="text/css">
			body {
				margin: 0px;
				font-family: "Helvetica Neue",Helvetica,Arial,sans-serif;
				font-size: 14px;
				line-height: 20px;
				color: rgb(51, 51, 51);
				background-color: rgb(255, 255, 255);
			}

			.hero-unit {
				padding: 20px;
				margin-bottom: 30px;
				border-radius: 6px 6px 6px 6px;
			}

			.container {
				width: 940px;
				margin-right: auto;
				margin-left: auto;
			}

			.row {
				margin-left: -20px;
			}

			h1 {
				margin: 10px 0px;
				font-family: inherit;
				font-weight: bold;
				text-rendering: optimizelegibility;
			}

			.hero-unit h1 {
				margin-bottom: 0px;
				font-size: 60px;
				line-height: 1;
				letter-spacing: -1px;
				color: inherit;
			}

			.description {
				padding-top: 5px;
				padding-left: 5px;
				font-size: 18px;
				font-weight: 200;
				line-height: 30px;
				color: inherit;
			}

			p {
				margin: 0px 0px 10px;
			}
						
			#log {
				background: blue;
				margin: 0;
				padding: 0.5em 0.5em 0.5em 0.5em;
				overflow: auto;
			}

			#trap {
				background: yellow;
				margin: 0;
				padding: 0.5em 0.5em 0.5em 0.5em;
				overflow: auto;
			}
			
			#form {
				padding: 0 0.5em 0 0.5em;
				margin: 0;
				position: absolute;
				bottom: 1em;
				left: 0px;
				width: 100%;
				overflow: hidden;
			}
		</style>
	</head>
  	
  	<body>
  		<header class="hero-unit" style="background-color:#A9F16C">
			<div class="container">
			<div class="row">
			  <div class="hero-text">
			    <h1>FirLumber Rally</h1>
			  </div>
			</div>
			</div>
		</header>
		<div id="trap"></div>
		<div id="log"></div>
	</body>
</html>
