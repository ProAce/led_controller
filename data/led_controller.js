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

cW.on("input:change", function(color) {
  var color = cW.color.rgb;

  send_color(color);
  console.log(color);
});

cW.on("mount", function(color) {});
