// Index page 
const char index_html[] =  R"=====(<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8" />
  <title>Hexapod</title>
  <meta name="robots" content="noindex, nofollow">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no" />
  <meta name="apple-mobile-web-app-capable" content="yes" />
  <meta name="apple-mobile-web-app-status-bar-style" content="black" />
  <link rel="shortcut icon" href="apple-touch-icon-120x120.png">
  <link rel="apple-touch-icon" href="apple-touch-icon-120x120.png" />
  <link rel="stylesheet" href="remote.css">
  <script src="ocanvas-2.8.3.min.js"></script>
</head>
<body>
<div id="vertical">
HexAJpod v0.2
</div>
<div id="horizontal">
<canvas id="canvas" width="100%" height="100%"></canvas>
<script>  
window.addEventListener("orientationchange", function() { if( window.orientation == -90 || window.orientation == 90 ) window.location.reload(); });

var c = document.querySelector("#canvas");

c.width = window.innerWidth;
c.height = window.innerHeight;

c.addEventListener("touchmove", function (e) { e.preventDefault(); }, false);
  
var canvas = oCanvas.create({ canvas: "#canvas", background: "#000" });

var connection = null;

function start(websocketServerLocation){
    connection= new WebSocket(websocketServerLocation);
    connection.onmessage = function (e) 
{  
  console.log('Server: ', e.data);
  textOutput.text = e.data;
  textOutput.redraw();
};
    connection.onopen = function () { connection.send('Connect ' + new Date()); }; 
    connection.onerror = function (error) { console.log('WebSocket Error ', error);};
    connection.onclose = function(){  textOutput.text = "Disconnected";
                                      textOutput.redraw();
                                      setTimeout(function(){ start(websocketServerLocation); }, 500);
                                   };
}
start('ws://192.168.4.1:81/');

// Controlbuttons
var r = canvas.width / 17;
var r0 = canvas.width / 30;
var r1 = canvas.width / 20;
var r2 = canvas.width / 6;
var d = r*2;
var d1 = r1/2;
var bx = canvas.width *0.65;
var by = canvas.height *0.56;
var jx = canvas.width *0.25;
var jy = canvas.height *0.56;

var sx = canvas.width *0.9;
var sy = canvas.height *0.56;
var pressedj = false;
var presseds = false;

var redButton = canvas.display.ellipse({ x:bx-d,y:by, radius: r, stroke: "7px Grey", fill: "Red" }).add();
var blueButton = canvas.display.ellipse({ x:bx,y:by-d, radius: r, stroke: "7px Grey", fill: "Blue" }).add();
var yellowButton = canvas.display.ellipse({ x:bx,y:by+d, radius: r, stroke: "7px Grey", fill: "Yellow" }).add();
var greenButton = canvas.display.ellipse({ x:bx+d,y:by, radius: r, stroke: "7px Grey", fill: "Green" }).add();

var joystickRim = canvas.display.ellipse({ x: jx, y: jy, radius: r2, stroke: "30px Grey" }).add();
var joystickButton = canvas.display.ellipse({ x: jx, y: jy, radius: r1, stroke: "7px Grey",fill: "White" }).add();

var shiftBox = canvas.display.rectangle( { x: sx-d1, y: sy-r2, width:d1*2, height:r2*2, fill: "Grey", stroke: "30px Black" }).add();
var shiftButton = canvas.display.ellipse({ x: sx, y: sy, radius: r0, stroke: "7px Grey",fill: "White" }).add();

var textOutput = canvas.display.text( {x: canvas.width *0.5, y: canvas.height *0.1, origin: { x: "center", y: "top" }, font: "bold 30px sans-serif", text: "Gamepad", fill: "Grey"} ).add();

redButton.bind("touchstart mousedown", function () 
{ 
  connection.send('#R');
  redButton.stroke = "none"; 
  redButton.redraw() 
});
blueButton.bind("touchstart mousedown", function () 
{ 
  connection.send('#B');
  blueButton.stroke = "none"; 
  blueButton.redraw();
});
yellowButton.bind("touchstart mousedown", function () 
{ 
  connection.send('#Y');
  yellowButton.stroke = "none"; 
  yellowButton.redraw() 
});
greenButton.bind("touchstart mousedown", function ()
{ 
  connection.send('#G');
  greenButton.stroke = "none"; 
  greenButton.redraw() 
});
canvas.bind("touchend mouseup", function () 
{ 
  redButton.stroke = "7px Grey"; redButton.redraw();
  blueButton.stroke = "7px Grey"; blueButton.redraw();
  yellowButton.stroke = "7px Grey"; yellowButton.redraw(); 
  greenButton.stroke = "7px Grey"; greenButton.redraw();
});

joystickButton.bind ("touchstart mousedown", function () 
{
  joystickButton.stroke = "none";
  joystickButton.redraw(); 
  pressedj = true; 
});

joystickButton.bind ("touchend mouseup", function () 
{ 
  joystickButton.x = jx; 
  joystickButton.y = jy; 
  joystickButton.stroke = "7px Grey";
  joystickButton.redraw(); 
  pressedj = false;
  connection.send('#x,0');
  connection.send('#y,0');
});

shiftButton.bind ("touchstart mousedown", function () 
{
  shiftButton.stroke = "none";

  shiftButton.redraw(); 
  presseds = true; 
});

shiftButton.bind ("touchend mouseup", function () 
{ 
  shiftButton.x = sx; 
  shiftButton.y = sy; 
  shiftButton.stroke = "7px Grey";
  shiftButton.redraw(); 
  presseds = false;
  connection.send('#z,0');
});

joystickRim.bind( "mousemove", function () 
{ 
  if( pressedj )
  {
    var xpos = Math.max(-r2,Math.min(canvas.mouse.x - jx,r2));
    var ypos = Math.max(-r2,Math.min(jy - canvas.mouse.y,r2));
  
    joystickButton.x = canvas.mouse.x; 
    joystickButton.y = canvas.mouse.y; 
    joystickButton.redraw(); 
    connection.send('#x,' + Math.round(127*xpos/r2) ); 
    connection.send('#y,' + Math.round(127*ypos/r2) );
  
  }
});

shiftBox.bind( "mousemove", function () 
{ 
  if( presseds )
  {
    var zpos =  Math.max(-r2,Math.min(sy-canvas.mouse.y,r2));
    shiftButton.x = sx; 
    shiftButton.y = canvas.mouse.y; 
    shiftButton.redraw(); 
    connection.send('#z,' + Math.round(127*zpos/r2) );
  }
});

shiftBox.bind( "touchmove", function () 
{ 
  if( presseds )
  {
    var zpos = Math.max(-r2,Math.min(sy-canvas.touch.y,r2)) ;
    shiftButton.x = sx; 
    shiftButton.y = canvas.touch.y; 
    shiftButton.redraw(); 
    connection.send('#z,' + Math.round(127*zpos/r2) );
  }
});

joystickRim.bind ("touchmove", function () 
{ 
  if( pressedj )
  {
    var xpos = Math.max(-r2,Math.min(canvas.touch.x - jx,r2));
    var ypos = Math.max(-r2,Math.min(jy - canvas.touch.y,r2));
    joystickButton.x = canvas.touch.x; 
    joystickButton.y = canvas.touch.y; 
    joystickButton.redraw(); 
    connection.send('#x,' + Math.round(127*xpos/r2) ); 
    connection.send('#y,' + Math.round(127*ypos/r2) );
  }
});
</script>
</div>
</body>
</html>
)=====";
