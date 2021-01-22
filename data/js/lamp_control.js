const effectSelVar = document.getElementById("effectSelecter")

const url = "/action.html";

effectSelVar.addEventListener("change", changeEffect);

function changeEffect() {
    var effectSelValue = effectSelVar.value;
    var effectSelIndex = effectSelVar.selectedIndex;
    console.log("Effect: " + effectSelValue + "=" + effectSelIndex);

    var data = new FormData();
    data.append("MOD", effectSelIndex);

    actionSend(data);

    // const http = new XMLHttpRequest();
    // http.open("POST", url, true);
    // http.onreadystatechange = function() {//Call a function when the state changes.
    //     if (http.readyState == 4) {
    //         console.log(http.responseText);
    //     }
    // }
    // http.send(data);
}			
	

const buttonOn = document.getElementById("fid-2");
buttonOn.addEventListener("click", e => {
    // console.log("Button ON clicked.");

    var data = new FormData();
    data.append("PWR", "ON");

    actionSend(data);

    // const http = new XMLHttpRequest();
    // http.open("POST", url, true);
    // http.onreadystatechange = function() {//Call a function when the state changes.
    //     if (http.readyState == 4) {
    //         console.log(http.responseText);
    //     }
    // }
    // http.send(data);
});

const buttonOff = document.getElementById("fid-1");
buttonOff.addEventListener("click", e => {
    // console.log("Button OFF clicked.");

    var data = new FormData();
    data.append("PWR", "OFF");

    actionSend(data);

    // const http = new XMLHttpRequest();
    // http.open("POST", url, true);
    // http.onreadystatechange = function() {//Call a function when the state changes.
    //     if (http.readyState == 4) {
    //         console.log(http.responseText);
    //     }
    // }
    // http.send(data);
});

function actionSend(data) {
    const http = new XMLHttpRequest();
    http.open("POST", url, true);
    http.onreadystatechange = function() {//Call a function when the state changes.
        if (http.readyState == 4) {
            console.log(http.responseText);
        }
    }
    http.send(data);
}