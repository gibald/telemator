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

function locationSuccess(pos) {
  // Construct URL
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
      pos.coords.latitude + '&lon=' + pos.coords.longitude;

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);

      // Temperature in Kelvin requires adjustment
      var temperature = Math.round(json.main.temp - 273.15);
      console.log('Temperature is ' + temperature);

      // Conditions
      var conditions = json.weather[0].main;      
      console.log("Conditions are " + conditions);
      
      // Assemble dictionary using our keys
      var dictionary = {
        "KEY_TEMPERATURE": temperature,
        "KEY_CONDITIONS": conditions
      };
 
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Weather info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
        }
      );
    }      
  );
}

function locationError(err) {
  console.log('Error requesting location!');
}

function getSBInfo(){
  
  var ajax = require('ajax');
  ajax({ url: 'http://192.168.1.46:8080/test.json', type: 'json' },
    function(data){
      console.log(":::::::2"+data.squeeze.hours);
      squeezeboxAlarm.body("Hours:"+data.squeeze.hours+"\nDays:"+data.squeeze.days);
    }
  );

}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function (e) {
  	try {
		console.log(JSON.stringify(e.payload));
		if (e.payload["MESSAGE_KEY"] == "play") 
		{
		  console.log("Button SELECT");
      var url1="http://192.168.1.40:9002/jsonrpc.js";
      var myData='{"id":1,"method":"slim.request","params":["00:04:20:26:27:45",["status","-",1,"tags:gABbehldiqtyrSuoKLN"]]}'
      ajaxJSONPost(url1, myData,
        function(responseText) {
          // responseText contains a JSON object with weather info
          var json = JSON.parse(responseText);
          var title = json.result.playlist_loop[0].title;
          var albumartist = json.result.playlist_loop[0].albumartist;
          var album = json.result.playlist_loop[0].album;
          var info = title+" de "+album+" par "+albumartist;
          console.log(info);

          // Assemble dictionary using our keys
          var dictionary = {
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
		if (e.payload["MESSAGE_KEY"] == "previous") 
		{
		  var myurl="http://192.168.1.400:9002/status.html?p0=playlist&p1=jump&p2=-1&player=00%3A04%3A20%3A26%3A27%3A45";
		  xhrRequest(myurl, 'GET', function(responseText) {console.log("senddd");})
		}
		if (e.payload["MESSAGE_KEY"] == "next") 
		{
		  console.log("Button DOWN");
      var myurl="http://192.168.1.400:9002/status.html?p0=playlist&p1=jump&p2=%2b1&player=00%3A04%3A20%3A26%3A27%3A45";
      xhrRequest(myurl, 'GET', function(responseText) {console.log("senddd");})
		}
		} catch (exc) {
		  console.log("exception: " + exc.message);
		}
	}
);