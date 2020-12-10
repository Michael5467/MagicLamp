const effectSelVar = document.getElementById("effectSelecter")
// effectSelVar.addEventListener("change", e => {
// 	alert("Effect changed!");
// 	// var effectSelValue = effectSelVar.val();
// 	// var effectSelText = effectSelVar.text();

// 	// alert(effectSelText + " = " + effectSelValue);
// });

const url = "/action.html";

effectSelVar.onchange = function() {
    var effectSelValue = effectSelVar.value;
    // var effectSelIndex = $("#effectSelecter option:selected").index();
    var effectSelIndex = effectSelVar.selectedIndex;
    // alert(effectSelValue);
    alert("Effect: " + effectSelValue + "=" + effectSelIndex);

    // var data = new FormData();
    // data.append("effect", effectSelVar.value);

    // const http = new XMLHttpRequest();
    // http.open("POST", url, true);
    // http.onreadystatechange = function() {//Call a function when the state changes.
    //     if (http.readyState == 4) {
    //         alert(http.responseText);
    //     }
    // }
    // http.send(data);
}			

const buttonOn = document.getElementById("fid-2");
buttonOn.addEventListener("click", e => {
    // alert("Button ON clicked.");

    var data = new FormData();
    data.append("power", "ON");

    const http = new XMLHttpRequest();
    http.open("POST", url, true);
    http.onreadystatechange = function() {//Call a function when the state changes.
        if (http.readyState == 4) {
            alert(http.responseText);
        }
    }
    http.send(data);
});

const buttonOff = document.getElementById("fid-1");
buttonOff.addEventListener("click", e => {
    // alert("Button OFF clicked.");

    var data = new FormData();
    data.append("power", "OFF");

    const http = new XMLHttpRequest();
    http.open("POST", url, true);
    http.onreadystatechange = function() {//Call a function when the state changes.
        if (http.readyState == 4) {
            alert(http.responseText);
        }
    }
    http.send(data);
});