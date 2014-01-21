<!DOCTYPE html>

<html>
<head>
  <title>FirLumber Rally</title>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8">

  <script src="//ajax.googleapis.com/ajax/libs/jquery/2.0.3/jquery.min.js"></script>
  <script type="text/javascript">

    $(window).ready(function() {
      // Thing I *swear* isn't a global variable.
      var conn;
      var times = {}; // per-lane times (determine winner)

      // Calibration constants
      var trap_dist = 10; // trap distance in cm
      var cmps_to_mph = 22369362.9; // cm -> mph constant
      var convert = trap_dist * cmps_to_mph; // convert / trap_ms -> mph


      // Wrapper in case person is using Firefox (sorry, no logs for you)
      var log = function(msg) {
        if (typeof window.console != 'undefined') {
          console.log(msg);
        }
      }

      // Hide error
      var hideError = function() {
        $('#error').text('').hide();
      }
      hideError();

      // Show error
      var showError = function(msg) {
        $('#error').text(msg).show();
      }

      // Snag templates
      var race_result_template = $('#race-result-template').detach().attr('id',null);
      var trap_result_template = $('#trap-result-template').detach().attr('id',null);

      // Bail if web browser sucks
      if (!window['WebSocket']) {
        showError('Error: Your browser does not support WebSockets');
        return;
      }

      // Handle connection errors / closes
      var connectionError = function(evt) {
        log("Connection lost");
        log(evt);
        showError('Connection to server lost.');

//        // Retry connection
//        if ( conn.state != 3 ) { // 3 = closed
//          conn.close();
//        }
//        setTimeout(openConnection, 3000);
      };

      var connectionEstablished = function(evt) {
        log("Connection established");
        hideError()
      };

      // Normal message receive routines
      var messageReceived = function(evt) {
        var msg = $.parseJSON(evt.data);
        // This breaks on
        log(msg);

        if (msg.Type == "TrapTime") {
          var trap_txt = "-";
          if ( msg.Time != 4294967295000 ) {
            trap_txt = (convert / msg.Time).toFixed(2) + " mph";
          }
          $('#trap-results .row:first-child').text(trap_txt);
        } else if (msg.Type == "RaceStart") {
          $('#trap-results').prepend(trap_result_template.clone());
          $('#race-results').prepend(race_result_template.clone());
          a_time = b_time = null;
        } else if (msg.Type == "RaceEnd") {
          if ( typeof times.a == 'undefined' ) return;
          if ( typeof times.b == 'undefined' ) return;
          if ( times.a == times.b ) return;
          var winner;
          if ( times.a < times.b ) winner = "a";
          if ( times.a > times.b ) winner = "b";
          $('#race-results .row:first-child').find('.' + winner).addClass("winner");
        } else if (msg.Type == "LaneTime") {
          times[msg.Lane] = msg.Time;
          if ( msg.Time == 4294967295000 ) {
			$('#race-results .row:first-child').find('.' + msg.Lane).text("DNF");
          } else {
			var time_text = (msg.Time/1000000000).toFixed(3) + " s";
			$('#race-results .row:first-child').find('.' + msg.Lane).text(time_text);
		  }
        }
      };

      var openConnection = function() {
        // Connect and set callbacks
        conn = new WebSocket("ws://{{.host}}/ws");
        conn.onclose = connectionError;
        conn.onerror = connectionError;
        conn.onmessage = messageReceived;
        conn.onopen = connectionEstablished;
      };

      openConnection();
    });
  </script>

  <style type="text/css">
    body {
      margin: 0px;
      font-family: "Helvetica Neue",Helvetica,Arial,sans-serif;
      color: #3a3a3a;
      font-size: 30px; /* <-- All sizes are keyed off this font size */

      min-width: 1200px;
    }

    #error {
      margin: 1em;
      border-radius: 1em;
      border: 2px solid #3a3a3a;
      padding: 1em;

      background: #8c0400;
      color: #ffffff;
    }

    #banner {
      padding-bottom: 0.4em;
      padding-top: 0.4em;
      background: #A9F16C;

      font-size: 150%;
      font-weight: bold;
      text-align: center;

      border-bottom: 2px solid #3a3a3a;
    }

    #race {
      /* second number is trap +4 */
      margin: 1em 18em 1em 1em;

      border-radius: 1em;
      border: 2px solid #3a3a3a;
      padding: 1em;

      background: #e7e7e7;
    }

    #trap {
      float: right;
      width: 14em;
      margin: 1em;

      border-radius: 1em;
      border: 2px solid #3a3a3a;
      padding: 1em;

      background: #9cd9ff;
    }

    .widget-title {
      font-weight: bold;
      font-size: 120%;
      border-bottom: 2px dashed #3a3a3a;
    }


    /* Race result lane header tweaks */
    .column-header {
      font-weight: bold;
      font-size: 80%;
    }
    #race .column-header {
      color: #7c7c7c;
    }
    #trap .column-header {
      color: #456a81;
    }

    /* Race result row */
    .row {
      text-align: right;
      position: relative;
    }
    .row div , #trap .row {
      /* Without, winner bold will add to line height and floating divs DO WHATEVER THE FUCK THEY WANT */
      height: 1.3em;
    }
    .row:nth-child(even):not(.column-header) {
      background: rgba(0, 0, 0, 0.1);
    }

    #race .row div.a {
      width: 50%;
      float: left;
    }

    #race .row div.b {
      width: 50%;
      margin-left: 50%;
    }

    /* Make first result bigger */
    .row:nth-child(1) {
      font-size: 250%;
      font-weight: bold;
    }

    /* Winner */
    /*.winner:before {*/
      /*content: "> ";*/
    /*}*/

    .winner {
      color: #34722a;
      font-weight: bold;
      /*height: 1em;*/
    }

  </style>
</head>

<body>
<div id="banner">
  FirLumber Rally
</div>
<div id="error">
  You need JS enabled for this to work.
</div>
<div id="trap">
  <div class="widget-title">
    Trap
  </div>
  <div class="row column-header">
    <div>Lane A</div>
  </div>
  <div id="trap-results">
    <div id="trap-result-template" class="row">
      -
    </div>
  </div>
</div>
<div id="race">
  <div class="widget-title">
    Race Results
  </div>
  <div class="row column-header">
    <div class="a">Lane A</div>
    <div class="b">Lane B</div>
  </div>
  <div id="race-results">
    <div id="race-result-template" class="row">
      <div class="a">-</div>
      <div class="b">-</div>
    </div>
  </div>
</div>
</body>
</html>
