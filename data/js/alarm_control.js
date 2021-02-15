const url = "/action.html";

var allCheckBoxElem = document.getElementsByClassName("item_checkbox")
// console.log(allCheckBoxElem);

for (var i = 0; i < allCheckBoxElem.length; i++) {
    // console.log(allCheckBoxElem[i]);
    allCheckBoxElem[i].addEventListener("click", clickOnFlag);
    disTimeElem(allCheckBoxElem[i].value + "Time", !allCheckBoxElem[i].checked);
}

var allInputTimeElem = document.getElementsByClassName("item_time")
// console.log(allInputTimeElem);
var dayList = ["mon", "tue", "wed", "thu", "fri", "sat", "sun"];
var index = dayList.indexOf(200); // 1

for (var i = 0; i < allInputTimeElem.length; i++) {
    // console.log(allInputTimeElem[i]);
    allInputTimeElem[i].addEventListener("change", changeAlarmEvent);
}

function actionSend(data) {
    const http = new XMLHttpRequest();
    http.open("POST", url);
    http.onreadystatechange = function () {
        //Call a function when the state changes.
        if (http.readyState == 4) {
            console.log(http.responseText);
        }
    }
    http.send(data);
}

function changeAlarmEvent(e) {
    console.log("changeAlarmEvent(e):");
    console.log(e);
    changeAlarm(e.target)
}

function changeAlarm(target) {
    console.log("changeAlarm(e):");
    console.log(target);
    var idStr = target.id;
    console.log(idStr);
    var dayName = idStr.substr(0, 3);
    var dayIndex = dayList.indexOf(dayName);
    console.log(dayIndex);
    console.log(target.value);

    var data = new FormData();
    data.append("ALR", "SET");
    data.append("day", dayIndex);
    data.append("status", +allCheckBoxElem[dayIndex].checked);
    data.append("time", target.value);
    // data.append("hour", target.valueAsDate.getHours());
    // data.append("min", target.valueAsDate.getMinutes());
    actionSend(data);
}

function actionGet(data, callback) {
    const http = new XMLHttpRequest();
    http.open("POST", url, false);
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

function applyAlarms(cfgJSON) {
    console.log("applyAlarms:");
    console.log(cfgJSON);
    console.log("--->");
    jsonResponse = JSON.parse(cfgJSON);
    console.log(jsonResponse)
    console.log("===>");
    for (key in jsonResponse) {
        console.log(key, ":", jsonResponse[key]);
    }

    for (i = 0; i < jsonResponse.ALARMS.length; i++) {
        alarm = jsonResponse.ALARMS[i];
        dayName = dayList[i];

        allCheckBoxElem[i].checked = alarm.ON;
        disTimeElem(dayName + "Time", !alarm.ON);
    }
}

function refreshAlarms(e) {
    var data = new FormData();
    data.append("ALR", "GET");
    actionGet(data, applyAlarms);
}

function clickOnFlag(e) {
    // // console.log(e)
    // // console.log(e.target);
    // var currTimeElem = document.getElementById(e.target.value + "Time");
    // // console.log(currTimeElem);
    // currTimeElem.disabled = !e.target.checked;

    // changeAlarm(document.getElementById(e.target.value + "Time"));

    var dayIndex = dayList.indexOf(e.target.value);
    var data = new FormData();
    data.append("ALR", "SET");
    data.append("day", dayIndex);
    data.append("status", +e.target.checked);
    actionSend(data);

    disTimeElem(e.target.value + "Time", !e.target.checked);
}

function disTimeElem(name, flag) {
    var currTimeElem = document.getElementById(name);
    currTimeElem.disabled = flag;
}

refreshAlarms();