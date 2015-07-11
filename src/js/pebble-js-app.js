kodi_ip="192.168.1.51";
kodi_port="8080";
kodi_url="http://"+kodi_ip+":"+kodi_port+"/jsonrpc?request=";

SQ_Adress = localStorage.getItem(6);

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
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

function trackInfo() {
  console.log("trackinfo():"+SQ_Adress);
  var url=SQ_Adress+"/jsonrpc.js";
  var myData='{"id":1,"method":"slim.request","params":["00:04:20:26:27:45",["status","-",1,"tags:gABbehldiqtyrSuoKLN"]]}'
  ajaxJSONPost(url, myData,
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
      var title = json.result.playlist_loop[0].title;
      var albumartist = json.result.playlist_loop[0].artist;
      var album = json.result.playlist_loop[0].album;
      // var info = title+" de "+album+" par "+albumartist;
      // console.log(info);

      // Assemble dictionary using our keys
      var dictionary = {
        "PLAT": "sq",
        "TITLE_INFO": title,
        "ARTIST_INFO": albumartist,
        "ALBUM_INFO": album,
      };
 
      // Send to Pebble
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
    var myData='{"jsonrpc": "2.0", "method": "Player.GetItem", "params": {"playerid": 1 }, "id": "test"},'
    ajaxJSONPost(kodi_url, myData,
      function(responseText) {
        var json = JSON.parse(responseText);
        var title = json.result.item.label;
        console.log(title);
        // var albumartist = json.result.playlist_loop[0].artist;
        // var album = json.result.playlist_loop[0].album;
        // var info = title+" de "+album+" par "+albumartist;
        // console.log(info);

        // Assemble dictionary using our keys
        var dictionary = {
          "PLAT": "kodi",
          "TITLE_INFO": title,
          // "ARTIST_INFO": albumartist,
          // "ALBUM_INFO": album,
        };
   
        // Send to Pebble
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
    var kodi_url="http://"+kodi_ip+":"+kodi_port+"/jsonrpc?request=";
    var kodi_command='{"jsonrpc": "2.0", "method": "Player.PlayPause", "params": { "playerid": 1 }, "id": 1}';
    xhrRequest(kodi_url+kodi_command, 'GET', function(responseText) {console.log(responseText);});
  }
}

function locationError(err) {
  console.log('Error requesting location!');
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
    // console.log("111"+SQ_Adress);
    //Send to Pebble, persist there
    // Pebble.sendAppMessage(
    //   {"SQ_ADRESS": SQ_Adress},
    //   function(e) {
    //     console.log("Sending settings data..."+SQ_Adress);
    //   },
    //   function(e) {
    //     console.log("Settings feedback failed!");
    //   }
    // );
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
          trackInfo();
        }
        if (e.payload["MESSAGE_KEY"] == "play"){
          var myurl=SQ_Adress+"/status.html?p0=pause&player=00%3A04%3A20%3A26%3A27%3A45";
          xhrRequest(myurl, 'GET', function(responseText) {console.log("play");});
        }
        if (e.payload["MESSAGE_KEY"] == "previous"){
          var myurl=SQ_Adress+"/status.html?p0=playlist&p1=jump&p2=-1&player=00%3A04%3A20%3A26%3A27%3A45";
          xhrRequest(myurl, 'GET', function(responseText) {console.log("previous");});
          trackInfo();
        }
        if (e.payload["MESSAGE_KEY"] == "next"){
          var myurl=SQ_Adress+"/status.html?p0=playlist&p1=jump&p2=%2b1&player=00%3A04%3A20%3A26%3A27%3A45";
          xhrRequest(myurl, 'GET', function(responseText) {console.log("next");});
          trackInfo();
        }
      }
  		if( e.payload["STATUS_KEY"] == "kodi") {
        console.log("Kodi");
        if (e.payload["MESSAGE_KEY"] == "info"){
          send_command_Kodi("info");
        }
        if (e.payload["MESSAGE_KEY"] == "play"){
          send_command_Kodi("play");
        }
      }
		} catch (exc) {
		  console.log("exception: " + exc.message);
		}
	}
);