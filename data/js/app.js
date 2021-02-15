// used when hosting the site on the ESP8266
var url = "http://tv.local";
var urlBase = "";

// used when hosting the site somewhere other than the ESP8266 (handy for testing without waiting forever to upload to SPIFFS)
// var address = "192.168.86.55";
// var urlBase = "http://" + address + "/";

var currentPowerState;
var currentBrightness;
var colorWheel = new iro.ColorPicker("#colorWheelDemo");

function populateInfoTable(json) {
  let rows = json.map((info) => {
    let tds = Object.entries(info)
      .map(([key, value]) => {
        if (key === "name") {
          if (value.includes("_")) {
            value = value.split("_").join(" ");
          }
        }
        return `<td>${value}</td>`;
      })
      .join("");
    return `<tr>${tds}</tr>`;
  });
  document.getElementById("fastled__table").innerHTML = rows.join("");
}

function handlePower(value) {
  currentPowerState = parseInt(value);

  if (currentPowerState === 1) {
    document.getElementById("power-button").className =
      "btn btn-success btn-lg";
  } else if (currentPowerState === 0) {
    document.getElementById("power-button").className =
      "btn btn-secondary btn-lg";
  }
}

function updatePower() {
  // assume button press to toggle power
  var newPowerState = currentPowerState == 0 ? 1 : 0;
  setField("power", newPowerState)
    .then((json) => {
      currentPowerState = json.newValue;
      handlePower(currentPowerState);
    })
    .catch((error) => {
      setError(error);
    });
}

function handleBrightness(json) {
  currentBrightness = parseInt(json.value);
  document.getElementById("brightness-slider").min = json.min;
  document.getElementById("brightness-slider").max = json.max;
  document.getElementById("brightness-slider").value = currentBrightness;
  document.getElementById("brightness-value").innerHTML = currentBrightness;
}

function updateBrightness(value) {
  currentBrightness = parseInt(value);
  setField("brightness", value)
    .then((json) => {
      currentBrightness = json.newValue;
      document.getElementById("brightness-value").innerHTML = currentBrightness;
    })
    .catch((error) => {
      setError(error);
    });
}

function handlePattern(field) {
  document.getElementById("pattern-selection").innerHTML = field.value
    .split(/(?=[A-Z])/)
    .join(" ");
  let items = field.options
    .split(",")
    .map((item) => {
      return `<li class="dropdown-item" onclick="updatePattern(this.id)" id=${item}>${item
        .split(/(?=[A-Z])/)
        .join(" ")}</li>`;
    })
    .join("");
  document.getElementById("pattern-options").innerHTML = items;
  getInvalidModifiers();
}

function updatePattern(pattern) {
  setField("pattern", pattern)
    .then((result) => {
      document.getElementById(
        "pattern-selection"
      ).innerHTML = result.newValue.split(/(?=[A-Z])/).join(" ");
      getInvalidModifiers();
    })
    .catch((error) => setError(error));
}

function getInvalidModifiers() {
  getFieldModifiers("pattern").then((result) => {
    const ids = [...document.querySelectorAll("#modifiers > table tr")].map(
      ({ id }) => id
    );
    ids.forEach((modifier) => {
      document.getElementById(modifier).style.display = "";
    });
    const modifiers = result.modifiers.split(",");
    const invalidModifiers = ids.filter((id) => !modifiers.includes(id));
    invalidModifiers.forEach((modifier) => {
      document.getElementById(modifier).style.display = "none";
    });
  });
}

function updateModifier(id, value) {
  if (value === true || value === false) {
    setField(id.split("-")[0], value === true ? "1" : "0").catch((error) =>
      setError(error)
    );
  } else {
    setField(id.split("-")[0], value).catch((error) => setError(error));
  }
}

function handleModifier(id, value, type) {
  document.getElementById(`${id}-input`).value = value;
  if (type === "Boolean") {
    document.getElementById(`${id}-input`).checked =
      value === "1" ? true : false;
  }
}

