var allSliders = document.getElementsByClassName("slider")
// console.log(allSliders);
for (var i = 0; i < allSliders.length; i++) {
    // console.log(allSliders[i]);
    allSliders[i].addEventListener("input", inputSlider);
    var idStr = allSliders[i].id;
    updateValueTest(idStr.substr(0, idStr.length - 6) + "Value", allSliders[i].value);
}

function inputSlider(e) {
    // console.log(e)
    // console.log(e.target.value);
    var idStr = e.target.id;
    // console.log(idStr);
    // console.log(idStr.substr(0, idStr.length - 6) + "Value");
    // var currValueElem = document.getElementById(idStr.substr(0, idStr.length - 6) + "Value");
    // console.log(currValueElem);
    // currValueElem.innerHTML = e.target.value;

    updateValueTest(idStr.substr(0, idStr.length - 6) + "Value", e.target.value)
}

function updateValueTest(name, value) {
    var currValueElem = document.getElementById(name);
    // console.log(currValueElem);
    currValueElem.innerHTML = value;
}
