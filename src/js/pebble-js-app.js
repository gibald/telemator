var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

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

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
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

    // Get the initial weather
    getWeather();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function (e) {
  	try {
		console.log(JSON.stringify(e.payload));
		if (e.payload["KEY_EXCHANGEDATA"] == 1) 
		{
		  console.log("Button SELECT");

		  var myurl="http://192.168.1.40:9002/status.html?p0=play&player=00%3A04%3A20%3A26%3A27%3A45";
		  xhrRequest(myurl, 'GET', function(responseText) {console.log("senddd");})
		}
		if (e.payload["KEY_EXCHANGEDATA"] == 2) 
		{
		  var myurl="http://192.168.1.40:9002/status.html?p0=pause&player=00%3A04%3A20%3A26%3A27%3A45";
		  xhrRequest(myurl, 'GET', function(responseText) {console.log("senddd");})
		}
		if (e.payload["exchangeData"] == 3) 
		{
		  console.log("Button DOWN");
		}
		} catch (exc) {
		  console.log("exception: " + exc.message);
		}
	}
);