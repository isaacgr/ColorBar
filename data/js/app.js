// used when hosting the site on the ESP8266
var url = "http://esp32.local";
var urlBase = "";

// used when hosting the site somewhere other than the ESP8266 (handy for testing without waiting forever to upload to SPIFFS)
// var address = "192.168.86.55";
// var urlBase = "http://" + address + "/";

var currentPowerState;
var currentBrightness;

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
}

function updatePattern(pattern) {
  console.log(pattern);
  setField("pattern", pattern)
    .then((result) => {
      document.getElementById(
        "pattern-selection"
      ).innerHTML = result.newValue.split(/(?=[A-Z])/).join(" ");
    })
    .catch((error) => setError(error));
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

function getLedInfo() {
  fetch(url + "/fastLedInfo")
    .then((response) => {
      return response.json();
    })
    .then((json) => populateInfoTable(json))
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

function setError(error) {
  document.getElementById("error").innerHTML = `Error: ${error.message}`;
}

getLedInfo();
getAllInfo();
