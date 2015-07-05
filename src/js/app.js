/**
 * Welcome to Pebble.js!
 *
 * This is where you write your app.
 */
var UI = require('ui');
var Vector2 = require('vector2');

var main = new UI.Menu({
  backgroundColor: 'red',
  textColor: 'white',
  highlightBackgroundColor: 'yellow',
  highlightTextColor: 'black',
  sections: [{
    title: 'Telemator',
    items: [{
      title: 'Squeezebox',
      subtitle: 'Some subtitle',
      icon: 'images/squeezebox.png'
    }, {
      title: 'Kodi'
    }, {
      title: 'n/a'
    }, {
      title: 'n/a'
    }]
  }]
});
var squeezebox = new UI.Menu({
  backgroundColor: 'red',
  textColor: 'white',
  highlightBackgroundColor: 'yellow',
  highlightTextColor: 'black',
  sections: [{
    title: 'Squeezebox',
    items: [{
      title: 'Play',
    }, {
      title: 'Pause'
    }, {
      title: 'stop'
    }]
  }]
});
var squeezeboxAlarm = new UI.Card({
  title: 'Squeezebox Alarm',
  body: 'Loading...',
});
var testMenu = new UI.Card({
  title: 'n/a',
  body: 'n/a',
});
var kodiMenu = new UI.Menu({
  backgroundColor: 'red',
  textColor: 'white',
  highlightBackgroundColor: 'yellow',
  highlightTextColor: 'black',
  sections: [{
    title: 'Kodi',
    items: [{
      title: 'left',
    }, {
      title: 'Select'
    }, {
      title: 'right'
    }]
  }]
});
var kodiMenu2 = new UI.Menu({
  backgroundColor: 'red',
  textColor: 'white',
  highlightBackgroundColor: 'yellow',
  highlightTextColor: 'black',
  sections: [{
    title: 'Kodi',
    items: [{
      title: 'up',
    }, {
      title: 'Select'
    }, {
      title: 'down'
    }]
  }]
});
main.show();

main.on('select', function(e) {
  console.log('Selected item #' + e.itemIndex + ' of section #' + e.sectionIndex);
  console.log('The item is titled "' + e.item.title + '"');
  switch (e.item.title) {
    case "Squeezebox":
      squeezebox.show();
      main.hide();
      break;
    case "Kodi":
      kodiMenu.show();
      main.hide();
      break;
    default:
      //Statements executed when none of the values match the value of the expression
      break;
  }
});

// squeezebox.on('click', 'up', function(e) {
//   squeebox("play");
// });
// squeezebox.on('longClick', 'up', function(e) {
//   squeebox("playlist&p1=jump&p2=%2b1");
// });
// squeezebox.on('click', 'select', function(e) {
//  squeebox("pause");
// });
// squeezebox.on('longClick', 'select', function(e) {
//   squeezeboxAlarm.show();
//   squeezebox.hide();
//   getSBInfo()
// });
// squeezebox.on('click', 'down', function(e) {
//   squeebox("stop");
// });
// squeezebox.on('longClick', 'down', function(e) {
//   squeebox("shutdown");
// });
// squeezebox.on('click', 'back', function(e) {
//   main.show();
//   squeezebox.hide();
// });
squeezebox.on('select', function(e) {
  console.log('Selected item #' + e.itemIndex + ' of section #' + e.sectionIndex);
  console.log('The item is titled "' + e.item.title + '"');
  switch (e.item.title) {
    case "Play":
      squeebox("play");
      break;
    case "Pause":
      squeebox("pause");
      break;
    case "Stop":
      squeebox("stop")
      break;
    default:
      //Statements executed when none of the values match the value of the expression
      break;
  }
});
squeezebox.on('longSelect', function(e) {
  console.log('Selected item #' + e.itemIndex + ' of section #' + e.sectionIndex);
  console.log('The item is titled "' + e.item.title + '"');
  switch (e.item.title) {
    case "Play":
       squeebox("playlist&p1=jump&p2=%2b1");
      break;
    case "Pause":
      squeezeboxAlarm.show();
      squeezebox.hide();
      getSBInfo();
      break;
    case "Stop":
      squeebox("shutdown");
      break;
    default:
      //Statements executed when none of the values match the value of the expression
      break;
  }
});

kodiMenu.on('click', 'up', function(e) {
  sendKodi('{"jsonrpc": "2.0", "method": "Input.Left", "id": 1}');
});
kodiMenu.on('click', 'select', function(e) {
  sendKodi('{"jsonrpc": "2.0", "method": "Input.Select", "id": 1}');
});
kodiMenu.on('longClick', 'select', function(e) {
  sendKodi('{"jsonrpc": "2.0", "method": "Player.PlayPause", "params": { "playerid": 1 }, "id": 1}');
});
kodiMenu.on('click', 'down', function(e) {
  sendKodi('{"jsonrpc": "2.0", "method": "Input.Right", "id": 1}');
});
kodiMenu.on('longClick', 'down', function(e) {
  sendKodi('{"jsonrpc": "2.0", "method": "Input.Back", "id": 1}');
});
kodiMenu.on('accelTap', function(e) {
  console.log('Tapped the window');
  kodiMenu2.show();
  kodiMenu.hide();
});
kodiMenu.on('click', 'back', function(e) {
  main.show();
  kodiMenu.hide();
});
kodiMenu2.on('click', 'up', function(e) {
  sendKodi('{"jsonrpc": "2.0", "method": "Input.Up", "id": 1}');
});
kodiMenu2.on('click', 'select', function(e) {
  sendKodi('{"jsonrpc": "2.0", "method": "Input.Select", "id": 1}');
});
kodiMenu2.on('longClick', 'select', function(e) {
  sendKodi('{"jsonrpc": "2.0", "method": "Player.PlayPause", "params": { "playerid": 0 }, "id": 1}');
});
kodiMenu2.on('click', 'down', function(e) {
  sendKodi('{"jsonrpc": "2.0", "method": "Input.Down", "id": 1}');
});
kodiMenu2.on('longClick', 'down', function(e) {
  sendKodi('{"jsonrpc": "2.0", "method": "Input.Back", "id": 1}');
});
kodiMenu2.on('accelTap', function(e) {
  console.log('Tapped the window');
  kodiMenu.show();
  kodiMenu2.hide();
});
kodiMenu2.on('click', 'back', function(e) {
  main.show();
  kodiMenu2.hide();
});



function squeebox(command){
  var ajax = require('ajax');
  if(command=="shutdown") {
    var myurl="http://192.168.1.40:9002/index.html?player=00%3A04%3A20%3A26%3A27%3A45&p0=power&p1=0"
  } else {
  var myurl="http://192.168.1.40:9002/status.html?p0="+command+"&player=00%3A04%3A20%3A26%3A27%3A45"
  }
  ajax({ url: myurl, method:'get'});
}

function getSBInfo(){
  var ajax = require('ajax');
  ajax({ url: 'http://192.168.1.46:8080/test.json', type: 'json' },
    function(data){
      squeezeboxAlarm.body("Hours:"+data.squeeze.hours+"\nDays:"+data.squeeze.days);
    }
  );
}
function sendInfo(text){
  var ajax = require('ajax');
  ajax({ url: 'http://192.168.1.46:8080/form.php?var='+text });
}

function sendKodi(text){
  var ajax = require('ajax');
  ajax({ url: 'http://192.168.1.51:8080/jsonrpc?request='+text, type: 'json' },
    function(data){
      // if(data.result != "OK"){
      //   kodiMenu.body("error"); 
      // }else {kodiMenu.body("success"); }
    }
  );
}