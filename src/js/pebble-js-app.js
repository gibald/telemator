kodi_ip="192.168.1.51";
kodi_port="8080";
kodi_url="http://"+kodi_ip+":"+kodi_port+"/jsonrpc?request=";

SQ_Adress = "http://192.168.1.40:9002";
// SQ_Adress = localStorage.getItem(6);

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  // if xhr.status=null => off
  xhr.onload = function () {
    console.log("status::: "+xhr.status);
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function ajaxJSONPost(url, jsondata, callback){
  var xhr = new XMLHttpRequest();
  xhr.open("POST", url);
  xhr.setRequestHeader('Content-Type', 'application/json');
  xhr.onreadystatechange = function () {
    // console.log(xhr.readyState);
    // console.log(xhr.status);
    if (xhr.readyState == 4 && xhr.status == 200) {
      callback(xhr.responseText);
    }
  }
  xhr.send(jsondata);
}

function init_SqueezeBox() {
  console.log("init_SuqueezeBox");
  var url=SQ_Adress+"/jsonrpc.js";
  var myData='{"id":1,"method":"slim.request","params":["",["serverstatus",0,999]]}';
  ajaxJSONPost(url, myData,
    function(responseText) {
      var json = JSON.parse(responseText);
      SQ_mac = json.result.players_loop[0].playerid;
      console.log("mac_SQ: "+SQ_mac);
    }
  );
  trackInfo();
}

function trackInfo() {
  console.log("trackinfo():"+SQ_Adress);
  var url=SQ_Adress+"/jsonrpc.js";
  var myData='{"id":1,"method":"slim.request","params":["'+SQ_mac+'",["status","-",1,"tags:gABbehldiqtyrSuoKLN"]]}';
  ajaxJSONPost(url, myData,
    function(responseText) {
      var json = JSON.parse(responseText);
      var title = json.result.playlist_loop[0].title;
      var albumartist = json.result.playlist_loop[0].artist;
      var album = json.result.playlist_loop[0].album;

      var dictionary = {
        "PLAT": "sq",
        "TITLE_INFO": title,
        "ARTIST_INFO": albumartist,
        "ALBUM_INFO": album,
      };
 
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
        }
      );
    }
  );
}

function send_command_Kodi(command) {
  if(command == "info"){
    var myData='{"jsonrpc": "2.0", "method": "Player.GetItem", "params": {"playerid": '+kodi_playerid+' }, "id": "test"}';
    ajaxJSONPost(kodi_url, myData,
      function(responseText) {
        var json = JSON.parse(responseText);
        var title = json.result.item.label;

        var dictionary = {
          "PLAT": "kodi",
          "TITLE_INFO": title,
        };
   
        Pebble.sendAppMessage(dictionary,
          function(e) {
            console.log("Info sent to Pebble successfully!");
          },
          function(e) {
            console.log("Error sending weather info to Pebble!");
          }
        );
      }
    );
  }
  if(command == "play"){
    var kodi_command='{"jsonrpc": "2.0", "method": "Player.PlayPause", "params": { "playerid": '+kodi_playerid+' }, "id": 1}';
    xhrRequest(kodi_url+kodi_command, 'GET', function(responseText) {console.log(responseText);});
  }
  if(command == "select"){
    var kodi_command='{"jsonrpc": "2.0", "method": "Input.Select", "id": "test"}';
    xhrRequest(kodi_url+kodi_command, 'GET', function(responseText) {console.log(responseText);});
  }
  if(command == "left"){
    var kodi_command='{"jsonrpc": "2.0", "method": "Input.Left", "id": "test"}';
    xhrRequest(kodi_url+kodi_command, 'GET', function(responseText) {console.log(responseText);});
  }
  if(command == "right"){
    var kodi_command='{"jsonrpc": "2.0", "method": "Input.Right", "id": "test"}';
    xhrRequest(kodi_url+kodi_command, 'GET', function(responseText) {console.log(responseText);});
  }
  if(command == "up"){
    var kodi_command='{"jsonrpc": "2.0", "method": "Input.Up", "id": "test"}';
    xhrRequest(kodi_url+kodi_command, 'GET', function(responseText) {console.log(responseText);});
  }
  if(command == "down"){
    var kodi_command='{"jsonrpc": "2.0", "method": "Input.Down", "id": "test"}';
    xhrRequest(kodi_url+kodi_command, 'GET', function(responseText) {console.log(responseText);});
  }
  if(command == "move_left"){
    var kodi_command='{"jsonrpc": "2.0", "method": "Player.Move", "params": { "playerid": '+kodi_playerid+', "direction": "left" }, "id": "test"}';
    xhrRequest(kodi_url+kodi_command, 'GET', function(responseText) {console.log(responseText);});
  }
  if(command == "move_right"){
    var kodi_command='{"jsonrpc": "2.0", "method": "Player.Move", "params": { "playerid": '+kodi_playerid+', "direction": "right" }, "id": "test"}';
    xhrRequest(kodi_url+kodi_command, 'GET', function(responseText) {console.log(responseText);});
  }
}

