// used when hosting the site on the ESP8266
var address = location.hostname;
var urlBase = "";

// used when hosting the site somewhere other than the ESP8266 (handy for testing without waiting forever to upload to SPIFFS)
// var address = "192.168.86.55";
// var urlBase = "http://" + address + "/";

function formatLedInfo(json) {
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

function fetchLedInfo() {
  fetch("http://esp32.local/fastLedInfo")
    .then((response) => {
      return response.json();
    })
    .then((json) => formatLedInfo(json));
}

setInterval(() => {
  fetchLedInfo();
}, 5000);
fetchLedInfo();
