const url = "/action.html";

function actionSend(data) {
    const http = new XMLHttpRequest();
    http.open("POST", url, true);
    http.onreadystatechange = function () {
        //Call a function when the state changes.
        if (http.readyState == 4) {
            console.log(http.responseText);
        }
    }
    http.send(data);
}

function actionGet(data, callback) {
    const http = new XMLHttpRequest();
    http.open("POST", url, true);
    // http.responseType = 'json';
    http.onreadystatechange = function () {
        console.log("actionGet:onreadystatechange");
        //Call a function when the state changes.
        if (http.readyState == 4) {
            console.log("actionGet:readyState == 4");
            console.log(http.responseText);
            callback(http.response);
        }
    }
    http.send(data);
}

function updateValueText(name, value) {
    var currValueElem = document.getElementById(name);
    // console.log(currValueElem);
    currValueElem.innerHTML = value;
}

function inputSlider(e) {
    // console.log(e)
    // console.log(e.target.value);
    var idStr = e.target.id;
    // console.log(idStr);
    var nameStp = idStr.substr(0, idStr.length - 6);
    // console.log(nameStp + "Value");
    // var currValueElem = document.getElementById(nameStp + "Value");
    // console.log(currValueElem);
    // currValueElem.innerHTML = e.target.value;

    var alias = "NONE";
    if (nameStp == "Brightnes") {
        alias = "BRI";
    }
    if (nameStp == "Speed") {
        alias = "SPD";
    }
    if (nameStp == "Density") {
        alias = "DEN";
    }
    if (alias != "NONE") {
        var data = new FormData();
        data.append(alias, e.target.value);
        actionSend(data);
    }

    updateValueText(nameStp + "Value", e.target.value);
}

function updateSlidersText() {
    for (var i = 0; i < allSliders.length; i++) {
        var idStr = allSliders[i].id;
        var nameStp = idStr.substr(0, idStr.length - 6);
        updateValueText(nameStp + "Value", allSliders[i].value);
    }
}

function applySettings(cfgJSON) {
    console.log("applySettings:");
    console.log(cfgJSON);
    console.log("--->");
    jsonResponse = JSON.parse(cfgJSON, function (key, value) {
        if (key == 'date')
            return new Date(value);
        return value;
    });
    console.log(jsonResponse)
    console.log("===>");
    for (key in jsonResponse) {
        console.log(key, ":", jsonResponse[key]);
    }

    if (jsonResponse.STATE == 0)
        buttonOff.checked = true;
    else
        buttonOn.checked = true;
    effectSelVar.value = jsonResponse.MOD;
    briSlider.value = jsonResponse.BRI;
    spdSlider.value = jsonResponse.SPD;
    denSlider.value = jsonResponse.DEN;
    updateSlidersText();
}

function refreshSettings(e) {
    var data = new FormData();
    data.append("STS", "GET");
    actionGet(data, applySettings);
}

const briSlider = document.getElementById("BrightnesSlider");
const spdSlider = document.getElementById("SpeedSlider");
const denSlider = document.getElementById("DensitySlider");

var allSliders = document.getElementsByClassName("slider");
// console.log(allSliders);
for (var i = 0; i < allSliders.length; i++) {
    // console.log(allSliders[i]);
    allSliders[i].addEventListener("input", inputSlider);
    // var idStr = allSliders[i].id;
    // var nameStp = idStr.substr(0, idStr.length - 6);
    // updateValueText(nameStp + "Value", allSliders[i].value);
}
// updateSlidersText();

function changeEffect() {
    var effectSelValue = effectSelVar.value;
    var effectSelIndex = effectSelVar.selectedIndex;
    // console.log("Effect: " + effectSelValue + "=" + effectSelIndex);

    var data = new FormData();
    data.append("MOD", effectSelValue);

    actionSend(data);
}

const effectSelVar = document.getElementById("effectSelecter");
effectSelVar.addEventListener("change", changeEffect);

const buttonOn = document.getElementById("fid-2");
buttonOn.addEventListener("click", e => {
    // console.log("Button ON clicked.");

    var data = new FormData();
    data.append("PWR", "ON");

    actionSend(data);
});

const buttonOff = document.getElementById("fid-1");
buttonOff.addEventListener("click", e => {
    // console.log("Button OFF clicked.");

    var data = new FormData();
    data.append("PWR", "OFF");

    actionSend(data);
});

function resetEffectSettings(e) {
    var data = new FormData();
    data.append("STS", "RST");
    actionSend(data);
    refreshSettings();
}

const indReset = document.getElementById("index_reset");
indReset.addEventListener("click", resetEffectSettings);

const indRefresh = document.getElementById("index_refresh");
indRefresh.addEventListener("click", refreshSettings);

refreshSettings();