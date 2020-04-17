var cW = new iro.ColorPicker("#cW", {
  layout: [
    {
      component: iro.ui.Wheel
    },
    {
      component: iro.ui.Slider
    }
  ]
});

fetch("/led")
  .then(resp => resp.json())
  .then(data => {
    console.log(data);
    cW.color.rgb = data;
  });

function send_color(color) {
  fetch("/led", {
    method: "POST",
    body: JSON.stringify(color),
    headers: {
      Accept: "application/json",
      "Content-Type": "application/json"
    }
  });
}

function leds_off()
{
  cW.color.rgb = {"r":0, "g":0, "b":0};
  send_color(cW.color.rgb);
}

function leds_on()
{
  cW.color.rgb = {"r":255, "g":255, "b":255};
  send_color(cW.color.rgb);
}

cW.on("input:end", function(color) {
  var color = cW.color.rgb;

  send_color(color);
  console.log(color);
});

cW.on("mount", function(color) {});