function init_Kodi() {
  console.log("init_Kodi");
  var kodi_command='{ "id": 1, "jsonrpc": "2.0", "method": "Player.GetActivePlayers" }';
  xhrRequest(kodi_url+kodi_command, 'GET',
    function(responseText) {
      console.log(responseText);
      var json = JSON.parse(responseText);
      if(json.result[0]){
        if (json.result[0].hasOwnProperty('playerid') == true) {
          kodi_playerid = json.result[0].playerid;
          kodi_type = json.result[0].type;
          if(kodi_type=="video") {
            send_command_Kodi("info");
          }
        }
      }
      else {
        var dictionary = {
          "PLAT": "kodi",
          "TITLE_INFO": "NULL_NOT_PLAY",
        };
        Pebble.sendAppMessage(dictionary,
          function(e) {
            console.log("Info sent to Pebble successfully!");
          },
          function(e) {
            console.log("Error sending weather info to Pebble!");
          }
        );
      }
    }
  );
}

// function getSBInfo(){
//   var ajax = require('ajax');
//   ajax({ url: 'http://192.168.1.46:8080/test.json', type: 'json' },
//     function(data){
//       console.log(":::::::2"+data.squeeze.hours);
//       squeezeboxAlarm.body("Hours:"+data.squeeze.hours+"\nDays:"+data.squeeze.days);
//     }
//   );
// }

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
  }
);

Pebble.addEventListener("showConfiguration",
  function(e) {
    //Load the remote config page
    Pebble.openURL("http://srv.gibald.com/pebble/telemator_conf.html");
  }
);

Pebble.addEventListener("webviewclosed",
  function(e) {
    //Get JSON dictionary
    var configuration = JSON.parse(decodeURIComponent(e.response));
    console.log("Configuration window returned: " + JSON.stringify(configuration));
    var SQ_IP=configuration.ip;
    var SQ_Port=configuration.port;
    SQ_Adress="http://"+SQ_IP+":"+SQ_Port;

    localStorage.setItem(6, SQ_Adress);
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function (e) {
  	try {
      console.log(JSON.stringify(e.payload));
      console.log("status "+e.payload["STATUS_KEY"]);
  		console.log("message "+e.payload["MESSAGE_KEY"]);
      if( e.payload["STATUS_KEY"] == "sb") {
        if (e.payload["MESSAGE_KEY"] == "track_info") {
          init_SqueezeBox();
        }
        if (e.payload["MESSAGE_KEY"] == "play"){
          var myurl=SQ_Adress+"/status.html?p0=pause&player="+SQ_mac;
          xhrRequest(myurl, 'GET', function(responseText) {console.log("play");});
        }
        if (e.payload["MESSAGE_KEY"] == "previous"){
          var myurl=SQ_Adress+"/status.html?p0=playlist&p1=jump&p2=-1&player="+SQ_mac;
          xhrRequest(myurl, 'GET', function(responseText) {console.log("previous");});
          trackInfo();
        }
        if (e.payload["MESSAGE_KEY"] == "next"){
          var myurl=SQ_Adress+"/status.html?p0=playlist&p1=jump&p2=%2b1&player="+SQ_mac;
          xhrRequest(myurl, 'GET', function(responseText) {console.log("next");});
          trackInfo();
        }
      }
  		if( e.payload["STATUS_KEY"] == "kodi") {
        console.log("Kodi");
        if (e.payload["MESSAGE_KEY"] == "init"){
          init_Kodi();
        }
        if (e.payload["MESSAGE_KEY"] == "info"){
          send_command_Kodi("info");
        }
        if (e.payload["MESSAGE_KEY"] == "play"){
          send_command_Kodi("play");
        }
        if (e.payload["MESSAGE_KEY"] == "select"){
          send_command_Kodi("select");
        }
        if (e.payload["MESSAGE_KEY"] == "left"){
          send_command_Kodi("left");
        }
        if (e.payload["MESSAGE_KEY"] == "right"){
          send_command_Kodi("right");
        }
        if (e.payload["MESSAGE_KEY"] == "up"){
          send_command_Kodi("up");
        }
        if (e.payload["MESSAGE_KEY"] == "down"){
          send_command_Kodi("down");
        }
        if (e.payload["MESSAGE_KEY"] == "move_left"){
          send_command_Kodi("move_left");
        }
        if (e.payload["MESSAGE_KEY"] == "move_right"){
          send_command_Kodi("move_right");
        }
      }
		} catch (exc) {
		  console.log("exception: " + exc.message);
		}
	}
);