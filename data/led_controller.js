const socket = new WebSocket("ws://" + location.hostname + ":8080/");

var cW = new iro.ColorPicker("#cW", {
  layout: [
    {
      component: iro.ui.Wheel,
    },
    {
      component: iro.ui.Slider,
    },
  ],
});

function send_color(color) {
  var data = ["c", color.r, color.g, color.b];
  socket.send(data);
}

function leds_off() {
  cW.color.rgb = { r: 0, g: 0, b: 0 };
}

function leds_on() {
  cW.color.rgb = { r: 255, g: 255, b: 255 };
}

cW.on("color:change", function (color) {
  var color = cW.color.rgb;

  send_color(color);
  console.log(color);
});

cW.on("mount", function (color) {});

socket.addEventListener("open", function (event) {
  socket.send("i"); // Request initial settings on the opening off the websocket
});

socket.addEventListener("message", function (event) {
  console.log("Message from server ", event.data);

  var splitData = event.data.split(",");

  switch (
    splitData[0] // Parse the incoming messages
  ) {
    case "c":
      cW.color.r = splitData[1];
      cW.color.g = splitData[2];
      cW.color.b = splitData[3];
      break;
  }
});

window.onbeforeunload = function () {
  socket.close();
  window.location.reload(true);
};
