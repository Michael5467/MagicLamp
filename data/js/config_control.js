const url = "/action.html";

var lampName = document.getElementById("lampName");
var lampDate = document.getElementById("lampDate");
var lampTime = document.getElementById("lampTime");
var lampDebug = document.getElementById("lampDebug");

function actionSend(data) {
    const http = new XMLHttpRequest();
    http.open("POST", url, false);
    http.onreadystatechange = function () {
        //Call a function when the state changes.
        if (http.readyState == 4) {
            console.log(http.responseText);
        }
    }
    http.send(data);
}

function getConfig(e) {
    var data = new FormData();
    data.append("CFG", "GET");

    // actionGet(data, applySettings);

    const http = new XMLHttpRequest();
    http.open("POST", url, false);
    // http.responseType = 'json';
    http.onreadystatechange = function () {
        console.log("getConfig:onreadystatechange");
        //Call a function when the state changes.
        if (http.readyState == 4) {
            console.log("getConfig:readyState == 4");
            console.log(http.responseText);

            // applyName(http.response);

            console.log("applySettings:");
            console.log("--->");
            jsonResponse = JSON.parse(http.response, function (key, value) {
                if (key == "DATE")
                    return new Date(value);
                return value;
            });
            // console.log(jsonResponse)
            // console.log("===>");
            for (key in jsonResponse) {
                console.log("[", key, "] : {", jsonResponse[key], "}");
            }

            console.log(jsonResponse.LN);
            console.log(lampName);
            lampName.value = jsonResponse.LN;
            lampDebug.innerHTML = jsonResponse.DATE;
            lampDate.innerHTML = jsonResponse.DATE.toDateString();
            lampTime.innerHTML = jsonResponse.DATE.toTimeString();
        }
    }
    http.send(data);
}

function applyConfig(e) {
    var data = new FormData();
    data.append("CFG", "SET");
    data.append("NAME", lampName.value);
    actionSend(data);
}

function resetConfig(e) {
    var data = new FormData();
    data.append("CFG", "RST");
    actionSend(data);
    getConfig();
}

const indApply = document.getElementById("config_apply");
indApply.addEventListener("click", applyConfig);

const indReset = document.getElementById("config_reset");
indReset.addEventListener("click", getConfig);

getConfig();

// var date = new Date(2014, 11, 31, 12, 30, 0);
// console.log(date);
// console.log(lampDate);
// console.log(lampTime);
// console.log(date.toDateString());
// console.log(date.toTimeString());

// lampDate.innerHTML = date.toDateString();
// lampTime.innerHTML = date.toTimeString();