function handlePalette(field) {
  document.getElementById("palette-selection").innerHTML = field.value
    .split(/(?=[A-Z])/)
    .join(" ");
  let items = field.options
    .split(",")
    .map((item) => {
      return `<li class="dropdown-item" onclick="updatePalette(this.id)" id=${item}>${item
        .split(/(?=[A-Z])/)
        .join(" ")}</li>`;
    })
    .join("");
  document.getElementById("palette-options").innerHTML = items;
}

function updatePalette(palette) {
  setField("palette", palette)
    .then((result) => {
      document.getElementById(
        "palette-selection"
      ).innerHTML = result.newValue.split(/(?=[A-Z])/).join(" ");
    })
    .catch((error) => setError(error));
}

function handleColorTemperature(field) {
  document.getElementById(
    "temperature-selection"
  ).innerHTML = field.value.split(/(?=[A-Z])/).join(" ");
  let items = field.options
    .split(",")
    .map((item) => {
      return `<li class="dropdown-item" onclick="updateColorTemperature(this.id)" id=${item}>${item
        .split(/(?=[A-Z])/)
        .join(" ")}</li>`;
    })
    .join("");
  document.getElementById("temperature-options").innerHTML = items;
}

function updateColorTemperature(temperature) {
  setField("colorTemperature", temperature)
    .then((result) => {
      document.getElementById(
        "temperature-selection"
      ).innerHTML = result.newValue.split(/(?=[A-Z])/).join(" ");
    })
    .catch((error) => setError(error));
}

function handleSolidColor(field) {
  const r = field.value.split(",")[0];
  const g = field.value.split(",")[1];
  const b = field.value.split(",")[2];
  colorWheel.color.rgb = { r, g, b };
}

function populateFields(json) {
  json.forEach((field) => {
    if (field.name === "power") {
      currentPowerState = parseInt(field.value);
      handlePower(field.value);
    } else if (field.name === "brightness") {
      handleBrightness(field);
    } else if (field.name === "pattern") {
      handlePattern(field);
    } else if (field.name === "solidColor") {
      handleSolidColor(field);
    } else if (field.name === "palette") {
      handlePalette(field);
    } else if (field.name === "colorTemperature") {
      handleColorTemperature(field);
    } else if (field.isModifier === true) {
      handleModifier(field.name, field.value, field.type);
    }
  });
}

function getField(name) {
  return fetch(`${url}/fieldValue?name=${name}`)
    .then((response) => {
      return response.json();
    })
    .then((json) => json)
    .catch((error) => {
      setError(error);
    });
}

function setField(name, value) {
  return fetch(`${url}/fieldValue?name=${name}&value=${value}`, {
    method: "POST"
  })
    .then((response) => {
      return response.json();
    })
    .then((json) => json);
}

function getFieldModifiers(name) {
  return fetch(`${url}/fieldModifiers?name=${name}`)
    .then((response) => {
      return response.json();
    })
    .then((json) => json)
    .catch((error) => {
      setError(error);
    });
}

function getLedInfo() {
  fetch(url + "/fastLedInfo")
    .then((response) => {
      return response.json();
    })
    .then((json) => {
      populateInfoTable(json);
      setTimeout(getLedInfo, 10000);
    })
    .catch((error) => {
      setError(error);
    });
}

function getAllInfo() {
  fetch(url + "/all")
    .then((response) => {
      return response.json();
    })
    .then((json) => {
      populateFields(json);
    })
    .catch((error) => {
      setError(error);
    });
}

colorWheel.on("input:end", function (color, changes) {
  // when the color has changed, the callback gets passed the color object and an object providing which color channels (out of H, S, V) have changed.
  setField(
    "solidColor",
    `${color.rgb.r},${color.rgb.g},${color.rgb.b}`
  ).catch((error) => setError(error));
});

function setError(error) {
  console.log(error);
  document.getElementById("error").innerHTML = `Error: ${error.message}`;
}

getLedInfo();
getAllInfo();
