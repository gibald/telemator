/**
 * Welcome to Pebble.js!
 *
 * This is where you write your app.
 */
var UI = require('ui');
var Vector2 = require('vector2');
var Accel = require('ui/accel');

var main = new UI.Card({
  title: 'Telemator',
  body: '1 - squeezebox\n2- kodi\n3- Test',
});
var squeezebox = new UI.Card({
  title: 'Squeezebox',
  body: '1 - Play\n2- Pause\n3- stop',
});
var squeezeboxAlarm = new UI.Card({
  title: 'Squeezebox Alarm',
  body: 'Loading...',
});
var testMenu = new UI.Card({
  title: 'n/a',
  body: 'n/a',
});
var kodiMenu = new UI.Card({
  title: 'Kodi',
  body: '1: left\n2: Select\n3: right',
});
var kodiMenu2 = new UI.Card({
  title: 'Kodi',
  body: '1: up\n2: Select\n3: down',
});
Accel.init();
main.show();

main.on('click', 'up', function(e) {
  squeezebox.show();
  main.hide();
});
main.on('click', 'select', function(e) {
  kodiMenu.show();
  main.hide();
});
main.on('click', 'down', function(e) {
  testMenu.show();
  main.hide();
  testMenu.on('accelTap', function(e) {
  console.log('Tapped the window');
});
});


squeezebox.on('click', 'up', function(e) {
  squeebox("play");
});
squeezebox.on('longClick', 'up', function(e) {
  squeebox("playlist&p1=jump&p2=%2b1");
});
squeezebox.on('click', 'select', function(e) {
 squeebox("pause");
});
squeezebox.on('longClick', 'select', function(e) {
  squeezeboxAlarm.show();
  squeezebox.hide();
  getSBInfo()
});
squeezebox.on('click', 'down', function(e) {
  squeebox("stop");
});
squeezebox.on('longClick', 'down', function(e) {
  squeebox("shutdown");
});
squeezebox.on('click', 'back', function(e) {
  main.show();
  squeezebox.hide();
});

testMenu.on('click', 'up', function(e) {
  sendInfo("JSSSSSS");
});
testMenu.on('click', 'select', function(e) {
});
testMenu.on('click', 'down', function(e) {
  getInfo();
});
testMenu.on('click', 'back', function(e) {
  squeezebox.show();
  testMenu.hide();
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